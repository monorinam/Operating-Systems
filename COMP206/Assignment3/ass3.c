// Monorina Mukhopadhyay, ID 260364335
#include <stdio.h>
#include <string.h>
#define MAX_LEN 4096 //max length of sentence
#define ERROR -1 //error code
#define SUCCESS 0
#define MAX_TRY 5 //Ask for inputs a max of 5 times
// encryption and decryption method 
// This method does the caesar shift in both directions
// For encryption (with left shift) key must be negative
// For decryption (with right shift) key must be positive
// For a negative shift, the function gets the positive modulus
// So when A is left shifted by 3, -3 mod 26 is 23 mod 26 which is X
// Input	:	Line to be encrypted (char* )
//			:	key for encryption (-ve for left shift, +ve for right shift)
// Output	:	The Caesar shifted string (printed to output)	
int caesarshift(char line[], int key, char shifted[])
{
	int length;
	int letter;
	length = strlen(line);
	char first_letter; // saves the first letter (different for capital and lower case letters, either A or a)
	int newchar;

	for(int i = 0; i <  length; i++)
	{
		// Check if the character is a letter (if not leave it as is)
		if((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z'))
		{
			if(line[i] >= 'A' && line[i] <= 'Z')
			{
				first_letter = 'A';

			}
			if(line[i] >= 'a' && line[i] <= 'z')
			{
				first_letter = 'a';

			}
			//Convert from ASCII to alphabet index
			letter = line[i] - first_letter;
			newchar = (letter + key)%26;
			// Implement positive modulus
			// -x mod m is equivalent to x+m mod m
			while(newchar < 0)
				newchar = newchar + 26;
			//Convert back to ASCII value
			newchar = newchar + first_letter;
			// Convert from integer to character 
			shifted[i] = (char)newchar;
		}
		else
		{	
			//Non alphabet values, left as is
			shifted[i] = line[i];
		}
	}
	// Null terminate the string to prevent any garbage at the end
	shifted[length]='\0';
	return SUCCESS;

}
//Main method
int main(void)
{
	char line[MAX_LEN];
	char encrypted[MAX_LEN];
	char decrypted[MAX_LEN];
	int key;
	int trials;
	
	// Get the sentence
	trials = 0;
	printf("Please enter a sentence (max 4096 words)\n");
	while(fgets(line, MAX_LEN, stdin) == NULL && trials < MAX_TRY )
	{
		printf("Error with gettting sentence, try again.\n Please enter a sentence (max 4096 words) \n");
		trials = trials + 1;
	}
	if(trials == MAX_TRY)
		printf("Maximum tries reached, terminating");

	else
	{
		
		trials = 0;
		// Get key 
		// Check key is correct
		do
		{
			if(trials > 0)
			{
				printf("The key value is wrong! You entered %d ,Key should be between 1 and 25\n",key);	
			}
			printf("Please enter the key for left shifting\n");
			scanf("%d",&key);
			trials = trials + 1;
			
		} while((key>25 || key < 0) && trials < MAX_TRY);
		if(trials == MAX_TRY)
			printf("Maximum tries reached, terminated. \n");
		else
		{
			//Display original message
			printf("The original message \t %s \n",line);
			//Encryption here (left shift by key)
			if(caesarshift(line,(-1*key),encrypted) != ERROR)
				printf("The encrypted line is \t %s \n",encrypted);
			// Decryption here (right shift by key)
			if(caesarshift(encrypted,key,decrypted) != ERROR)
				printf("The decrypted line is \t %s \n",decrypted);
		}
	}
}