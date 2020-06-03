#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>


/*
 * Functions for reading and writing 16- and 32-bit little-endian integers.
 */

static unsigned short read_word(FILE *fp);
static unsigned int   read_dword(FILE *fp);
static int            read_long(FILE *fp);

static int            write_word(FILE *fp, unsigned short w);
static int            write_dword(FILE *fp, unsigned int dw);
static int            write_long(FILE *fp, int l);

/*
 * read the bmp header from file fp and store 
 * the values into the struct pointed to by header
 * 
 * returns ERROR in case of failure, else OK
 */ 
int 
readHeader(FILE *fp, struct bmp_header *header)
{
    int infosize;

    /* Read the file header and any following bitmap information... */
    header->bfType      = read_word(fp);
    header->bfSize      = read_dword(fp);
    header->bfReserved1 = read_word(fp);
    header->bfReserved2 = read_word(fp);
    header->bfOffBits   = read_dword(fp);

    if (header->bfType != BF_TYPE) {
        /* Not a bitmap file - return NULL... */
        return 0;
    }

    infosize = header->bfOffBits - 14;
    if (infosize > 40) {
        return 0;
    }
    return 1;
}

/*
 * read the bmp info header from file fp and store 
 * the values into the struct pointed to by header
 * 
 * returns ERROR in case of failure, else OK
 */ 
int 
readInfo(FILE *fp, struct bmp_info *info)
{
    info->bmiHeader.biSize          = read_dword(fp);
    info->bmiHeader.biWidth         = read_long(fp);
    info->bmiHeader.biHeight        = read_long(fp);
    info->bmiHeader.biPlanes        = read_word(fp);
    info->bmiHeader.biBitCount      = read_word(fp);
    if (info->bmiHeader.biBitCount != 24) {
        return 0;
    }
    info->bmiHeader.biCompression   = read_dword(fp);
    if (info->bmiHeader.biCompression != BI_RGB) {
        return 0;
    }
    info->bmiHeader.biSizeImage     = read_dword(fp);
    info->bmiHeader.biXPelsPerMeter = read_long(fp);
    info->bmiHeader.biYPelsPerMeter = read_long(fp);
    info->bmiHeader.biClrUsed       = read_dword(fp);
    info->bmiHeader.biClrImportant  = read_dword(fp);

    return 1;
}

/*
 * write the bmp info header to file fp using
 * values stored in the struct pointed to by header
 * 
 * returns ERROR in case of failure, else OK
 */ 
int 
writeInfo (FILE *fp, struct bmp_info *info)
{
    write_dword(fp, info->bmiHeader.biSize);
    write_long(fp, info->bmiHeader.biWidth);
    write_long(fp, info->bmiHeader.biHeight);
    write_word(fp, info->bmiHeader.biPlanes);
    write_word(fp, info->bmiHeader.biBitCount);
    write_dword(fp, info->bmiHeader.biCompression);
    write_dword(fp, info->bmiHeader.biSizeImage);
    write_long(fp, info->bmiHeader.biXPelsPerMeter);
    write_long(fp, info->bmiHeader.biYPelsPerMeter);
    write_dword(fp, info->bmiHeader.biClrUsed);
    write_dword(fp, info->bmiHeader.biClrImportant);

    return 1;
}; 

/*
 * write the bmp header to file fp from the values 
 * found in the struct pointed to by header
 * 
 * returns ERROR in case of failure, else OK
 */ 
int 
writeHeader (FILE *fp, struct bmp_info *info)
{
    int infosize, bitsize; 
    int size;
    int length;

    /* Figure out the bitmap size */
    if (info->bmiHeader.biSizeImage == 0) {
        length = (info->bmiHeader.biWidth * 3 + 3) & ~3;
	bitsize = length * info->bmiHeader.biHeight;
    } else
	bitsize = info->bmiHeader.biSizeImage;

    /* Figure out the header size */
    infosize = sizeof(struct bmp_info_header);
    size = sizeof(struct bmp_header) + infosize + bitsize;

    /* Write the file header, bitmap information, and bitmap pixel data... */
    write_word(fp, BF_TYPE);        /* bfType */
    write_dword(fp, size);          /* bfSize */
    write_word(fp, 0);              /* bfReserved1 */
    write_word(fp, 0);              /* bfReserved2 */

    write_dword(fp, 14 + infosize); /* bfOffBits */

    return 1;
}


/*
 * 'read_word()' - Read a 16-bit unsigned integer.
 */

static unsigned short     /* O - 16-bit unsigned integer */
read_word(FILE *fp)       /* I - File to read from */
    {
    unsigned char b0, b1; /* Bytes from file */

    b0 = getc(fp);
    b1 = getc(fp);

    return ((b1 << 8) | b0);
    }


/*
 * 'read_dword()' - Read a 32-bit unsigned integer.
 */

static unsigned int               /* O - 32-bit unsigned integer */
read_dword(FILE *fp)              /* I - File to read from */
    {
    unsigned char b0, b1, b2, b3; /* Bytes from file */

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
    }


/*
 * 'read_long()' - Read a 32-bit signed integer.
 */

static int                        /* O - 32-bit signed integer */
read_long(FILE *fp)               /* I - File to read from */
    {
    unsigned char b0, b1, b2, b3; /* Bytes from file */

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
    }


/*
 * 'write_word()' - Write a 16-bit unsigned integer.
 */

static int                     /* O - 0 on success, -1 on error */
write_word(FILE           *fp, /* I - File to write to */
           unsigned short w)   /* I - Integer to write */
    {
    putc(w, fp);
    return (putc(w >> 8, fp));
    }


/*
 * 'write_dword()' - Write a 32-bit unsigned integer.
 */

static int                    /* O - 0 on success, -1 on error */
write_dword(FILE         *fp, /* I - File to write to */
            unsigned int dw)  /* I - Integer to write */
    {
    putc(dw, fp);
    putc(dw >> 8, fp);
    putc(dw >> 16, fp);
    return (putc(dw >> 24, fp));
    }


/*
 * 'write_long()' - Write a 32-bit signed integer.
 */

static int           /* O - 0 on success, -1 on error */
write_long(FILE *fp, /* I - File to write to */
           int  l)   /* I - Integer to write */
    {
    putc(l, fp);
    putc(l >> 8, fp);
    putc(l >> 16, fp);
    return (putc(l >> 24, fp));
    }

