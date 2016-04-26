#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#define m 21


static unsigned short symbol_num; // variety  number of  symbols
static unsigned char buffer;
static unsigned long int  mMASK;
static unsigned long int  norMASK;
static int m;
int code_length = 0;

//static unsigned char  bits_in_buffer = 0;
FILE * fp , *fpo;

struct Bit
{
  unsigned char UMSB:1;  //MSB of up boundary
  unsigned char USMSB:1; //second MSB of up boundary
  unsigned char LMSB:1;  //MSB of low boundary
  unsigned char LSMSB:1; //second MSB of low boundary
  unsigned char sendbit:1;
  unsigned char bits_in_buffer:3;//number of bits in buffer to be write into output file
}bit;

void final_send(int l,int Scale3)
{
   ///printf("final send\n");
   ///printf("bit.bits_in_buffer = %d , Buffer = %d , L = %d , Scale3 = %d\n",bit.bits_in_buffer,buffer,l,Scale3);
   // buffer = buffer << (8 - bit.bits_in_buffer);
   bit.LMSB = l >> (m - 1);
   int total_bits = Scale3 + m;
   int sent_bits = 0;
   while(sent_bits != total_bits)
   {
       if(1 <= sent_bits && sent_bits <= Scale3)
       {
          bit.sendbit = ! bit.LMSB;
       }
       else
       {
           if(sent_bits <1)
           {
            bit.sendbit = bit.LMSB;
           }
           else
            bit.sendbit = (l >> (m - sent_bits + Scale3 - 1)) & 1;
       }

       buffer = (buffer << 1) + bit.sendbit;
       if(bit.bits_in_buffer == 7)
       {
        sent_bits ++;
        // printf("######write %d into outputfile \n",buffer);
        fwrite(&buffer,1,1,fpo);
        //clear buffer
	code_length ++;
        //buffer = 0;
        bit.bits_in_buffer = 0;
       }
       else
       {
         sent_bits ++;
         bit.bits_in_buffer ++;
       }
   }

   buffer = buffer << (8 - bit.bits_in_buffer);
   ///printf("######write %d into outputfile \n",buffer);
   fwrite(&buffer,1,1,fpo);
   code_length ++;
}


void send()
{

  if(bit.sendbit)
  {
    ///printf("*****send 1\n");
  }
  else
  {
  	///printf("*****send 0\n");
  }

  ///printf("######bits_in_buffer = %d\n",bit.bits_in_buffer);
  buffer = (buffer << 1) + bit.sendbit;

  if(bit.bits_in_buffer == 7)
  {
    ///printf("######write %x into outputfile \n",buffer);
    fwrite(&buffer,1,1,fpo);
    code_length ++;
    //buffer = 0;               //clear buffer
    bit.bits_in_buffer = 0;
  }
  else
  {
    bit.bits_in_buffer ++ ;
  }
}



void rescale(unsigned long int* l , unsigned long int* u , int index_of_sym ,int Cum_count[])
{
	///printf("rescale: ");
	///printf("Cum_count[%d] = %d   ",index_of_sym, Cum_count[index_of_sym]);
	///printf("Cum_count[%d] = %d   ",index_of_sym - 1, Cum_count[index_of_sym - 1]);
	unsigned long int ll = *(l);
	unsigned long int uu = *(u);
        *(u) = ll + ((uu - ll + 1) * (Cum_count[index_of_sym])) / Cum_count[symbol_num] - 1;
	*(l) = ll + ((uu - ll + 1) * (Cum_count[index_of_sym - 1])) / Cum_count[symbol_num];
        ///printf("(%ld , %ld)--->>>(%ld , %ld)\n", ll , uu , *(l) , *(u));
	//printf("U = %d -> U = %d\n", uu , *(u));
}


//int get_current_symbol(int wait_for_encode)




void main(int argc ,char * args[])
{
   if(argc != 3)
  {
  	printf("%d\n",argc);
  	printf("Please input  input-filename and output-filename\n");
  }

  if((fp = fopen(args[1] , "r"))== NULL)    //open symbol file
  {
  	printf("%s\n",args[1]);
    printf("open input file error\n");
  	exit(0);
  }

  if((fpo = fopen(args[2] , "wb"))== NULL)    //open output file
  {
    printf("%s\n",args[2]);
    printf("open output file error\n");
    exit(0);
  }

 printf("Arithmetic Coding start ...\n");
  // if((fs = fopen("statistic.txt" , "w"))== NULL)    //open statistic file
  // {
  //   //printf("%s\n",args[2]);
  //   printf("create statistic-file error\n");
  //   exit(0);
  // }
  //-------------------------------------------------------
  //initialize
  //-------------------------------------------------------
  int source_length = 0;
  int symbols_statistic[2][256] = {0};

  buffer = 0;
  symbol_num = 0;

 /*
  statistics of input file
 */

 unsigned char c ;
 int temp;
 int i = 0;
 while( (temp = fgetc(fp)) != EOF)
    {
    	i ++;
     	c = temp;
        source_length ++;
        symbols_statistic[0][c] ++;
        ///printf("current_symbol is %x , symbols_statistic[0][%u] = %d\n",c,c,symbols_statistic[0][c]);
  
    }   //printf("%c",c);
 
  //printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  i = %d   %d\n", i,symbols_statistic[0][255]);
  int Count[256] = {0}; //frequency of symbol
  unsigned char Symbols[256];

  //fprintf(fs, "%d\n" , symbol_num);
  for(int i = 0 ;i <= 255; ++i)
  {
  	if(symbols_statistic[0][i] != 0)
  	{
  		Count[symbol_num] = symbols_statistic[0][i];
     	Symbols[symbol_num] = i;
  		symbol_num ++;
  		//printf("%d\n",symbol_num);
  		symbols_statistic[1][i] = symbol_num;
  		///printf("number %d kind symbol is %8x , its total number is %d \n" , symbol_num , i , symbols_statistic[0][i]);
    }
  }

  fwrite(&symbol_num,sizeof(symbol_num),1,fpo);

  code_length = code_length + sizeof(symbol_num);

  for (int i = 0; i < symbol_num; i++)
  {
    ///fprintf(fs, "%d %d\n", Symbols[i] , Count[i]);
    fwrite(&Symbols[i],sizeof(Symbols[i]),1,fpo);
    fwrite(&Count[i],sizeof(Count[i]),1,fpo);
    code_length = code_length + sizeof(Symbols[i]) + sizeof(Count[i]);
  }

  // write statistic information to statistic.txt


  //fclose(fp);
  //frequency of symbol
  //int source[] = {1,3,2,1,3,1,3,2,2};

  int Cum_count[symbol_num + 1];
  //-----------------------------------------
  //calculate Cun_count[]
  //-----------------------------------------
  Cum_count[0] = 0;

  for (int i = 0; i < symbol_num; ++i)
  {
     	Cum_count[i + 1] = Cum_count[i] + Count[i];
     	///printf("Cum_count[%d] = %d\n",i + 1,Cum_count[i+1]);
  }

  //fclose(fs);


  m = (int)(log(Cum_count[symbol_num])/log(2)) + 1 + 2; 
  mMASK =  (1 << m) - 1;
  printf("m = %d\n",m);
  norMASK=  1 << (m-1);
  int Scale3 = 0;

  //initialize  boundary
  /*
    attention:
    we just use m bits (1 to m ,total 32 for 32-bits OS and 64 bits for 64-bits OS) of l and u which here defined is "unsigned int"
  */
  unsigned long int l = 0;
  //unsigned int u = 0x7FFFFFFF;
  unsigned long int u = mMASK;
  ///printf("L = %ld , U = %ld\n",l,u);

//set fp back to the head of symbol file
rewind(fp);

unsigned char symbol;
int current_num = 0;
   while((temp = fgetc(fp)) != EOF)
   {
     symbol = temp;
     current_num ++;
      //encode have not finished
   	/// printf("current_symbol is %x \n",symbol);
   	 while(1)
   	 {
        bit.UMSB = (u >> (m - 1))%2;
        bit.LMSB = (l >> (m - 1))%2;
            if(bit.UMSB == bit.LMSB)
            {
              bit.sendbit = bit.UMSB;
                ///  if(bit.sendbit)
                 /// {
                  ///  printf("E2 mapping:--------------------------------------");
                 /// }
                 /// else
                  ///  printf("E1 mapping:--------------------------------------");
                  ///  printf("(%ld , %ld)--->>>",l,u);
              /*---------------------
              if UMSB = LMSB = b
                shift l to left 1 bit and shift 0 into LSB;
                shift u to left 1 bit and shift 1 into LSB;
              ----------------------*/
              l = (l << 1) & mMASK;    //guarantee that l and u always use m bits
              u = ((u << 1) + 1) & mMASK;
             /// printf("(%ld , %ld)\n",l,u);
              send();
              bit.sendbit = !bit.sendbit;
                  while(Scale3 > 0)
                  {
                   /// printf("Scale3 = %d\n",Scale3);
                    send();
                    Scale3--;
                   /// printf("%d\n",bit.sendbit);
                  }
             }
             else
             {
                bit.USMSB = (u>>(m -2))%2;
                bit.LSMSB = (l>>(m -2))%2;

                if(bit.USMSB == 0 && bit.LSMSB == 1)  //USMSB = 0 and LSMSB = 1  only if when u = 10... and l = 01...
                {
                  ///printf("E3 mapping:--------------------------------------");
                 /// printf("(%ld , %ld)--->>>",l,u);
                  l = ((l << 1) ^ norMASK) & mMASK;
                  u = (((u << 1) ^ norMASK) + 1) & mMASK;
                  ///printf("(%ld , %ld)\n",l,u);
                  Scale3 ++ ;
                  ///printf("Scale3 = %d\n",Scale3);
                }
                else
                {
                	//rescale l and u
                	//int index_of_sym = get_current_symbol(wait_for_encode);
                	///printf("@@@@@@the number %d is (ASCII = %x)\n",current_num,symbol);
                	int index_of_sym = symbols_statistic[1][symbol];
                	rescale(&l,&u,index_of_sym,Cum_count);
                	break;
                	//wait_for_encode -- ;
                }

             }
        }
    }
  final_send(l,Scale3);
  //all symbols have been encoded
  fclose(fp);
  fclose(fpo);
  printf("encode finished\n");
  printf("Arithmetic code stream length is %d bytes\n",code_length);
}
