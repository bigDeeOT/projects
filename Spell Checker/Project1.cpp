/*
* Project1.cpp
*
* Name: Ima Student
* EE312 Fall 2014
* SpellCheck
*/

#include <stdio.h> // provides declarations for printf and putchar
#include <stdint.h> // provides declarations for int32_t uint32_t and the other (new) standard C types

/* All of your code must be in this file. Please no #includes other than standard system headers (ie.., stdio.h, stdint.h)
*
* Many students find it helpful to declare global variables (often arrays). You are welcome to use
* globals if you find them helfpul. Global variables are by no means necessary for this project.
*/

/* You must write this function (spellCheck). Do not change the way the function is declared (i.e., it has
* exactly two parameters, each parameter is a standard (mundane) C string (see SpellCheck.pdf).
* You are expected to use reasonable programming style. I *insist* that you indent
* reasonably and consistently in your code. I strongly encourage you to avoid big functions
* So, plan on implementing spellCheck by writing two or three other "support functions" that
* help make the actual spell checking easier for you.
* There are no explicit restictions on using functions from the C standard library. However,
* for this project you should avoid using functionality from the C+=1 standard libary. You will
* almost certainly find it easiest to just write everything you need from scratch!
*/

uint32_t numOfDicWords;
char *pointers[2000000];				//array of pointers


char isWord(char art[]) {
	if ((((art[0] >= 'A') && (art[0] <= 'Z')) | ((art[0] >= 'a') && (art[0] <= 'z')))				// if first character is inside range of letters
		&& (((art[1] >= 'A') && (art[1] <= 'Z')) | ((art[1] >= 'a') && (art[1] <= 'z')))) {			//is second character inside range of letters
		return 1;
	}
	return 0;
}

char *newArticlePt(char art[]) {
	while (isWord(art)) {						//if the pt is already on a word, skip to the end of the word
		art+=1;
	}
	while (!isWord(art) && art[0] != 0) {		//if pt is not on a word, go to the first word	
		art+=1;
	}
	return art;
}
char *newDictionPt(char dic[]) {
	while ((dic[0] != 0xA) && (dic[0] != 0)) {
		dic+=1;
	}
	if (dic[0] != 0) {
		dic+=1;
	}
	return dic;
}
char wordLength(char words[]) {
	char a = 0;
	while (((words[0] >= 0x41) && (words[0] <= 0x5A)) | ((words[0] >= 0x61) && (words[0] <= 0x7A))) {
		a+=1;
		words+=1;
	}
	return a;
}
char wordLengthD(char words[]) {
	char a = 0;
	while ((words[0] != 0xA) && (words[0] != 0)) {
		a+=1;
		words+=1;
	}
	return a;
}
char isSameLetter(char art[], char dic[]) {
	if ((dic[0] >= 'A') && ((art[0] == dic[0]) | (dic[0] - art[0] == 0x20) | (art[0] - dic[0] == 0x20))) {
		return 1;
	}
	return 0;
}

uint32_t wordCount(char dic[]) {
	uint32_t i;
	for (i = 0; dic[0] != 0; i++) {
		dic = newDictionPt(dic);
	}
	return i;
}

char leftRightSame(char art[], char dic[]) {
	char a, d, aLetter, dLetter, lettersChecked;
	a = wordLength(art);
	d = wordLengthD(dic); 
	for (lettersChecked = 0; (lettersChecked < a) && (lettersChecked < d) && isSameLetter(art + lettersChecked, dic + lettersChecked); lettersChecked += 1) {
																								//see if any of the letters are different
	}
	if ((lettersChecked == d) && (a == d)) {													//word matched
		return 0;
	}
	if (lettersChecked == d) {																	//ex: article word is firetruck, dictionary word is fire											
		return 1;																				//word is to the right of midpoint
	}
	if (lettersChecked == a) {																	//ex: article word is fire, dictionary word is firetruck	
		return -1;																				//word is to the left of midpoint
	}
	aLetter = art[lettersChecked];
	dLetter = dic[lettersChecked];
	if ((aLetter >= 'a') && (aLetter <= 'z') && (dLetter >= 'A') && (dLetter <= 'Z')) {			//article word is lowercase, dictionary is capitalized,
		dLetter += 0x20;
	}
	if ((aLetter >= 'A') && (aLetter <= 'Z') && (dLetter >= 'a') && (dLetter <= 'z')) {			//article word is captialized, dictionary is lowercase,
		aLetter += 0x20;
	}
	if ((aLetter - dLetter) < 0) {																//word is to the left of midpoint
		return -1;
	}
	else if ((aLetter - dLetter) > 0) {															//word is to the right of midpoint
		return 1;
	}
	

}

//this is the slow and simple version
char notInDiction1(char art[], char dic[]) {
	char a, d, letter;
	a = wordLength(art);
	while (*dic != 0) {
		d = wordLengthD(dic);
		for (letter = 0; (a == d) && (letter < d) && isSameLetter(art + letter, dic + letter); letter += 1) {
			//just check each letter
		}
		if (letter == d) {		//if every letter matched
			return 0;
		}
		dic = newDictionPt(dic);
	}
	return 1;
}

char notInDiction(char art[], char dic[]) {
	uint32_t right = numOfDicWords - 1;
	uint32_t left = 0;
	uint32_t middle = (right - left)/2;
	char *midpoint = dic;

	while ((right - left) > 3) {												//guess where word is until there's only 3 words left to check
		dic = pointers[middle];													//update dictionary pointer (where we guess the word will be)										
		if (leftRightSame(art, dic) < 0) {										//word is to the left of midpoint so update boundaries for a more refined guess
			right = middle;
			middle = left + ((middle - left) / 2);
		}
		else if (leftRightSame(art, dic) > 0) {									//word is to the right of midpoint so update boundaries for a more refined guess
			left = middle;
			middle = middle + ((right - middle) / 2);
		}
		else if (leftRightSame(art, dic) == 0) {								//if our guess is correct
			return 0;
		}
	}
	dic = pointers[left];														
	for (uint32_t i = 0; (dic[0] != 0) && (i <= (right - left)); i+=1 ) {		//check the next few words past the left hand boundry 
		if (leftRightSame(art, dic) == 0) {
			return 0;
		}
		dic = newDictionPt(dic);
	}
	return 1;
}



void printWord(char art[]) {
	char a = wordLength(art);
	for (char i = 0; i < a; i += 1) {
		printf("%c", art[0]);
		art += 1;
	}
	printf("%c", '\n');
}



void spellCheck(char article[], char dictionary[]) {
	char *articlePt = article;
	char *dictionPt = dictionary;
	if (wordLength(articlePt) < 2) {						//move pointer to start of first word
		articlePt = newArticlePt(articlePt);
	}
	numOfDicWords = wordCount(dictionary);
	
	for (int i = 0; dictionPt[0] != 0; i += 1) {
	 pointers[i] = dictionPt;
	 dictionPt = newDictionPt(dictionPt);
	}
	while (*articlePt != 0) {								//itterate through article
		if (notInDiction(articlePt, dictionary)) {
			printWord(articlePt);
		}
		articlePt = newArticlePt(articlePt); 				//Go to next word
	}
}
