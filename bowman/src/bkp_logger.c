#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/bkp_logger.h"

/**************************************************************************
*	Defines & Maro
**************************************************************************/

#define M 32 + BKP_LOGGER_LINESIZE 

/**************************************************************************
*	Structs, Enum, Unio and Typesdef	
**************************************************************************/

struct entry
{
	char log[BKP_LOGGER_LINESIZE];
	int lev;

}entry;

/**************************************************************************
*	Globals	
**************************************************************************/
int _index = 0;
int stc_output = BKP_LOGGER_TERMOUT;
struct entry stc_buffer[BKP_LOGGER_MAXBUFFER];  // write only N lines of M caracters;
int stc_inited = 0;
pthread_mutex_t stc_mutex_; 
FILE * outf;
int stc_loglevel,
	stc_maxbuffer_line,
	_fok;
char Ll[5][8] = {" DEBUG "," INFO  ","WARNING"," ERROR "," FATAL "};
char stc_path [BKP_LOGGER_LINESIZE];

/***************************************************************************
*	Prototypes 	
**************************************************************************/

static void moveInBuffer(int level,const char * sz);
static void doFlush(void);

/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/

int bkp_logger_init(int level,int output,const char * logdir,char append)
{
	char mydir[BKP_LOGGER_LINESIZE / 2];
	DIR * dir = NULL;
	char tmp[M];

	_fok = 0;

	if(level == -1)
	{
		stc_inited = 0;
		return 0;
	}

	stc_maxbuffer_line = 1; //default, flush every lines to modify use bkp_logger_setFlushAt()
	stc_output = output;
	stc_loglevel = level;

	if(NULL == logdir)
		sprintf(mydir,"%s","./logs/");
	else
		sprintf(mydir,"%s",logdir);

	dir = opendir(mydir);
	stc_inited = 1;

	if(dir)
	{
		closedir(dir);
	}
	else
	{
		sprintf(tmp,"Specified directory not found '%s'",mydir);
		bkp_logger_write(BKP_LOGGER_ERROR,tmp);
		stc_inited = 0;
		return -1;
	}

	sprintf(stc_path,"%s/log.txt",mydir);
	if(append == BKP_LOGGER_OAPPEND)
	{
		outf = fopen(stc_path,"at");
	}
	else
	{
		outf = fopen(stc_path,"wt");
		if(append != BKP_LOGGER_OWRITE)
			bkp_logger_write(BKP_LOGGER_WARNING,"Unknow openning file mode, LOGGER will open file as 'w'");
	}

	if(outf == NULL)
	{
		sprintf(tmp,"Cannot open '%s'",stc_path);
		bkp_logger_write(BKP_LOGGER_ERROR,tmp);
		stc_inited = 0;
		return -1;
	}

	_fok = 1;
	fclose(outf);

	pthread_mutex_t lk = PTHREAD_MUTEX_INITIALIZER;
	stc_mutex_ = lk;

	return 0;	
}

/*--------------------------------------------------------------------------------*/
void bkp_logger_write(int level,const char * message,...)
{
	if(0 == stc_inited)
		return;

	if(stc_loglevel <= level)
	{
		time_t     now;
		char mess[M];
		char *p;
		char tmp[24];
		struct tm  ts;
		va_list argptr;


		time(&now);

		ts = *localtime(&now);
		strftime(tmp, M, "%Y-%m-%d %H:%M:%S", &ts);
		sprintf(mess,"[%s][ %s ] -> ",Ll[level],tmp);
		p = mess +strlen(mess);


		va_start(argptr, message);
		vsprintf(p,message,argptr);
		va_end(argptr);
		moveInBuffer(level,mess);

		if(stc_output == BKP_LOGGER_TERMOUT || stc_output == (BKP_LOGGER_TERMOUT | BKP_LOGGER_FILEOUT) || stc_output > BKP_LOGGER_DRAWOUT)
		{
			pthread_mutex_lock(&stc_mutex_);
			fprintf(stderr,"%s",mess);
			pthread_mutex_unlock(&stc_mutex_);
		}
	}

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_logger_terminate()
{
	doFlush();
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_logger_setFlushAt(int line)
{
	stc_maxbuffer_line = line;
	return;
}

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
static void moveInBuffer(int level,const char * sz)
{
	stc_buffer[_index].lev = level;
	strcpy(stc_buffer[_index].log,sz);
	++_index;
	if(_index >= stc_maxbuffer_line)
		doFlush();

	return;
}
/*--------------------------------------------------------------------------------*/
static void doFlush()
{
	int i = _index,
		j;
	_index = 0;

	if(BKP_LOGGER_TERMOUT == stc_output || BKP_LOGGER_DRAWOUT == stc_output || (BKP_LOGGER_DRAWOUT|BKP_LOGGER_TERMOUT) == stc_output)
		return;

	if(0 != _fok)
	{
		pthread_mutex_lock(&stc_mutex_);
		outf = fopen(stc_path,"at");
		if(NULL != outf)
		{
			for(j = 0;j < i;++j) fwrite(stc_buffer[j].log,strlen(stc_buffer[j].log),sizeof(char),outf);
			fclose(outf);
		}
		pthread_mutex_unlock(&stc_mutex_);
	}

	return;
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

/*
*/
