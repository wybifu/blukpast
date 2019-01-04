#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <string.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/blukpast.h"

/**************************************************************************
*	Defines & Maro
**************************************************************************/

/**************************************************************************
*	Structs, Enum, Unio and Typesdef	
**************************************************************************/

/**************************************************************************
*	Globals	
**************************************************************************/
int _Screen_w = 0, 
	_Screen_h = 0, 
	_Screen_full = BKP_FALSE;
char  _Screen_title[64] = "BKP Game Engine";

int _OGL_Ver = 4, 
	_OGL_ver = 1;

int _Logger = BKP_TRUE,
	_Logger_LEV = BKP_LOGGER_DEBUG,
	_Logger_OUT = BKP_LOGGER_TERMOUT | BKP_LOGGER_FILEOUT,
	_Logger_MODE = BKP_LOGGER_OWRITE;

const char *_Logger_p = NULL;

/***************************************************************************
*	Prototypes 	
**************************************************************************/

static void pwdMe(const char * arg);

/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/

/*--------------------------------------------------------------------------------*/
void bkp_setWindowInfo(int w, int h, int fullscreen,const char *sztitle)
{
	_Screen_w = w;
	_Screen_h = h;
	_Screen_full = fullscreen;
	sprintf(_Screen_title, "%s", sztitle);

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_setLogInfo(const char  * path,int lev, int out, int mode)
{
	_Logger_p = path;	
	_Logger_LEV = lev;
	_Logger_OUT = out;
	_Logger_MODE = mode;

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_setOpenGLInfo(int V,int v)
{
	_OGL_Ver = V;
	_OGL_ver = v;

	return;
}
/*--------------------------------------------------------------------------------*/
int bkp_startEngine(char ** argv)
{
	int ret;
	pwdMe(argv[0]);

	if(_Logger == BKP_TRUE)
		bkp_logger_init(_Logger_LEV, _Logger_OUT,(_Logger_p == NULL) ? "./" : _Logger_p, _Logger_MODE);

	/* graphics engine start */

	bkp_logger_write(BKP_LOGGER_INFO,"Creating Memory pools...\n");
	bkp_memory_init(-1); // -1 = infinit number

	bkp_logger_write(BKP_LOGGER_INFO,"Starting Graphics Engine ...\n");
	ret = bkp_graphics_init(_Screen_w,_Screen_h, _Screen_full,_Screen_title,
			_OGL_Ver, _OGL_ver);

	if(ret != BKP_TRUE)
	{
		bkp_logger_write(BKP_LOGGER_INFO,"Starting Graphics Engine [Failed]\n");
		return BKP_FALSE;
	}
	bkp_logger_write(BKP_LOGGER_INFO,"Graphics Engine started\n");

	bkp_material_initPlatform();

	/* //////////////////////// */


	return BKP_TRUE;
}

/*--------------------------------------------------------------------------------*/
void bkp_stopEngine()
{
	bkp_material_releasePlatform();

	bkp_logger_write(BKP_LOGGER_INFO,"\n\n");
	bkp_graphics_close();
	bkp_logger_write(BKP_LOGGER_INFO,"Graphics Engine stopped [OK]\n");


	bkp_memory_destroyPools();
	bkp_logger_write(BKP_LOGGER_INFO,"Destroying Memory pools[OK]\n");

	bkp_logger_terminate();
	bkp_logger_write(BKP_LOGGER_INFO,"Logger closed [OK]\n");

	bkp_logger_write(BKP_LOGGER_INFO,"Game Engine stopped [OK]\n");


	return;
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

static void pwdMe(const char * arg)
{
	int j;
	int i = strlen(arg)-1;
	char path[256] = {0};

	for(; i>0; --i)
	{
#ifdef _WIN32
		if(arg[i]=='\\')
			break;
#else
		if(arg[i]=='/')
			break;
#endif
	}

	if(i<1)
		return;

	for(j=0; j<=i; ++j)
		path[j]=arg[j];

	fprintf(stdout,"Moving to directory `%s`\n",path);                                                                                               
	if(chdir(path)<0)
	{
		fprintf(stderr,"Error on '%s'\n",path);
		exit(EXIT_FAILURE);
	}
	return;
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

