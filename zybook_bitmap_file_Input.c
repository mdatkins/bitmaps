#include <stdio.h>

// Lab exercise to read a bitmap from a binary FILE, parse and print a bitmap header  and some pixel data
// 1/22/2018      md atkins

//code adapted from http://stackoverflow.com/questions/1047723/reading-bytes-from-bmp-file (original source by unwind)

// See https://en.wikipedia.org/wiki/BMP_file_format  for description of the bitmap file header:
// bytes 0 thru 13 = bitmap file header
// bytes 14 thru (varies) = bitmap info header

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned char BYTE;

/* structures for defining DIBs from WinGDI.h  */
typedef struct {   	//bitmap info header
   DWORD      biSize;   //#bytes in DIBheader
   DWORD      biWidth;  //image width in pixels
   DWORD       biHeight; //image heightin pixels
   WORD       biPlanes;
   WORD       biBitCount; //# bits per pixel
   DWORD      biCompression;
   DWORD      biSizeImage;
   DWORD       biXPelsPerMeter;
   DWORD       biYPelsPerMeter;
   DWORD      biClrUsed;
   DWORD      biClrImportant;
} biHeader, *PBITMAPINFOHEADER, *pbh;

typedef struct tagRGBTRIPLE {  //from WinGDI.h
   BYTE    b; //rgbtBlue;
   BYTE    g; //rgbtGreen;
   BYTE    r; //rgbtRed;
} pix, *ppix;

const pix RED = { 0,0,255 };
const pix GREEN = { 0,255,0 };
const pix BLUE = { 255,0,0 };

//pixelstart: return a pointer to the start of the bitmap pixels
// @p : pointer to start of bitmap file
pix* pixelstart(char* p)
{
   pix* start = NULL;
   unsigned int offset_to_pix = *(p + 10); //File header bytes 10-13 are the offset to pixel start
   start = (pix*)(p + offset_to_pix);
   return start;
}

void bitmapinfo(biHeader* p) //parse DIBheader to determine image size & pix depth
                             // p points to start of bitmap info header    
{
   printf("BMP width(pix)= %u \n", p->biWidth);
   printf("BMP height(pix)= %u \n", p->biHeight);
   printf("Bits per pix= %u \n", p->biBitCount);
   return;
}

void printpix(pix* p, int count) //print hex value of pixels starting @ p
{
   printf("#  R  G  B\n"); //header
   for (int i = 0; i < count; i++)
   {
      printf("%2d %02x %02x %02x \n", i, (i + p)->r, (i + p)->g, (i + p)->b);
   }
}

int file_size(FILE *in)
//return file size of any file
{
   if (fseek(in, 0, SEEK_END) == 0)  //go to file end.  fseek() returns 0 if successful
   {
      long len = ftell(in);  //ftell() gives current location in file
      if (len > 0)
      {
         if (fseek(in, 0, SEEK_SET) == 0)  //put pointer back at start of file
         {
            return (int)len;
         }
      }
   }
   return 0;
}

char* load_binary(char *filename, int *size)
{
   FILE *in = NULL;
   char *data = NULL;  //pointer to memory block holding input file after fread()
   int len;  // holds size of input file

   in = fopen(filename, "rb");
   if (in == NULL)
      printf("\n Error opening file %s \n", filename);
   else //open succeeded
   {
      if (len = file_size(in)) // if not empty file
         if ((data = (char *)malloc(len)) != NULL)  //if we can malloc enough RAM
            if (fread(data, 1, len, in) == len)  //copy file into RAM
               *size = len;
            else
            {
               free(data);
               data = NULL;
            }
      fclose(in);
   }
   return data;
}


int main()
{
   int i = 0, count = 8;

   char fname[40] = "";//file and possibly path names
   char *image; //pointer to image in memory

   int size; //size of input file
   int bitmaplen = 0; //bytes of actual pixels, sans header

   printf("Enter input bitmap filename (full path if not in current directory): ");
   scanf("%s", fname);

   printf("Enter number of pixels to print: ");
   scanf("%d", &count);

   image = load_binary(fname, &size);
   if (image != NULL)
   {
      printf("\nBMP size = %u bytes\n", (unsigned int)size);
      biHeader* p = (biHeader*)(image + 14); //biheader starts at byte 14
      bitmapinfo(p);

      pix* ps = pixelstart(image);
      //     printf("Pixels start at address = %8x \n", ps);

      //print pixels
      printf("\n");
      printpix(ps, count);
   }
   free(image);  // free memory (pointer) at the very end of your program
   return 0;
}
