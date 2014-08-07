#include <stdio.h>
#include <ctype.h>

unsigned char GetHex(FILE *infile)
{ int ch;
  ch=fgetc(infile);

  return (unsigned char)((ch<='9')?ch-'0':ch-'A'+10);
 }

unsigned char GetHex2(FILE *infile)
{ return (unsigned char)((GetHex(infile)<<4)|(GetHex(infile)&0x0f));
 }

void GetDummy(FILE *infile, int count)
{ for(;count;count--)
    fgetc(infile);
 }

int main(int argc, char *argv[])
{ FILE *infile, *outfile;
  unsigned int addr, count, type;
  unsigned int start_addr=0x10000, end_addr=0;
  unsigned char rom[0x10000];

  if ((argc<3) ||
      (! ((infile=fopen(argv[1],"r")) &&
          (outfile=fopen(argv[2],"wb")) )
      ))
  { printf("Usage: %s <infile> <outfile>\n",argv[0]);
    return(1);
   }

  for(addr=0;addr<sizeof(rom);addr++)
    rom[addr]=0xff;

  while(!feof(infile))
  { if (fgetc(infile)!=':')
      break;

    count=GetHex2(infile);

    addr=(GetHex2(infile)<<8)|GetHex2(infile);
    type=GetHex2(infile);

    switch(type)
    { case 0:
    
        if(addr<start_addr)
          start_addr=addr;

        if(addr+count>end_addr)
          end_addr=addr+count-1;

        for(;count;count--)
        { rom[addr++]=GetHex2(infile);
         }
        GetDummy(infile,3);
        break;

      case 1:

      
        fwrite(&rom[start_addr],end_addr-start_addr+1,1,outfile);
        fclose(infile);
        fclose(outfile);
        return(0);
     }
   }
   fclose(infile);
   fclose(outfile);
   return(1);
 }
