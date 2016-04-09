#include<stdio.h>
void main()
{
	unsigned char ua = -128;
	char b =  - 128;
	int i = ua;
	int j = b;
        unsigned int ui = ua;
	unsigned int uj = b;	
	printf("ua = %x , b = %x , i = %x , j = %x , ui = %x , uj = %x\n" ,ua,b,i,j,ui,uj );
}
