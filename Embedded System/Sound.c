// Sound.c
// Runs on any computer
// Sound assets based off the original Space Invaders 
// Import these constants into your SpaceInvaders.c for sounds!
// Jonathan Valvano
// November 17, 2014
#include <stdint.h>
#include "Sound.h"
#include "DAC.h"
#include "tm4c123gh6pm.h"
#include "SpaceInvaders.h"
#include "Tunes.h"


	struct SoundandSize {
	const unsigned char *music;
	int size;
	int timesToPlay;
	int ttpCounter;
	};
typedef struct SoundandSize Sounds;
	
Sounds SoundArray[] = {
{shoot,4080,1,1},{Mario_Hoo,5792,1,1},{Mario_Hoo,10176,1,1},{bg,26032,1,1},{bg,27360,1,1}
};

int	Index;
void SoundOut(void) {
	int D;
	int a = 0;
	int oneSoundOnly = 0;
	//see if any characters have a sound to play
	for (a = 0; a < 37; a++) {
	if (characters[a].play == 1 && oneSoundOnly == 0) {
	//play sound
		oneSoundOnly = 1;
	D = characters[a].song;
	if (SoundArray[D].ttpCounter > 0) {
	GPIO_PORTB_DATA_R = SoundArray[D].music[Index];
	Index = (Index + 1)%SoundArray[D].size;
	//how many times to play the sound
	if (Index == (SoundArray[D].size - 1)) {
		SoundArray[D].ttpCounter--;
		}
	//****
	}
	if (SoundArray[D].ttpCounter <= 0) {
		characters[a].play = 0;
		SoundArray[D].ttpCounter = SoundArray[D].timesToPlay;
	}
	}
	}
}

