#include <stdio.h>
#include <stdlib.h>
void main(int argc ,char * args[])
{
   FILE* fi, * fo;
   if(argc != 3)
  {
  	printf("%d\n",argc);
  	printf("Please input  input-filename and output-filename\n");
  }

  if((fi = fopen(args[1] , "r"))== NULL)    //open symbol file
  {
  	printf("%s\n",args[1]);
    printf("open input file error\n");
  	exit(0);
  }

  if((fo = fopen(args[2] , "r"))== NULL)    //open output file
  {
    printf("%s\n",args[2]);
    printf("open output file error\n");
    exit(0);
  }

  int temp1,temp2,i = 0;
  while(1)
  {
  	 i ++;
     temp1 = fgetc(fi);
     temp2 = fgetc(fo);
     if(temp1 == EOF && temp2 == EOF)
     {
     	printf("%s is same as  %s\n",args[1],args[2]);
     	break; 
     }
     else
     {
       if(temp1 != temp2)
       {
       	printf("the number %d btye not equal\n",i);
        break;
       }
     }
  }
 
}
