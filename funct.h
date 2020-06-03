typedef  struct bmp_header bmpHeader;
typedef  struct bmp_info bmpInfo;

int contrastf(char* filename, char* filename2, double percent);
int contautof(char* filename, char* filename2);
int scalef(char* filename, char* filename2, int scale, int scalefactor);
int flipf(char* filename, char* filename2, int flip);
int flipm2f(char* filename, char* filename2, int flipm2);
int copyf(char* filename, char* filename2);

typedef struct
{
    unsigned char  rgbBlue;
    unsigned char  rgbGreen;
    unsigned char  rgbRed;
}pixel;

