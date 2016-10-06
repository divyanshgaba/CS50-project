#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "bmp.h"
#include <stdbool.h>
#include <math.h>
bool endian();
int ConvertBinaryToDecimal(long long n);
bool Makeme(int *bits,FILE *outptr);
bool decode();
bool encode();

bool endian()
{
   unsigned int i = 1;
   char *c = (char*)&i;
   if (*c)    
       return true;
   return false;
}
int sizefactor(int txtSize,int imgSize)
{
    int n=1;
    while(txtSize*7 >=imgSize)
    {
        imgSize*=2;
        n++;
    }
    return n;
}
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
    if(endian())
        sprintf(c,"%d%d%d%d%d%d%d",bits[6],bits[5],bits[4],bits[3],bits[2],bits[1],bits[0]);
    else
        sprintf(c,"%d%d%d%d%d%d%d",bits[0],bits[1],bits[2],bits[3],bits[4],bits[5],bits[6]);
    int d = atoi(c);
    char ch = ConvertBinaryToDecimal(d);
    if(ch=='~')
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
     int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int bits[8];
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
            
                bits[w++] = triple.rgbtRed%2;
                if(w==7)
                {   
                    w=0;
                   if(!Makeme(bits,outptr))
                   {
                       return true;
                   }
                }
                bits[w++] = triple.rgbtBlue%2;
                if(w==7)
                {   
                    w=0;
                    if(!Makeme(bits,outptr))
                    {
                        return true;       
                    }
                    
                }
                bits[w++] = triple.rgbtGreen%2;
                if(w==7)
                {   
                    w=0;
                   if(!Makeme(bits,outptr))
                   {
                       return true;
                   }
                }
            }
              fseek(inptr, padding, SEEK_CUR);
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
    
    
    //Re-sizing image to store all the data
    fseek(read, 0L, SEEK_END);
    int txtSize = ftell(read);
    rewind(read); 
    int ns=sizefactor(txtSize,bf.bfSize);
     // determine padding for scanlines
    int inipadding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    //storing width of infile
    int oldWidth = bi.biWidth; 
    //storing width of outfile
    int oldHeight = abs(bi.biHeight);
    //updating dimensions for outfile
    bi.biWidth*= ns;
    bi.biHeight*= ns;
    //Padding for outfile
    int finpadding = (4- (bi.biWidth * sizeof(RGBTRIPLE)) % 4 ) %4;
    //updating size of outfile image (Image size is sum of all RGB triplets)
    bi.biSizeImage = abs(bi.biHeight) * (bi.biWidth * sizeof(RGBTRIPLE) + finpadding);
    //updating size of outfile, ( file size is sum of all RGB triplets  and File headers)   
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    //variable to store all of that string
    char *info=(char*)calloc(txtSize,sizeof(char));
    int ghgh=0;
    while(!feof(read))
    {
        info[ghgh++]=fgetc(read);
    }
    //our way to know when to stop reading
    info[ghgh++]='~';
    info[ghgh]='\0';
    fclose(read);
    int len = strlen(info);
    int *bits=(int*)calloc(txtSize*7,sizeof(int));
    int w=0;     // get the bits out of them. Words
    if(endian())
    {   for(int y =0;y<len;y++)
        {
            unsigned char ch=info[y];
            for(int z =0;z<7;z++)
            {
                bits[w++]=ch%2;
                ch=ch>>1;
                
            }
           
        }
    }
    else
    {
        for(int y =0;y<len;y++)
        {
            unsigned char ch=info[y];
            for(int z =0;z<7;z++)
            {
                bits[w++]=ch%2;
                ch=ch<<1;
                
            }
           
        }
    }
    // How many were read
    printf("%d %d %d %d\n",len,w,ghgh,ns);
    // To check later if everything got copied
    len=w;
    w=0;
    int count =0;
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    // iterate over infile's scanlines
    for (int i = 0; i < oldHeight; i++)
    {
        for(int x = 0; x < ns; x++)
        {
            fseek(inptr, ((sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)) +(oldWidth * 3 + inipadding) * i ), SEEK_SET);
            // iterate over pixels in scanline
            for (int j = 0; j < oldWidth; j++)
            {
               
                RGBTRIPLE triple;
                        // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                
                for(int k=0;k<ns;k++)
                { 
                    
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
                    count=count%150 + 1;
                }
            }
    
            // adding padding over to the new file, if needed
            for (int k = 0; k < finpadding; k++)
            {
            fputc(0x00, outptr);
            }
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
int main(void)
{

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
