/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bmp.h"
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

char* itoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;
 
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    int h=strlen(str)-1;
    for(int k=0;k<strlen(str)/2;k++)
    {
        char temp = str[k];
        str[k]=str[h];
        str[h--]=temp;
    }
 
    return str;
}
void Makeme(int *bits)
{
    
    char c[9];
    sprintf(c,"%d%d%d%d%d%d%d%d",bits[7],bits[6],bits[5],bits[4],bits[3],bits[2],bits[1],bits[0]);
    int d = atoi(c);
    char ch = ConvertBinaryToDecimal(d);
    if(ch=='0')
    {
        printf("\n");
        exit(0);
    }
    else
        printf("%c",ch);

}
int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: ./copy infile outfile\n");
        return 1;
    }

    // remember filenames
    char* infile = argv[1];


    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
   

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
  /*  if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
       
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }  
*/
    int bits[8];
    int w =0;
   
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            
            // temporary storage
            RGBTRIPLE first;
       
            // read RGB triple from infile
            fread(&first, sizeof(RGBTRIPLE), 1, inptr);
            if(first.rgbtRed!=255&&first.rgbtBlue!=255&&first.rgbtGreen!=255&&first.rgbtRed!=0&&first.rgbtBlue!=0&&first.rgbtGreen!=0)
            {    bits[w++] = first.rgbtRed&1;
                if(w==8)
                {   
                    w=0;
                   Makeme(bits);
                }
                bits[w++] = first.rgbtBlue&1;
                if(w==8)
                {   
                    w=0;
                   Makeme(bits);
                }
                bits[w++] = first.rgbtGreen&1;
                if(w==8)
                {   
                    w=0;
                   Makeme(bits);
                }
            }
        }
            
    }
        printf("\n");
    // close infile
    fclose(inptr);

    // that's all folks
    return 0;
}
