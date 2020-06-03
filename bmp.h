
#ifndef _BITMAP_H_
#  define _BITMAP_H_

#include <stdio.h>


#define OK     1
#define ERROR  0 


/*
 * Bitmap file data structures (these are defined in <wingdi.h> under
 * Windows...)
 *
 * Note that most Windows compilers will pack the following structures, so
 * when reading them under MacOS or UNIX we need to read individual fields
 * to avoid differences in alignment...
 */
#define BF_TYPE 0x4D42             /* "MB" */

#pragma pack(2)

/**** BMP file header structure ****/ 
struct bmp_header        
{
    unsigned short bfType;           /* Magic number for file */
    unsigned int   bfSize;           /* Size of file */
    unsigned short bfReserved1;      /* Reserved */
    unsigned short bfReserved2;      /* ... */
    unsigned int   bfOffBits;        /* Offset to bitmap data */
}; 


/**** BMP file info structure ****/
struct bmp_info_header  {
    unsigned int   biSize;           /* Size of info header */
    int            biWidth;          /* Width of image */
    int            biHeight;         /* Height of image */
    unsigned short biPlanes;         /* Number of color planes */
    unsigned short biBitCount;       /* Number of bits per pixel */
    unsigned int   biCompression;    /* Type of compression to use */
    unsigned int   biSizeImage;      /* Size of image data */
    int            biXPelsPerMeter;  /* X pixels per meter */
    int            biYPelsPerMeter;  /* Y pixels per meter */
    unsigned int   biClrUsed;        /* Number of colors used */
    unsigned int   biClrImportant;   /* Number of important colors */
}; 

/*
 * Constants for the biCompression field...
 */

#  define BI_RGB       0             /* No compression - straight BGR data */
#  define BI_RLE8      1             /* 8-bit run-length compression */
#  define BI_RLE4      2             /* 4-bit run-length compression */
#  define BI_BITFIELDS 3             /* RGB bitmap with RGB masks */


/**** Colormap entry structure ****/
struct rgb  {
    unsigned char  rgbBlue;          /* Blue value */
    unsigned char  rgbGreen;         /* Green value */
    unsigned char  rgbRed;           /* Red value */
    unsigned char  rgbReserved;      /* Reserved */
}; 

/**** Bitmap information structure ****/
struct bmp_info {
    struct bmp_info_header bmiHeader;      /* Image header */
    struct rgb             bmiColors[256]; /* Image colormap */
};

/*
 * Prototypes...
 */
/*
 * read bmp and info headers from file 
 */
int readHeader(FILE *fp, struct bmp_header *header); 
int readInfo(FILE *fp, struct bmp_info *info); 

/*
 * write bmp and info headers from file 
 */
int writeHeader (FILE *fp, struct bmp_info *info); 
int writeInfo (FILE *fp, struct bmp_info *info); 
#endif /* !_BITMAP_H_ */


