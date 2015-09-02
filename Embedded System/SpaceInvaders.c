// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10
// 0-127 0-159
// Last Modified: 3/6/2015 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "images.h"
#include "Sound.h"
//#include "structs.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

int lives = 3;
int score;
int k;											//offset for shell animation
int gameover = 1;
#define numOfChar 38
#define dtime 40

typedef enum {dead, dying, alive, paused} status;
struct state {
	int x;
	int y;
	int w;
	int h;
	const unsigned short *image;
	status life;
	int dietime;
	int doublehit;
	int play;
	int song;
};
typedef struct state stype;
stype characters[numOfChar];

stype *pt;
int i;

#define Shoot 0
#define GetsHit 1
#define MarioHoo 2
#define hi 3

//initial state of everything on the screen
void charInit(void){

//first row of aliens
for (i=0; i <= 3; i++){
	pt = &characters[i];
	pt -> x = i*30;
	pt -> y = 27;
	pt -> life = alive;
	pt -> image = Bowser1;
	pt -> w = 24;
	pt -> h = 28;
	pt -> dietime = dtime;
	pt -> doublehit = 0;
	pt -> play = 0;
	pt -> song = MarioHoo;
	};
	
//second row of aliens
for (i=0; i <=5 ; i++){
	pt = &characters[i+6];	
	pt ->x = i*20;
	pt ->y = 58;
	pt ->life = alive;
	pt -> image = Yoshi1;
	pt -> w = 17;
	pt -> h = 32;
	pt -> dietime = dtime;
	pt -> doublehit = 0;
	pt -> play = 0;
	pt -> song = MarioHoo;
	}
//third row of aliens	
	for (i=0; i <= 5; i++){
	pt = &characters[i+12];	
	pt ->x = i*20;
	pt ->y = 75;
	pt ->life = alive;
	pt -> image = Fishi1;
	pt -> w = 18;
	pt -> h = 18;
	pt -> dietime = dtime;
	pt -> doublehit = 0;
	pt -> play = 0;
	pt -> song = MarioHoo;
	}
//Mario
	pt = &characters[18];	
	pt ->x = 52;
	pt ->y = 159;
	pt ->life = alive;
	pt ->image = Mario;
	pt -> w = 19;
	pt -> h = 19;
	pt -> dietime = dtime;
	pt -> play = 0;
	pt -> song = MarioHoo;
//three bunkers
	for (i=0; i <= 2; i++){
	pt = &characters[i+19];		
	pt ->x = i*40 + 15;
	pt ->y = 120;
	pt ->life = alive;
	pt -> image = Mushroom1;
	pt -> w = 24;
	pt -> h = 10;
	pt -> dietime = 3;
	pt -> play = 0;
	pt -> song = 3;
	}
	//shell	
	for (i=0; i<=9; i++){
	pt = &characters[22 + i];	
	pt ->x = 52;
	pt ->y = 158;
	pt ->life = dead;
	pt ->image = Shell1;
	pt -> w = 20;
	pt -> h = 20;
	pt -> dietime = dtime/2;
	pt -> play = 0;
	pt -> song = 0;
	}
	//bomb
	for (i=0; i<=5; i++){
	pt = &characters[32 + i];	
	pt ->x = 52;
	pt ->y = 158;
	pt ->life = dead;
	pt ->image = Fireball1;
	pt -> w = 16;
	pt -> h = 16;
	pt -> dietime = dtime/2;
	pt -> play = 0;
	pt -> song = 3;
	}
	
	lives = 3;
	score = 0;
	gameover = 1;
	k = 0;
}

void systickINIT(void){
	NVIC_ST_CTRL_R = 0; 
	NVIC_ST_RELOAD_R = 500000; 
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x7;
	
}
//7256

const unsigned short *Bowser[] = {Bowser1, Bowser2};
const unsigned short *Yoshi[] = {Yoshi1, Yoshi2};
const unsigned short *Fishi[] = {Fishi1, Fishi2};
const unsigned short *E10anime[] = {SmallEnemy10pointA, SmallEnemy10pointB};
const unsigned short *dyingAnime[] = {enemyDying, SmallEnemy10pointB};
const unsigned short *dyingBowser[] = {BowserDie1, BowserDie2};
const unsigned short *dyingYoshi[] = {YoshiDie1, YoshiDie2};
const unsigned short *dyingFishi[] = {FishiDie1, FishiDie2};
const unsigned short *dyingShellAnime[] = {ShellDie1, ShellDie2};
const unsigned short *dyingBombAnime[] = {enemyDying, SmallEnemy10pointB};
const unsigned short *dyingMarioAnime[] = {MarioDie1, MarioDie2};
const unsigned short *shellAnime[] = {Shell1, Shell2, Shell3, Shell4};
const unsigned short *bombAnime[] = {Fireball1, Fireball2, Fireball1, Fireball2};
const unsigned short *bunkerAnime[] = {DeadMushroom, Mushroom4, Mushroom3, Mushroom2}; //first one needs to be a blank
int readyToInterrupt;
int j;											//offset for alien animation

int a;											//offset for iterrating through characters
int b;											//offset for iterraying through characters
stype *pt2;									//pointer for iterrating through characters
stype *pt3;									//pointer for iterrating through characters
int flaptime = 15;					//speed of animation
int shelltime = 5;					//speed of animation for shell
int alienShiftTime = 20;		//speed of aliens moving left or right
int marioShiftTime = 2;			//speed of mario moving left or right
int leftOrRight = 1;				//direction aliens are moving
int edge;										//aliens move down only when they hit the edge of the screen
int buttonFire;							//button for launching shell
uint32_t random = 0;
int  bombCount;


void SysTick_Handler(void) {
if (readyToInterrupt == 1) {
	readyToInterrupt = 0;
/*Each alien animates between two different images. There are three types of aliens.
	There's another image that briefly shows when you shoot the alien.
	Then it goes away
	*/
	for (a = 0; a <= 5; a++) {
		pt2 = &characters[a];
		if (pt2->life == alive) {
			pt2->image = Bowser[j];
		}
		if (pt2->life == dying) {
				if (pt2->image == DeadBowser) {
					pt2->life = dead;
					score = score + 30;
					}
				pt2->dietime--;
				pt2->image = dyingBowser[j];
				if (pt2->dietime == 0) {
					pt2->image = DeadBowser;
					pt2->dietime = dtime;				
					}
				}
	}
	for (a = 6; a <= 11; a++) {
		pt2 = &characters[a];
		if (pt2->life == alive) {
			pt2->image = Yoshi[j];
		} 
		if (pt2->life == dying) {
				if (pt2->image == DeadYoshi) {
					pt2->life = dead;
					score = score + 20;
					}
				pt2->dietime--;
				pt2->image = dyingYoshi[j];
				if (pt2->dietime == 0) {
					pt2->image = DeadYoshi;
					pt2->dietime = dtime;				
					}
				}
	}
	for (a = 12; a <= 17; a++) {
		pt2 = &characters[a];
		if (pt2->life == alive) {
			pt2->image = Fishi[j];
		} 
		if (pt2->life == dying) {
				if (pt2->image == DeadFishi) {
					pt2->life = dead;
					score = score + 10;
					}
				pt2->dietime --;
				pt2->image = dyingFishi[j];
				if (pt2->dietime == 0) {
					pt2->image = DeadFishi;
					pt2->dietime = dtime;				
					}
				}
	}
	//animation for shell
		for (a = 22; a <= 31; a++) {
		pt2 = &characters[a];
		if (pt2->life == alive) {
			pt2->image = shellAnime[k];
		} 
		if (pt2->life == dying) {
				if (pt2->image == DeadShell) {
					pt2->life = dead;
					}
					pt2->image = DeadShell;
				}
	}
			//animation for bomb
		for (a = 32; a <= 37; a++) {
		pt2 = &characters[a];
		if (pt2->life == alive) {
			pt2->image = bombAnime[k];
		} 
		if (pt2->life == dying) {
				if (pt2->image == DeadBomb) {
					pt2->life = dead;
					}
				pt2->dietime--;
				pt2->image = dyingBombAnime[j];
				if (pt2->dietime == 0) {
					pt2->image = DeadBomb;
					pt2->dietime = dtime/2;				
					}
				}
	}
		//animation for mario 
		pt2 = &characters[18];
		if (pt2->life == alive) {
			pt2->image = Mario;
		}
		if (pt2->life == dying) {
				if (pt2->image == DeadMario) {
					pt2->life = dead;
					}
				pt2->dietime--;
				pt2->image = dyingMarioAnime[j];
				if (pt2->dietime == 0) {
					pt2->image = DeadMario;							///change to deadmario**************
					pt2->dietime = dtime;				
					}
				}
		if (pt2->life == dead && lives > 0) {
			pt2->life = alive;
			lives--;
		}
	
	//end of animation
	
	
	
	//This shifts the aliens to the left or right. 
	//but first, it checks to see if any of the aliens are on the edge of the screen
	if (alienShiftTime == 0) {
	edge = leftOrRight;
	for (a = 0; a <=17; a++) {
		pt2 = &characters[a]; 
		if ((pt2->life != dead) && (pt2->x == 0)){
			leftOrRight = 1;
		}
		if ((pt2->life != dead) && (pt2->x == (128 - pt2->w))){
			leftOrRight = -1;
			}
		}
	edge = edge + leftOrRight;
	for (a = 0; a<=17; a++) {
		pt2 = &characters[a];
		if (edge != 0) {											
			pt2->x = pt2->x + leftOrRight;		//not on edge, move left/right
		} else if (edge ==0) {
			pt2->y ++;												//on edge, move downward
			}			
		}
	}
	//end of alien shift
	
	//fire shell
	//first check if there's already a shell in the air, if so, set pointer to the next shell
	a=0, pt2 = &characters[22];
	while ((a <= 9) && (pt2->life == alive)){
		a++;	
		pt2 = &characters[22+a];
		}
	//produce shell when button is pressed
	if (buttonFire == 0x00 && GPIO_PORTF_DATA_R == 0x10) {
		pt2->life = alive;
		pt2->x = characters[18].x;
		pt2->y = (characters[18].y - characters[18].h);	
		pt2->play = 1;
	}
	buttonFire = GPIO_PORTF_DATA_R;
		//prevents multiple shells from overlapping
	for (b = 0; b <= 5; b++) {									
		pt3 = &characters[22+b];
		if (((pt2->y - pt3->y) > 0) && ((pt2->y - pt3->y) < pt2->h) && (pt3->life == alive)){
			pt2->life = dead;
				}
			}
	//move shells upward
	for (a = 0; a <= 9; a++){
		pt2 = &characters[22+a];
		if (pt2->life == alive){
			pt2->y--;

		}
	}
	 
	//kill shells if they go off the screen
	for (a = 0; a <= 9; a++){
		pt2 = &characters[22+a];
		if (pt2->y < 0) {
			pt2->life = dead;
		}
	}
	//end of fire shell
	
	//drop bomb
	//set pointer to next bomb to drop
	a = 0, pt2 = &characters[32];
	while (a <= 5 && (pt2->life == alive)){
		a++;
		pt2 = &characters[32+a];
	}
	//produce bomb to drop from a randomly chosen alien
	if ((pt2->life == dead) && (Random32() < 400000000)){
	random = Random()%18;
		if (characters[random].life == alive){
		pt2->life = alive;
		pt2->x = characters[random].x;
		pt2->y = (characters[random].y + pt2->h);
		}
	}
	//dont let bomb overlap with aliens
	for(b = 0; b <= 17; b++) {
		pt3 = &characters[b];
		if (((pt2->y - (pt2->h)/2) <= pt3->y) && pt3->life == alive) {
			pt2->y = pt2->y + pt3->h;
		}
	}
	//prevents multiple bombs from overlapping
	for (b = 0; b <= 5; b++) {
				pt3 = &characters[32+b];
				if (((pt3->y - pt2->y) >= 0) && ((pt3->y - pt2->y) < pt2->h) && (pt3->life == alive) && (a != b)){
						pt2->life = dead;
				}
			}	
	//move bombs downward
	for (a = 0; a<= 5; a++){
		pt2 = &characters[32+a];	
		if (pt2->life == alive && bombCount == 0){
		pt2->y++;		
		}

	}
	for (a = 0; a<= 5; a++) {
		pt2 = &characters[32+a];
			pt2->doublehit = 0;
	}
	//clear the bombs that miss
	for (a = 0; a <= 5; a++){
		pt2 = &characters[32+a];
		if (pt2->y == (159 + pt2->h) && pt2->life == alive){
			pt2->life = dead;
		}
	}
	
	//end of drop bomb
	
	//move mario
	if (ADC_In() < 60 && characters[18].x >= 0 && marioShiftTime == 0) {
	characters[18].x --;
	}
	if (ADC_In() > 220 && (characters[18].x <= (127 - characters[18].w)) && marioShiftTime == 0) {
	characters[18].x ++;
	}
	//end of move mario
	
	//collision detection
			//	for missiles hitting aliens
	for (a = 22; a <= 31; a++){
		pt2 = &characters[a];
		if (pt2->life == alive) {
		for (b=0; b <= 17; b++){
			pt3 = &characters[b];
			if(pt3->life == alive) {
			if((((pt2->y - pt2->h) == (pt3->y - 1)) | ((pt2->y - pt2->h) == (pt3->y - 2))) 
				&& (((pt2->x >= pt3->x) && (pt2->x < (pt3->x + pt3->w))) | (((pt2->x + pt2->w) >= pt3->x) && ((pt2->x + pt2->w) < (pt3->x + pt3->w))))){
				if (characters[b-1].doublehit != 1) {
				pt3->life = dying;
				pt2->life = dying;
				pt3->doublehit = 1;
			//	pt3->play = 1;
				}
				}
			}
			}
		}
	}
	for (a = 0; a <= 17; a++) {
		pt2 = &characters[a];
		pt2->doublehit = 0; 
	}

	//for missiles hitting bunkers
		for (a = 22; a <= 31; a++){
		pt2 = &characters[a];
		if (pt2->life == alive) {
		for (b=19; b <= 21; b++){
			pt3 = &characters[b];
			if(pt3->life == alive) {
				if(((pt2->y -pt2->h) == (pt3->y -2)) && 
					((pt2->x + pt2->w/2) >= pt3->x) && ((pt2->x + pt2->w/2) < (pt3->x + pt3->w))){
					if (pt3->dietime >= 0) {
					pt3->image = bunkerAnime[pt3->dietime];
					pt3->dietime--;
					pt2->life = dying;	
					} else {
						pt3->life = dead;
					}	
				}
			}
		}
		}
	}
			//for bombs hitting bunkers
		for (a = 32; a <= 37; a++){
		pt2 = &characters[a];
		if (pt2->life == alive) {
		for (b=19; b <= 21; b++){
			pt3 = &characters[b];
			if(pt3->life == alive) {
				if((pt2->y == (pt3->y - pt3->h + 2)) && 
					(((pt2->x >= pt3->x) && (pt2->x < (pt3->x + pt3->w))) | (((pt2->x + pt2->w) >= pt3->x) && ((pt2->x + pt2->w) < (pt3->x + pt3->w))))){
					if (pt3->dietime >= 0) {
					pt3->image = bunkerAnime[pt3->dietime];
					pt3->dietime--;
					pt2->life = dying;	
					} else {
						pt3->life = dead;
					}
				}
			}
		}
		}
	}
	//Bomb hitting mario
	for (a = 32; a <= 37; a++){
		pt2 = &characters[a];
		if (pt2->life == alive) {
			pt3 = &characters[18];
			if(pt3->life == alive) {
				if((pt2->y == (pt3->y - pt3->h + 2)) && 
					(((pt2->x >= pt3->x) && (pt2->x < (pt3->x + pt3->w))) | (((pt2->x + pt2->w) >= pt3->x) && ((pt2->x + pt2->w) < (pt3->x + pt3->w))))){
					pt2->life = dying;	
					pt3->life = dying;
					pt3->play = 1;
				}
			}		
		}
	}
		
	//end collision detection
	
	//check if any enemies left
	gameover = 0;
	for (a = 0; a < 17; a++) {
		pt2 = &characters[a];
		if (pt2->life == alive) {
			gameover++;
		}
	}
	//end of check if any enemies left

	//timing
	if (alienShiftTime == 0) {
		alienShiftTime = 20;	
	}
	alienShiftTime--;
	if (bombCount == 0) {
		bombCount = 5;
	}
	bombCount --;
	if (marioShiftTime == 0) {
		marioShiftTime = 2;
	}
	marioShiftTime--;
	if (flaptime == 0) {
	j = (j + 1)%2;
	flaptime = 15;
	}
	flaptime --;
	if (shelltime == 0) {
	k = (k +1)%4;
	shelltime = 5;
	}
	shelltime --;
			//end of timing
}			//end of if-statement to execute systick
}			//end of systick

void PortF_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x20;			//Port F is the switch
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTF_DIR_R |= 0x00;      
  GPIO_PORTF_AFSEL_R = 0x00;     
  GPIO_PORTF_DEN_R |= 0xFF;
	
	SYSCTL_RCGC2_R |= 0x2;				// Activate Port B
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTB_DEN_R |= 0xFF;
	GPIO_PORTB_DIR_R |= 0xFF;
	GPIO_PORTB_AFSEL_R &= ~0xFF;
	GPIO_PORTB_AMSEL_R &= ~0xFF;
	GPIO_PORTB_PCTL_R &= ~0xFF;
}
unsigned long TimerCount;

void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  TIMER2_CTL_R = 0x00000000;   // 1) disable timer2A
  TIMER2_CFG_R = 0x00000000;   // 2) 32-bit mode
  TIMER2_TAMR_R = 0x00000002;  // 3) periodic mode
  TIMER2_TAILR_R = period-1;   // 4) reload value
  TIMER2_TAPR_R = 0;           // 5) clock resolution
  TIMER2_ICR_R = 0x00000001;   // 6) clear timeout flag
  TIMER2_IMR_R = 0x00000001;   // 7) arm timeout
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x02000000; // 8) priority 0
  NVIC_EN0_R = 1<<23;          // 9) enable IRQ 23 in
  TIMER2_CTL_R = 0x00000001;   // 10) enable timer2A
}

void Timer3_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x08;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  TIMER3_CTL_R = 0x00000000;   // 1) disable timer2A
  TIMER3_CFG_R = 0x00000000;   // 2) 32-bit mode
  TIMER3_TAMR_R = 0x00000002;  // 3) periodic mode
  TIMER3_TAILR_R = period-1;   // 4) reload value
  TIMER3_TAPR_R = 0;           // 5) clock resolution
  TIMER3_ICR_R = 0x00000001;   // 6) clear timeout flag
  TIMER3_IMR_R = 0x00000001;   // 7) arm timeout
  NVIC_PRI8_R = (NVIC_PRI8_R&0x02FFFFFF)|0x00000000; // 8) priority 0
  NVIC_EN0_R = 1<<(35-31);          // 9) enable IRQ 23 in
  TIMER3_CTL_R = 0x00000001;   // 10) enable timer2A
}


void Timer2A_Handler(void){
	TIMER2_ICR_R = 0x1;
	SoundOut();
}

uint32_t x;
int main(void){
	TExaS_Init(SSI0_Real_Nokia5110_Scope);
	Output_Init();
	charInit();
	//ST7735_DrawBitmap(0, 160, splash, 128, 160);
	ADC_Init(); 
	PortF_Init();
	systickINIT();
	Random_Init(34);
	Timer2_Init(7256);
//7256
	while(1) {
		
	//home screen
	//ST7735_FillScreen(0x0400);
/*	if (1) {
		while (GPIO_PORTF_DATA_R == 0){
			ST7735_DrawBitmap(0, 159, splash, 128, 160);
	}
			while (GPIO_PORTF_DATA_R != 0){
	}
}*/
	ST7735_DrawBitmap(0, 159, splash, 128, 160);
	while (GPIO_PORTF_DATA_R == 0){
		ST7735_DrawBitmap(0, 159, splash, 128, 168);
	//	ST7735_SetCursor(3, 2);
		if ((score != 0) && gameover != 0) {
		ST7735_SetCursor(3, 6);
		ST7735_OutString("Awwwww, you lost!");
		ST7735_SetCursor(3, 7);
		ST7735_OutString("Score: ");
		LCD_OutDec(score);
		}
		if (gameover == 0) {
		ST7735_SetCursor(1, 6);
		ST7735_OutString("You Win!");
		ST7735_SetCursor(3, 7);
		ST7735_OutString("Score: ");
		LCD_OutDec(score);		
		}
		Random();
		Random32();
ST7735_DrawBitmap(0, 160, splash, 128, 168);
	}
				while (GPIO_PORTF_DATA_R != 0){
				}

	ST7735_FillScreen(0x0400);
		charInit();
	//end of home screen

	while (lives > 0 && gameover != 0){
		characters[32].play = 1;
		readyToInterrupt = 1;
		for (i = 0; i < numOfChar; i++) {
		pt = &characters[i];
			if (pt ->life != dead){
				ST7735_DrawBitmap(pt ->x, pt ->y, pt ->image, pt ->w, pt ->h);			
					}						
		}
	}
	}
}
