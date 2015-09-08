# projects

/*  Blip lanaguage
*   C++ program which serves as an interpreter for the toy language "blip"
*   main.cpp can be edited to specifiy the name of the blip file to read from
*   the syntax is easy to read. reading through the syntax you can see that for example,
*   arithemtic is done like this:
*   instead of writing 5 + 4
*   it's + 5 4
*   and for 5 + 4 - 3
*   its - + 5 4 3
*   This allows us to easily create a BST for us to execute the expressions
*   For while loops we call it a "do" loop, with the end of the loop signified
*   by do spelled backwords, "od"
*   same for if statements. if <conditional expresion> ... statements... fi
*   Look at test5.blip to see a function which returns the factorial of x.
*   The function is defined to make recursive calls to calculate the factorial.
*   Compile the program using test1.blip and look at the code in that file
*   to see some exploration of the blip language. 
*/


/*  Spell Checker
*   C++ program which serves as a spell checker. Specify a text file and it
*   will compare it to words in a "dictionary" file.
*   look at main.cpp 
*   Look at the definition of generalTest3, for example. There are two lines
*   for readFile. One for the name of the article to check and one for
*   a dictionary.
*   Run the program and watch as it does it's intended function.
*   My initial implementation of the program involved checking each word in the
*   article with each word in the dictionary to see if it found a match.
*   I later optimized it by creating an array of pointers to each word in the dictionary
*   and from there, using a binary search to see if there is a match in the dictionary
*/
