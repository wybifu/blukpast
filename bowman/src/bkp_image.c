#include <stdlib.h>
#include <stdio.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/lodepng.h"

/**************************************************************************
*	Defines & Maro
**************************************************************************/

/**************************************************************************
*	Structs, Enum, Unio and Typesdef	
**************************************************************************/

/**************************************************************************
*	Globals	
**************************************************************************/

/***************************************************************************
*	Prototypes 	
**************************************************************************/


/**************************************************************************
*	Main	
**************************************************************************/

/*
int main(int argc, char **argv)
{
	unsigned char * image = NULL;
	unsigned width,height;


	image = load_png("bg.png",&width,&height);
	if(image == NULL)
	{
		fprintf(stderr,"exiting\n");
		return EXIT_FAILURE;
	}

	free(image);
	
	return EXIT_SUCCESS;
}
*/


/**************************************************************************
*	Implementations	
**************************************************************************/

unsigned char * bkp_image_loadPNG(const char *path, unsigned int *  w, unsigned int * h)
{
	unsigned error;
	unsigned char * png = NULL;
	unsigned char * image = NULL;
	size_t pngsize;

	error = lodepng_load_file(&png,&pngsize, path);
	if( !error) error = lodepng_decode32(&image,w, h,png,pngsize);
	if(error)
	{
		free(png);
		fprintf(stderr,"error %u: %s\n",error,lodepng_error_text(error));
		return NULL;
	}

	free(png);

	return image;
}

/*--------------------------------------------------------------------------------*/
void bkp_image_vflip(unsigned char *img,unsigned int w, unsigned int h)
{
	int width_in_byte = w * 4;
	unsigned char *top = NULL;
	unsigned char * bottom = NULL;
	unsigned char tmp = 0;
	int half_height = h / 2;

	int row,col;

	for(row = 0; row < half_height; ++row)
	{
		top = img + row * width_in_byte;
		bottom = img +(h - row - 1) * width_in_byte;
		for(col = 0; col < width_in_byte; ++col)
		{
			tmp = *top;
			*top = *bottom;
			*bottom = tmp;
			++top;
			++bottom;
		}
	}

	return;
}
/*--------------------------------------------------------------------------------*/
void bkp_image_hflip(unsigned char *img,unsigned int w, unsigned int h)
{
	int width_in_byte = w * 4;
	unsigned char *top = NULL;
	unsigned char * bottom = NULL;
	unsigned char * left_t= NULL;
	unsigned char * right_t= NULL;
	unsigned char * left_b= NULL;
	unsigned char * right_b= NULL;
	unsigned char tmp = 0;
	int half_height = h / 2;
	int half_width = w / 2;

	int row,col,i;

	for(row = 0; row < half_height; ++row)
	{
		top = img + row * width_in_byte;
		bottom = img +(h - row - 1) * width_in_byte;
		left_t = top;		
		left_b = bottom;		
		right_t = top + width_in_byte ;		
		right_b = bottom;		
		for(col = 0; col < half_width ; ++col)
		{
			left_t = top + (col * 4);
			right_t = top + width_in_byte - 4 - (col * 4);
			left_b = bottom + (col * 4);
			right_b = bottom + width_in_byte - 4 - (col * 4);
			for(i = 0; i < 4; ++i,++left_t, ++ left_b, ++right_t, ++right_b)
			{
				tmp = *left_t;
				*left_t = *right_t;
				*right_t = tmp;

				tmp = *left_b;
				*left_b = *right_b;
				*right_b = tmp;
			}
		}
	}

	return;
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

