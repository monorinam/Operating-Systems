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
int caesarshift(char* line, int key)
{
	int length;
	int letter;
	length = strlen(line);
	char shifted[length];
	char first_letter;
	int newchar;

	for(int i = 0; i <  length; i++)
	{
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
			if(newchar < 0)
				newchar = newchar + 26;
			newchar = newchar + first_letter;
			shifted[i] = (char)newchar;
		}
		else
		{	
			shifted[i] = line[i];
		}
	}
	printf("The encrypted line is \t %s \n",shifted);
	return SUCCESS;

}
// decryption method
int main(void)
{
	char* line = NULL;
	size_t line_n = 0;
	char* encrypted;
	char* decrypted;
	int key;
	printf("Please enter a sentence (max 4096 words)\n");
	if(getline(&line, &line_n, stdin) == -1 )
		perror("Error with gettting sentence");
	else{
		printf("Please enter the key for left shifting\n");
		scanf("%d",&key);
		printf("The original message \t %s \n",line);
		if(caesarshift(line,(-1*key)) == ERROR)
			perror("Encryption failed \n");
		//if(decrypt(line, key) == ERROR);
		//	perror("Decryption failed \n");
	}
}