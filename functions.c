#include "bmp.h"
#include "bmp.c"
#include "funct.h" //header file in which functions prototypes and typedefs are included
	
int contrastf(char* filename, char* filename2, double percent)
{	
	FILE *fp1, *fp2;
	int padding;
	int i, j, k;
	bmpInfo *inf1;
	bmpHeader *h1;
	
	fp1 = fopen(filename,"rb");
	if (fp1 == NULL){
		puts("file doesn't exist");
		exit(1);
	}
						
	if((h1 = (bmpHeader*)malloc(sizeof(bmpHeader)*1)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
					
	if((inf1 = (bmpInfo*)malloc(sizeof(bmpInfo)*1)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
									
	if(readHeader( fp1, h1 ) == ERROR){
		printf("header error\n");
		fclose(fp1);
		exit(1);
	}

	if(readInfo( fp1, inf1 ) == ERROR){
		printf("header INFO error \n");
		fclose(fp1);
		exit(1);
	}	
					
	int meshtimh=0;
	fp2 = fopen(filename2,"wb");
	if (fp2 == NULL){
		puts("file doesn't exist");
		exit(1);
	}
	
	// determine the padding and the extra zero bytes that need to be put in order for the 
	// image width to be divisible by 4 
	padding =  (4 - (inf1->bmiHeader.biWidth * sizeof(pixel)) % 4) % 4;
						
	//our rgb array dynamically allocated
	pixel **matrix;
	if ((matrix = (pixel**)malloc( inf1->bmiHeader.biWidth *sizeof(pixel*))) == NULL)
	{
		printf("No memory available, allocation failed!\nThe program is about to exit...\n");
		exit(0);
	}
	for (i=0; i< inf1->bmiHeader.biWidth; i++)
		if ((matrix[i] = (pixel*)malloc( inf1->bmiHeader.biHeight*sizeof(pixel))) == NULL)
		{
				printf("No memory available, allocation failed!\nThe program is about to exit...\n");
				exit(0);
		}
					
	for (i=0; i < inf1->bmiHeader.biHeight; i++)
	{
		for ( j = 0; j < inf1->bmiHeader.biWidth; j++)
		{
			fread(&matrix[j][i], sizeof(pixel), 1, fp1);
		}
		// skip padding
		fseek(fp1, padding, SEEK_CUR);
	}

	writeHeader (fp2, inf1);
	writeInfo(fp2, inf1);
	
	for (i=0; i < inf1->bmiHeader.biHeight; i++)
	{
		for (j=0; j < inf1->bmiHeader.biWidth; j++)
		{
			meshtimh = (matrix[j][i].rgbBlue + matrix[j][i].rgbRed + matrix[j][i].rgbGreen)/3;
			if(percent > 0)// + contrast
			{
				matrix[j][i].rgbBlue = meshtimh+((matrix[j][i].rgbBlue-meshtimh)+(percent*(matrix[j][i].rgbBlue-meshtimh)));
				matrix[j][i].rgbGreen = meshtimh+((matrix[j][i].rgbGreen-meshtimh)+(percent*(matrix[j][i].rgbGreen-meshtimh)));
				matrix[j][i].rgbRed = meshtimh+((matrix[j][i].rgbRed-meshtimh)+(percent*(matrix[j][i].rgbRed-meshtimh)));
				fwrite(&matrix[j][i], sizeof(pixel), 1, fp2); 
			}
			else if (percent < 0)// - contrast
			{
				//do the same as above because percent will be negative
				matrix[j][i].rgbBlue = meshtimh+((matrix[j][i].rgbBlue-meshtimh)+(percent*(matrix[j][i].rgbBlue-meshtimh)));
				matrix[j][i].rgbGreen = meshtimh+((matrix[j][i].rgbGreen-meshtimh)+(percent*(matrix[j][i].rgbGreen-meshtimh)));
				matrix[j][i].rgbRed = meshtimh+((matrix[j][i].rgbRed-meshtimh)+(percent*(matrix[j][i].rgbRed-meshtimh)));
				fwrite(&matrix[j][i], sizeof(pixel), 1, fp2); 
			}
		}
							
		// add the padding
		for (k=0; k < padding; k++)
		{
			fputc(0x00, fp2);//fill with zeros in hex
		}
	}
						
	fclose(fp1);
	fclose(fp2);
	for(i=0;i<inf1->bmiHeader.biWidth;i++)
		free(matrix[i]);
	free(matrix);
	printf("> file %s contrast changed and new image saved to %s\n", filename, filename2);
	//free every dynamically allocated space I havent freed yet
	free(inf1);
	free(h1);
	free(filename);
	free(filename2);
						
	return 0;			
}
				
				
int contautof(char* filename, char* filename2)
{
	FILE *fp1;
	int padding;
	int i, j, k;
	bmpInfo *inf1;
	bmpHeader *h1;
				
	if((h1 = (bmpHeader*)malloc(sizeof(bmpHeader)*1)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
					
	if((inf1 = (bmpInfo*)malloc(sizeof(bmpInfo)*1)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
						
	unsigned char *img;
	fp1 = fopen(filename,"rb");
	if (fp1 == NULL){
		puts("file doesn't exist");
		exit(1);
	}
			
	if(readHeader( fp1, h1 ) == ERROR){
		printf("header error\n");
		fclose(fp1);
		exit(1);
	}

	if(readInfo( fp1, inf1 ) == ERROR){
		printf("header INFO error \n");
		fclose(fp1);
		exit(1);
	}	
			
	double percent=0.1;
	if((img = (unsigned char*)malloc(sizeof(unsigned char) *  inf1->bmiHeader.biSizeImage)) == NULL){
		puts("NO SPACE AVAILABLE");
		exit(1);
	} 
	//read image file 
	fread(img,inf1->bmiHeader.biSizeImage,1,fp1);
	int meshtimh=0;
	i=0;
	//till its end
	for(j = 0; j< inf1->bmiHeader.biSizeImage; j++ ){
		i++;
		meshtimh+=img[j];//find the average of 3 bytes
		if(i==3)//every 3 bytes
		{
			for(k=j-2;k<=j;k++)
			{
				// + contrast
				img[k] = meshtimh+((img[k]-meshtimh)+(percent*(img[k]-meshtimh)));
			}
			meshtimh=0;
			i=0;
		}
	}
	fclose(fp1);
	fp1 = fopen(filename2,"wb");
	writeHeader (fp1, inf1);
	writeInfo(fp1, inf1) ;				
	fwrite(img,inf1->bmiHeader.biSizeImage,1,fp1);
	fclose(fp1);
	printf("> file %s contrast changed automatically and new image saved to %s\n", filename, filename2);
	free(img);

	//free every dynamically allocated space I havent freed yet
	free(inf1);
	free(h1);
	free(filename);
	free(filename2);
							
	return 0;
}

				
				
int flipf(char* filename, char* filename2, int flip)
{
	FILE *fp1, *fp2;
	int padding;
	int i, j, k;
	bmpInfo *inf1;
	bmpHeader *h1;
					
	if((h1 = (bmpHeader*)malloc(sizeof(bmpHeader)*1)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
					
	if((inf1 = (bmpInfo*)malloc(sizeof(bmpInfo)*1)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
	
	fp1 = fopen(filename,"rb");
	if (fp1 == NULL){
		puts("file doesn't exist");
		exit(1);
	}
					
	if(readHeader( fp1, h1 ) == ERROR){
		printf("header error\n");
		fclose(fp1);
		exit(1);
	}

	if(readInfo( fp1, inf1 ) == ERROR){
		printf("header INFO error \n");
		fclose(fp1);
		exit(1);
	}
	fp2 = fopen(filename2,"wb");
		if (fp2 == NULL){
			puts("file doesn't exist");
			exit(1);
		}
	padding =  (4 - (inf1->bmiHeader.biWidth * sizeof(pixel)) % 4) % 4;
						
		pixel **matrix;
		if ((matrix = (pixel**)malloc( inf1->bmiHeader.biWidth *sizeof(pixel*))) == NULL)
		{
			printf("No memory available, allocation failed!\nThe program is about to exit...\n");
			exit(0);
		}
		for (i=0; i< inf1->bmiHeader.biWidth; i++)
			if ((matrix[i] = (pixel*)malloc( inf1->bmiHeader.biHeight*sizeof(pixel))) == NULL)
			{
					printf("No memory available, allocation failed!\nThe program is about to exit...\n");
					exit(0);
			}
	for (i=0; i < inf1->bmiHeader.biHeight; i++)
	{
		for (j=0; j < inf1->bmiHeader.biWidth; j++)
		{
			fread(&matrix[j][i], sizeof(pixel), 1, fp1);
		}
		fseek(fp1, padding, SEEK_CUR);
	}
					
	if (flip == 1)//v
	{	
		//find the height centre
		int mid = inf1->bmiHeader.biHeight/2;
						
		writeHeader (fp2, inf1);
		writeInfo(fp2, inf1) ;
		//write the right data from the matrix to the file
		for (i=mid; i < inf1->bmiHeader.biHeight; i++)
		{
			for (j=0; j < inf1->bmiHeader.biWidth; j++)
			{
				fwrite(&matrix[j][i], sizeof(pixel), 1, fp2); 
			}
							
			// add the padding
			for (k=0; k < padding; k++)
			{
				fputc(0x00, fp2);//fill with zeros in hex
			}
		}
		for (i=0; i < mid; i++)
		{
			for (j=0; j < inf1->bmiHeader.biWidth; j++)
			{
				fwrite(&matrix[j][i], sizeof(pixel), 1, fp2); 
			}
							
			// add the padding
			for (k=0; k < padding; k++)
			{
				fputc(0x00, fp2);//fill with zeros in hex
			}
		}
		fclose(fp1);
		fclose(fp2);
		for(i=0;i<inf1->bmiHeader.biWidth;i++)
			free(matrix[i]);
		free(matrix);
		printf("> file %s flipped vertically and saved to %s\n", filename, filename2);
	}
					
	else if(flip == 2)//h
	{
		int mid = inf1->bmiHeader.biWidth/2;
						
		writeHeader (fp2, inf1);
		writeInfo(fp2, inf1) ;
					
		pixel temp;	
		k = mid;
		//find the matrix elements values by swapping the right elements
		for (i=0; i < inf1->bmiHeader.biHeight; i++)
		{
					for (j=0; j <  inf1->bmiHeader.biWidth/2; j++)
					{
						temp = matrix[j][i];
						matrix[j][i] = matrix[k][i];
						matrix[k][i] = temp;
						k++;					
					}
					k= mid;
		}
						
		for (i=0; i < inf1->bmiHeader.biHeight; i++)
		{
			for (j=0; j < inf1->bmiHeader.biWidth; j++)
			{
				fwrite(&matrix[j][i], sizeof(pixel), 1, fp2); 
			}
							
			// add the padding
			for (k=0; k < padding; k++)
			{
				fputc(0x00, fp2);//fill with zeros in hex
			}
		}
					
		fclose(fp1);
		fclose(fp2);
		for(i=0;i<inf1->bmiHeader.biWidth;i++)
			free(matrix[i]);
		free(matrix);
		printf("> file %s flipped horizontally and saved to %s\n", filename, filename2);
	}
	//free every dynamically allocated space I havent freed yet
	free(inf1);
	free(h1);
	free(filename);
	free(filename2);
						
	return 0;
}
				

int scalef(char* filename, char* filename2, int scale, int scalefactor)
{
	FILE *fp1, *fp2;
	int padding;
	int i, j, k;
	bmpInfo *inf1;
	bmpHeader *h1;
		
	fp1 = fopen(filename,"rb");
	if (fp1 == NULL){
		puts("file doesn't exist");
		exit(1);
	}
					
	if((h1 = (bmpHeader*)malloc(sizeof(bmpHeader)*1)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
					
	if((inf1 = (bmpInfo*)malloc(sizeof(bmpInfo)*1)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
					
	if(readHeader( fp1, h1 ) == ERROR){
		printf("header error\n");
		fclose(fp1);
		exit(1);
	}

	if(readInfo( fp1, inf1 ) == ERROR){
		printf("header INFO error \n");
		fclose(fp1);
		exit(1);
	}
	
	fp2 = fopen(filename2,"wb");
	if (fp2 == NULL){
		puts("file doesn't exist");
		exit(1);
	}
	
	// determine the padding
	padding =  (4 - (inf1->bmiHeader.biWidth * sizeof(pixel)) % 4) % 4;
						
	//our rgb array dynamically allocated
	pixel **matrix;
	if ((matrix = (pixel**)malloc( inf1->bmiHeader.biWidth *sizeof(pixel*))) == NULL)
	{
		printf("No memory available, allocation failed!\nThe program is about to exit...\n");
		exit(0);
	}
	for (i=0; i< inf1->bmiHeader.biWidth; i++)
		if ((matrix[i] = (pixel*)malloc( inf1->bmiHeader.biHeight*sizeof(pixel))) == NULL)
		{
				printf("No memory available, allocation failed!\nThe program is about to exit...\n");
				exit(0);
		}
					
	for (i=0; i < inf1->bmiHeader.biHeight; i++)
	{
		for (j=0; j < inf1->bmiHeader.biWidth; j++)
		{
			fread(&matrix[j][i], sizeof(pixel), 1, fp1);
		}				
		// skip padding
		fseek(fp1, padding, SEEK_CUR);
	}
	
	if (scale==1)//up
	{				
		int oldwidth=inf1->bmiHeader.biWidth ;
		int oldheight=inf1->bmiHeader.biWidth ;
		//change width and height 
		inf1->bmiHeader.biWidth *= scalefactor;
		inf1->bmiHeader.biHeight *= scalefactor;
						
		// new padding
		padding = (4 - ( inf1->bmiHeader.biWidth * sizeof(pixel)) % 4) % 4;
						
		//new image size
		inf1->bmiHeader.biSizeImage = (inf1->bmiHeader.biHeight) * ( inf1->bmiHeader.biWidth * sizeof(pixel) + padding);
						
		writeHeader (fp2, inf1);
		writeInfo(fp2, inf1);
						
		// create the new scaled image by using the scalefactor and writing the pixels to the file
		for (i=0; i < inf1->bmiHeader.biHeight; i++)
		{
			for ( j = 0; j < inf1->bmiHeader.biWidth; j++)
			{
				pixel temp;
				// use the scale factor to find the colour of the new pixel by writing more duplicate
				// pixels as many as the scale factor number
				temp = matrix[(int)(j / scalefactor)][(int)(i / scalefactor)];
				fwrite(&temp, sizeof(pixel), 1, fp2); 
			}
							
			// add the padding
			for (k=0; k < padding; k++)
			{
					fputc(0x00, fp2);//fill with zeros in hex
			}
		}
						
		printf("> file %s scaled up by %d and saved to %s\n", filename, scalefactor, filename2);
		fclose(fp1);
		fclose(fp2);
		for(i=0;i<oldwidth;i++)
			free(matrix[i]);
		free(matrix);
	}
	else if (scale == 2)//down
	{					
		int oldwidth=inf1->bmiHeader.biWidth ;
		int oldheight=inf1->bmiHeader.biWidth ;
		//new width and height				
		inf1->bmiHeader.biWidth /= scalefactor;
		inf1->bmiHeader.biHeight /= scalefactor;
						
		padding = (4 - ( inf1->bmiHeader.biWidth * sizeof(pixel)) % 4) % 4;
						
		// image size without the header 
		inf1->bmiHeader.biSizeImage = (inf1->bmiHeader.biHeight) * ( inf1->bmiHeader.biWidth * sizeof(pixel) + padding);
						
		writeHeader (fp2, inf1);
		writeInfo(fp2, inf1);
						
		// create the new scaled image by using the scalefactor and writing the pixels to the file
		for (i=0; i < inf1->bmiHeader.biHeight; i++)
		{
			for (j=0; j < inf1->bmiHeader.biWidth; j++)
			{
				pixel temp;
				//again use the scale factor in the opposite way to write less pixels of the original image
				temp = matrix[(int) (j * scalefactor)][(int) (i * scalefactor)];
				fwrite(&temp, sizeof(pixel), 1, fp2); 
			}
							
			// add the padding
			for (k=0; k < padding; k++)
			{
				fputc(0x00, fp2);//fill with zeros in hex
			}
		}
		printf("> file %s scaled down by %d and saved to %s\n", filename, scalefactor, filename2);
		fclose(fp1);
		fclose(fp2);
		for(i=0;i<oldwidth;i++)
			free(matrix[i]);
		free(matrix);	
	}
	
	//free every dynamically allocated space I havent freed yet
	free(inf1);
	free(h1);
	free(filename);
	free(filename2);
						
	return 0;
}
				

		
				
int flipm2f(char* filename, char* filename2, int flipm2)
{
	FILE *fp1, *fp2;
	int padding;
	int i, j, k;
	bmpInfo *inf1;
	bmpHeader *h1;
					
	fp1 = fopen(filename,"rb");
	if (fp1 == NULL){
		puts("file doesn't exist");
		exit(1);
	}
					
	if((h1 = (bmpHeader*)malloc(sizeof(bmpHeader)*1)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
					
	if((inf1 = (bmpInfo*)malloc(sizeof(bmpInfo)*1)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
					
	if(readHeader( fp1, h1 ) == ERROR){
		printf("header error\n");
		fclose(fp1);
		exit(1);
	}

	if(readInfo( fp1, inf1 ) == ERROR){
		printf("header INFO error \n");
		fclose(fp1);
		exit(1);
	}
	
	fp2 = fopen(filename2,"wb");
	if (fp2 == NULL){
		puts("file doesn't exist");
		exit(1);
	}
	
	padding =  (4 - (inf1->bmiHeader.biWidth * sizeof(pixel)) % 4) % 4;
						
		pixel **matrix;
		if ((matrix = (pixel**)malloc( inf1->bmiHeader.biWidth *sizeof(pixel*))) == NULL)
		{
			printf("No memory available, allocation failed!\nThe program is about to exit...\n");
			exit(0);
		}
		for (i=0; i< inf1->bmiHeader.biWidth; i++)
			if ((matrix[i] = (pixel*)malloc( inf1->bmiHeader.biHeight*sizeof(pixel))) == NULL)
				{
					printf("No memory available, allocation failed!\nThe program is about to exit...\n");
					exit(0);
				}	
	for (i=0; i < inf1->bmiHeader.biHeight; i++)
	{
		for (j=0; j < inf1->bmiHeader.biWidth; j++)
		{
			fread(&matrix[j][i], sizeof(pixel), 1, fp1);
		}
		fseek(fp1, padding, SEEK_CUR);
	}
					
	if (flipm2 == 1)//v
	{	
			int mid = inf1->bmiHeader.biHeight/2;
						
			writeHeader (fp2, inf1);
			writeInfo(fp2, inf1) ;
						
			for (i=0; i < inf1->bmiHeader.biHeight; i++)
			{
				for (j=0; j < inf1->bmiHeader.biWidth; j++)
				{
					pixel temp;
					temp = matrix[j][(inf1->bmiHeader.biHeight-1)-i];
					fwrite(&temp, sizeof(pixel), 1, fp2); 
				}
							
				// add the padding
				for (k=0; k < padding; k++)
				{
					fputc(0x00, fp2);//fill with zeros in hex
				}
			}
						
		fclose(fp1);
		fclose(fp2);
		for(i=0;i<inf1->bmiHeader.biWidth;i++)
		free(matrix[i]);
		free(matrix);
		printf("> file %s flipped by flipm2 vertically and saved to %s\n", filename, filename2);
	}
					
	else if(flipm2==2)//h
	{
		int mid = inf1->bmiHeader.biWidth/2;
						
		writeHeader (fp2, inf1);
		writeInfo(fp2, inf1) ;
						
		for (i=0; i < inf1->bmiHeader.biHeight; i++)
		{
			for (j=0; j < inf1->bmiHeader.biWidth; j++)
				{
					pixel temp;
					temp = matrix[(inf1->bmiHeader.biWidth-1)-j][i];
					fwrite(&temp, sizeof(pixel), 1, fp2); 
				}
							
		// add the padding
			for (k=0; k < padding; k++)
				{
					fputc(0x00, fp2);//fill with zeros in hex
				}
		}
					
		fclose(fp1);
		fclose(fp2);
		for(i=0;i<inf1->bmiHeader.biWidth;i++)
			free(matrix[i]);
		free(matrix);
		printf("> file %s flipped by flipm2 horizontally and saved to %s\n", filename, filename2);
	}
	
	//free every dynamically allocated space I havent freed yet
	free(inf1);
	free(h1);
	free(filename);
	free(filename2);	
	
	return 0;					
}				
					
			
//just make a copy of the image with the easiest way possible			
int copyf(char* filename, char* filename2)
{
	FILE *fp1, *fp2;
	int padding;
	int i, j, k;
	bmpInfo *inf1;
	bmpHeader *h1;
	unsigned char* img;
					
	fp1 = fopen(filename,"rb");
	if (fp1 == NULL){
		puts("file doesn't exist");
		exit(1);
	}
					
	if((h1 = (bmpHeader*)malloc(sizeof(bmpHeader)*1)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
					
	if((inf1 = (bmpInfo*)malloc(sizeof(bmpInfo)*1)) == NULL)
	{
		puts("NO SPACE AVAILABLE");
		exit(1);
	}
					
	if(readHeader( fp1, h1 ) == ERROR){
		printf("header error\n");
		fclose(fp1);
		exit(1);
	}

	if(readInfo( fp1, inf1 ) == ERROR){
		printf("header INFO error \n");
		fclose(fp1);
		exit(1);
	}
	
	if((img = (unsigned char*)malloc(sizeof(unsigned char) *  inf1->bmiHeader.biSizeImage)) == NULL){
		puts("NO SPACE AVAILABLE");
		exit(1);
	} 
	//read and write the whole image
	fread(img,inf1->bmiHeader.biSizeImage,1,fp1);
	fclose(fp1);
	fp1 = fopen(filename2,"wb");
	writeHeader (fp1, inf1);
	writeInfo(fp1, inf1);
	//write the one image which needs biSizeImage space to the new file
	fwrite(img,inf1->bmiHeader.biSizeImage,1,fp1);
	fclose(fp1);
	printf("> file %s copied to %s\n", filename, filename2);
	free(img);
	
	//free every dynamically allocated space I havent freed yet
	free(inf1);
	free(h1);
	free(filename);
	free(filename2);
			
	return 0;
}
			
			


