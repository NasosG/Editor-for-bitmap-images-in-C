#include <strings.h>
#include <string.h>
#include <ctype.h>
#include "functions.c"

void attrs(char* filename);
void help();

int main(int argc, char** argv)
{
	int i, j, k;
	FILE *fp1, *fp2;
	char *filename, *filename2;
	int scalefactor;
	double percent=0;
	int flip=0;
	int scale=0;
	int contrast=0;
	int contauto=0;
	int flipm2=0;
	int padding;

	if (argc<3 || argc>6) 
	{
		puts("INVALID ARGUMENTS");
		exit(0);
	}

	if(!strcmp(argv[1],"-attrs"))
	{
		if((filename=(char*)malloc(sizeof(argv[2])*100)) == NULL){
				puts("NO SPACE AVAILABLE");
				exit(1);
		}
		strcpy(filename,argv[2]);
		if(!strstr(filename,".bmp")) 
				strcat(filename,".bmp");
		attrs(filename);
		
	}

	/* edo mporousame na desmeusoume kai kalutera ton xoro ton onomaton analoga me to poia leitourgia
	   dinei o xrhsths me to katallhlo argv[]
	*/
	if((filename = (char*)malloc(sizeof(argv[1])*100)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
	if((filename2 = (char*)malloc(sizeof(argv[2])*100)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
	
	if (!strcmp(argv[1],"-scale"))
	{
		strcpy(filename,argv[4]); 
		strcpy(filename2,argv[5]);
		if (!strcmp(argv[2],"up")){
			scale=1;
			scalefactor = atoi(argv[3]);
		}
		else if (!strcmp(argv[2],"down")){
			scale=2;
			scalefactor = atoi(argv[3]);
		}
		else
		{
			puts("unknown command");
			return 0;
		}
	}
	else if (!strcmp(argv[1],"-flip"))
	{
		strcpy(filename,argv[3]); 
		strcpy(filename2,argv[4]);
		if (!strcmp(argv[2],"v")){
			flip=1;
		}
		else if (!strcmp(argv[2],"h")){
			flip=2;
		}
		else
		{
			puts("unknown command");
			return 0;
		}
	}
	else if(!strcmp(argv[1],"-contrast"))
	{
		strcpy(filename,argv[3]); 
		strcpy(filename2,argv[4]);
		if(!strcmp(argv[2],"auto"))
			contauto=1;
		else
		{
			contrast=1;
			percent = atoi(argv[2]);
			
			if ( percent<-100 || percent>100 )
			{
				puts("perccentage number should be between -100 and +100");
				return 0;
			}
			percent*=0.01;
		}
	}
	else if (!strcmp(argv[1],"-flipm2"))
	{
		strcpy(filename,argv[3]); 
		strcpy(filename2,argv[4]);
		if (!strcmp(argv[2],"v")){
			flipm2=1;
		}
		else if (!strcmp(argv[2],"h")){
			flipm2=2;
		}
		else
		{
			puts("unknown command");
			return 0;
		}
	}
	else if(!strcmp(argv[1],"-need") && !strcmp(argv[2],"help"))
	{
		help(); //print available commands
		free(filename);
		free(filename2);
		exit(0);
	}
	else
	{
		strcpy(filename,argv[1]); 
		strcpy(filename2,argv[2]);
	}
	
	if(!strstr(filename,".bmp"))  //add .bmp if it's not included
		strcat(filename,".bmp");
	if(!strstr(filename2,".bmp"))  
		strcat(filename2,".bmp");
	
	
	if(contrast)
	{	
		if( percent == 0 ) //zero percent change means just create the same image
			copyf(filename, filename2); //create a copy
		else contrastf(filename, filename2, percent);		
	}
	else if(contauto)
	{
		contautof(filename, filename2);	
	}
	else if(scale)
	{
		scalef(filename, filename2,  scale,  scalefactor);
	}
	else if(flip)
	{
		flipf(filename, filename2, flip);			
	}
	else if(flipm2)
	{
		flipm2f(filename, filename2, flipm2);		
	}
	else //just make a copy of the image with the easiest way possible
	{
		copyf(filename, filename2);		
	}
					
return 0;
}


void attrs(char* filename)
{
	FILE *fp1;
	bmpInfo *inf1;
	bmpHeader *h1;

	fp1 = fopen(filename,"rb");
	if (fp1 == NULL){
		puts("file doesn't exist");
		exit(1);
	}
	
	
	if((h1 = (bmpHeader*)malloc(sizeof(bmpHeader)*1)) == NULL){
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
	if(readHeader( fp1, h1 ) == ERROR){
		printf("header error\n");
		fclose(fp1);
		exit(1);
	}
	
	if((inf1 = (bmpInfo*)malloc(sizeof(bmpInfo)*1)) == NULL){
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
	if(readInfo( fp1, inf1 ) == ERROR){
		printf("header INFO error \n");
		fclose(fp1);
		exit(1);
	}
	fclose(fp1);

	printf("bfType: %d\nbfSize: %d\nbfReserved1: %d\nbfReserved2 %d\nbfOffBits %d\nbiSize: %d\nbiWidth: %d\nbiHeight: %d\nbiPlanes: %d\nbiBitCount: %d\nbiCompression: %d\nbiSizeImage: %d\nbiXPelsPerMeter: %d\nbiYPelsPerMeter: %d\nbiClrUsed: %d\nbiClrImportant: %d\n",
h1->bfType,
h1->bfSize,
h1->bfReserved1,
h1->bfReserved2,
h1->bfOffBits,
inf1->bmiHeader.biSize,
inf1->bmiHeader.biWidth,
inf1->bmiHeader.biHeight,
inf1->bmiHeader.biPlanes,
inf1->bmiHeader.biBitCount,
inf1->bmiHeader.biCompression,
inf1->bmiHeader.biSizeImage,
inf1->bmiHeader.biXPelsPerMeter,
inf1->bmiHeader.biYPelsPerMeter,
inf1->bmiHeader.biClrUsed,
inf1->bmiHeader.biClrImportant);

free(inf1);
free(h1);
free(filename);

exit(0);
}

void help()
{
		puts("\n**Available Commands**\n\n");
		puts("(1) ./blind [image source name] [image output name] -> to copy the image\n");
		puts("(2) ./blind -attrs [image name] -> to see image's info and attributes about width,height,size etc.\n");
		puts("(3) ./blind -scale <up/down> [scalefactor] [image source name] [image output name] -> to scale up/down by a factor\n");
		puts("(4) ./blind -flip <v/h> [image source name] [image output name] -> to mirror it vertically or horizontally\n");
		puts("(5) ./blind -contrast <number> [image source name] [image output name] -> to change image contrast\n");
		puts("(6) ./blind -contrast <auto> [image source name] [image output name] -> to automatically change image contrast\n");
		puts("(7) ./blind -flipm2 <v/h> [image source name] [image output name] -> to flip it vertically or horizontally\n");
		puts("(8) ./blind -need help -> to see this info");
}

