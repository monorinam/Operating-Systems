#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define MAX_N 60
#define SUCCESS 1
// typedef union {
//   float f;
//   struct {
//     unsigned int fraction : 23;
//     unsigned int exponent : 8;
//     unsigned int sign : 1;
//   } ieee;
// } number;
typedef union{
	float f;
	int i;
}number;

void part1(float num[], int length)
{	

	float product;
	int K = 0;
	int flag = 0; 
	printf("The numbers are:\n");
	for (int i = 0; i < length ; i++)
	{
		printf("Number %d %e \n",i,num[i]);
	}
	while(!flag)
	{
		product = num[0];
		for(int i = 1; i < length; i++)
		{
			product *= num[i];
		}
		if(product == INFINITY){
			for(int i = 0; i < length; i++)
			{
				num[i] /= 10;
			}
			K += length;
		}
		else if(product == 0)
		{
			for(int i = 0; i < length; i++)
			{
				num[i] *= 10;
			}
			K -= length;
		}
		else{
			flag = 1;
			printf("The product is: %.6e times 10 to the power of %d \n", product,K);
		}
		if(abs(K) == INFINITY)
		{
			flag = 1;
		}
	}

}

int main(void)
{	
	float num[3] = {powf(10,25), powf(10,25), powf(10,12)};
	part1(num,3);
	return SUCCESS;
}