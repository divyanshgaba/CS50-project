

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "bmp.h"
#include <stdbool.h>
#include <math.h>
int ConvertBinaryToDecimal(long long n)
{
    int decimalNumber = 0, i = 0, remainder;
    while (n!=0)
    {
        remainder = n%10;
        n /= 10;
        decimalNumber += remainder*pow(2,i);
        ++i;
    }
    return decimalNumber;
}
bool Makeme(int *bits,FILE *outptr)
{
    
    char c[9];
    sprintf(c,"%d%d%d%d%d%d%d",bits[6],bits[5],bits[4],bits[3],bits[2],bits[1],bits[0]);
    int d = atoi(c);
    char ch = ConvertBinaryToDecimal(d);
    if(ch=='0')
    {
        fclose(outptr);
        return false;
    }
    fputc(ch,outptr);
    return true;

}
bool decode()
{
    char infile[50];
    printf("Infile: ");
    scanf("%s",infile);
    // open input file
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return false;
    }// read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
       
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return false;
    }  
    char outfile[50];
    printf("Outfile: ");
    scanf("%s",outfile);
      // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return false;
    }
    int bits[7];
    int w =0;
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;
       
            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
                bits[w++] = triple.rgbtRed&1;
                if(w==7)
                {   
                    w=0;
                   if(!Makeme(bits,outptr))
                   {
                       return true;
                   }
                }
                bits[w++] = triple.rgbtBlue&1;
                if(w==7)
                {   
                    w=0;
                    if(!Makeme(bits,outptr))
                    {
                        return true;       
                    }
                    
                }
                bits[w++] = triple.rgbtGreen&1;
                if(w==7)
                {   
                    w=0;
                   if(!Makeme(bits,outptr))
                   {
                       return true;
                   }
                }
            }
    }
        printf("\n");
    // close infile
    fclose(inptr);
    fclose(outptr);
    return true;
}
bool encode()
{
    char infile[50];
    char outfile[50];
    printf("Infile :");
    scanf("%s",infile);
    // open input file
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return false;
    }
    
   
    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
            bi.biBitCount != 24 || bi.biCompression != 0)
    {
        
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return false;
    }
    printf("Outfile: ");
    scanf("%s",outfile);
    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return false;
    }
    printf("File with text to encode: ");
    char filename[50];
    scanf("%s",filename);
    FILE *read=fopen(filename,"r");
    if(read==NULL)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", filename);
        exit(1);
    }
    //some variables. duh
    char info[1500000]="";
    int ghgh=0;
    
    while(!feof(read))
    {
        info[ghgh++]=fgetc(read);
    }
    //our way to know when to stop reading
    //3 - ETX - End of Text
    info[ghgh++]='0';
    info[ghgh]='\0';
    fclose(read);
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    int len = strlen(info);
    int bits[1500000];
    int w=0;     // get the bits out of them. Words
    for(int y =0;y<len;y++)
    {
        unsigned char ch=info[y];
        for(int z =0;z<7;z++)
        {
            bits[w++]=ch%2;
            ch=ch>>1;
            
        }
       
    }
    // How many were read
    printf("%d %d %d\n",len,w,ghgh);
    // To check later if everything got copied
    len=w;
    w=0;
    
    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
           
            RGBTRIPLE triple;
                    // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
          
               
            if(w<len)
            {
                        
                        triple.rgbtRed = triple.rgbtRed%2==0?triple.rgbtRed:triple.rgbtRed-1;
                        triple.rgbtRed = triple.rgbtRed+bits[w++];
                    if(w<len)
                    {
                        triple.rgbtBlue = triple.rgbtBlue%2==0?triple.rgbtBlue:triple.rgbtBlue-1;
                        triple.rgbtBlue = triple.rgbtBlue+bits[w++];
                    }
                    if(w<len)
                    {
                        triple.rgbtGreen = triple.rgbtGreen%2==0?triple.rgbtGreen:triple.rgbtGreen-1;
                        triple.rgbtGreen = triple.rgbtGreen+bits[w++];
                    }
            }
         
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);
        
        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);
    if(w<len)
    {
        printf("Said enuff. Actually too much :( image is too small for that much data\n");
    }
    // that's all folks
    return true;
}
int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 1)
    {
        printf("Usage: ./stegano \n");
        return 1;
    }
    int choice;
    do{
        
        printf("\n1. Hide\n");
        printf("2. Read\n");
        printf("3. Exit\n");
        printf("Choice: ");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
                if(encode())
                {
                    printf("All done boys");
                }
                else
                {
                    printf("Didn't work. Maybe Try again?");
                }
                break;
            case 2:
                if(decode())
                {
                    printf("Phew All done. Wasn't Bad eh?");
                }
                else
                {
                    printf("That's some junk right there");    
                }
                break;
            case 3:
                printf("kthxbye\n");
                exit(0);
            default:
                printf("Invalid Choice. Try again");
                break;
        }
        getchar();
    }while(choice!=3);
    return 0;
}
