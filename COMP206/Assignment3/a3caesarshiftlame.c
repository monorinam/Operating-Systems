// Monorina Mukhopadhyay, ID 260364335
#include <stdio.h>
#include <string.h>
#define MAX_LEN 4096 //max length of sentence
#define ERROR -1 //error code
#define SUCCESS 0
// encryption and decryption method method
// For encryption (with left shift) key must be negative
// For decryption (with right shift) key must be positive
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
	return SUCCESS;

}
//Main method
int main(void)
{
	//Note the line is a pointer because getline requires a pointer, not an array
	char* line_ptr = NULL;
	size_t line_n = 0;
	char encrypted[MAX_LEN];
	char decrypted[MAX_LEN];
	char line[MAX_LEN];
	int key;
	printf("Please enter a sentence (max 4096 words)\n");
	if(getline(&line_ptr, &line_n, stdin) == -1 )
		perror("Error with gettting sentence");
	else{
		// Convert the line pointer to a line array, for consistency 
		// and to meet specifications of assignment
		for(int i = 0; i < strlen(line_ptr) && i < MAX_LEN; i++)
		{
			line[i] = line_ptr[i];
		}
		printf("Please enter the key for left shifting\n");
		scanf("%d",&key);
		printf("The original message \t %s \n",line);
		//Encryption here (left shift by key)
		if(caesarshift(line,(-1*key),encrypted) != ERROR)
			printf("The encrypted line is \t %s \n",encrypted);
		// Decryption here (right shift by key)
		if(caesarshift(encrypted,key,decrypted) != ERROR)
			printf("The decrypted line is \t %s \n",decrypted);
	}
}