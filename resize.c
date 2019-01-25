/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./copy infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];
    int n = atoi(argv[1]);
    if(n<1||n>100)
    {
        return 5;
    }
    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
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
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    long w =  bi.biWidth;
    long h =  abs(bi.biHeight);
    int opadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    bi.biWidth*=n;
    bi.biHeight*=n;
    
    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((sizeof(RGBTRIPLE)*bi.biWidth)+ padding)*abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    // iterate over infile's scanlines
        RGBTRIPLE *arr = (RGBTRIPLE *)malloc((w) * (h) * sizeof(RGBTRIPLE)),*x = NULL;
     x = arr ;
    for(int a=0;a<(h);a++)
    {
        for(int b=0;b<(w);b++)
        {
            fread(arr, sizeof(RGBTRIPLE), 1, inptr);
            arr++;
        }
        fseek(inptr, opadding, SEEK_CUR);
    }
    arr = x;
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight;)
    {   for(int b=0;b<n;b++)
      {  if(b!=0)
        {
            arr-= w;
        }
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; )
        {
            // temporary storage
            for(int a=0;a<n;a++)
            {// write RGB triple to outfile
            fwrite(arr, sizeof(RGBTRIPLE), 1, outptr);
            j++;
            }    
            arr++;
        }

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
          i++;
      } 
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
