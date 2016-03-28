#include <stdio.h>
#include <stdlib.h>
#define m 8
#define mMASK 0xFF
#define norMASK 0x80

static int symbol_num; // variety  number of  symbols

struct Bit
{
  unsigned char UMSB:1;  //MSB of up boundary 
  unsigned char USMSB:1; //second MSB of up boundary
  unsigned char LMSB:1;  //MSB of low boundary
  unsigned char LSMSB:1; //second MSB of low boundary
  unsigned char sendbit:1;
  unsigned char empty:3;
};


void send(struct Bit bit)
{
  if(bit.sendbit)
  {
  	printf("*****send 1\n");
  }
  else
  	printf("*****send 0\n");

}



void rescale(int* l , int* u , int index_of_sym ,int Cum_count[])
{
	printf("rescale: ");
	printf("Cum_count[%d] = %d   ",index_of_sym, Cum_count[index_of_sym]);
	printf("Cum_count[%d] = %d   ",index_of_sym -1, Cum_count[index_of_sym - 1]);
	int ll = *(l);
	int uu = *(u);
        *(u) = ll + (uu - ll + 1) * (Cum_count[index_of_sym]) / Cum_count[symbol_num] - 1;
	*(l) = ll + (uu - ll + 1) * (Cum_count[index_of_sym - 1]) / Cum_count[symbol_num];
        printf("(%d , %d)--->>>(%d , %d)\n", ll , uu , *(l) , *(u));
	//printf("U = %d -> U = %d\n", uu , *(u)); 
}


//int get_current_symbol(int wait_for_encode)




void main(int argc ,char * args[])
{
   if(argc != 2)
  {
  	printf("%d\n",argc);
  	printf("Please input single file\n");
  }
  
  FILE * fp;
  if((fp = fopen(args[1] , "r"))== NULL)    //open symbol file
  {
  	printf("%s\n",args[1]);
        printf("open file error\n");
  	exit(0);
  }
  
  int source_length = 0;
 // int symbol_num = 0;  
  int symbols_statistic[2][255] = {0};

  char c ;
  while( (c = fgetc(fp)) != EOF && c != '\n')
    {
        source_length ++ ;
        symbols_statistic[0][c] ++;
    }   //printf("%c",c);
  
 // int Count[];

  int Count[256] = {0}; //frequency of symbol

  for(int i = 0 ;i < 255; ++i)
  {
  	if(symbols_statistic[0][i] != 0)
  	{
  		Count[symbol_num]= symbols_statistic[0][i];
  		symbol_num ++;
  		symbols_statistic[1][i] = symbol_num;
  		printf("the number %d  kind symbol %d , its total number is %d\n",symbol_num, i ,symbols_statistic[0][i]);
    }
  }

  fclose(fp); 
  //frequency of symbol
  //int source[] = {1,3,2,1,3,1,3,2,2}; 
  //-------------------------------------------------------
  //initialize 
  //-------------------------------------------------------
  int Cum_count[symbol_num + 1];
  //-----------------------------------------
  //calculate Cun_count[]
  //-----------------------------------------
  Cum_count[0] = 0;
 
  for (int i = 0; i < symbol_num; ++i)
  {
     	Cum_count[i + 1] = Cum_count[i] + Count[i];
     	printf("Cum_count[%d] = %d\n",i + 1,Cum_count[i+1]);
  }
   
  


  int Scale3 = 0;

  //initialize  boundary
  /*
    attention:
    we just use m bits (1 to m ,total 32 for 32-bits OS and 64 bits for 64-bits OS) of l and u which here defined is "unsigned int" 
  */
  unsigned int l = 0;
  //unsigned int u = 0x7FFFFFFF;
  unsigned int u = mMASK;
  printf("L = %d , U = %d\n",l,u); 


  
struct Bit bit;
//test printf = 1;
bit.UMSB = 5/4; 
printf("%d\n",bit.UMSB);

//int wait_for_encode = source_length;  //number of symbols to encode

if((fp = fopen(args[1] , "r"))== NULL)    //open symbol file
  {
  	printf("%s\n",args[1]);
        printf("open file error\n");
  	exit(0);
  }

   char symbol;
   while( (symbol = fgetc(fp)) != EOF && symbol != '\n')
   {
      //encode have not finished
   	  //printf("current_symbol is %c \n",symbol);
   	 while(1)
   	 {
        bit.UMSB = (u >> (m - 1))%2;   
        bit.LMSB = (l >> (m - 1))%2;
            if(bit.UMSB == bit.LMSB)
            {
              bit.sendbit = bit.UMSB; 
                  if(bit.sendbit)
                  {
                    printf("E2 mapping:--------------------------------------");
                  }
                  else
                    printf("E1 mapping:--------------------------------------");  
                    printf("(%d , %d)--->>>",l,u);
              /*---------------------
              if UMSB = LMSB = b
                shift l to left 1 bit and shift 0 into LSB;
                shift u to left 1 bit and shift 1 into LSB;
              ----------------------*/
              l = (l << 1) & mMASK;    //guarantee that l and u always use m bits
              u = ((u << 1) + 1) & mMASK; 
              printf("(%d , %d)\n",l,u);
              send(bit);
              bit.sendbit = !bit.sendbit;   
                  while(Scale3 > 0)       
                  {  
                    printf("Scale3 = %d\n",Scale3);
                    send(bit);
                    Scale3--; 
                    //printf("%d\n",bit.sendbit);
                  }
             }
             else
             {
                bit.USMSB = (u>>(m -2))%2;
                bit.LSMSB = (l>>(m -2))%2;      

                if(bit.USMSB == 0 && bit.LSMSB == 1)  //USMSB = 0 and LSMSB = 1  only if when u = 10... and l = 01...
                {
                  printf("E3 mapping:--------------------------------------");
                  printf("(%d , %d)--->>>",l,u);
                  l = ((l << 1) ^ norMASK) & mMASK;  
                  u = (((u << 1) ^ norMASK) + 1) & mMASK;
                  printf("(%d , %d)\n",l,u);
                  Scale3 ++ ;
                  printf("Scale3 = %d\n",Scale3);
                }
                else
                {
                	//rescale l and u
                	//int index_of_sym = get_current_symbol(wait_for_encode); 
                	printf("@@@@@@current_symbol is %c\n",symbol);
                	int index_of_sym = symbols_statistic[1][symbol]; 
                	rescale(&l,&u,index_of_sym , Cum_count);
                	break;
                	//wait_for_encode -- ;
                }      

             }
        }
    }
     //all symbols have been encoded
  fclose(fp);
  printf("encode finished\n");
}
