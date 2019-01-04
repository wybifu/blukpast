#ifndef __BKP_LOGGER_H_
#define __BKP_LOGGER_H_

/*********************************************************************
 * Defines 
*********************************************************************/

#define BKP_LOGGER_LINESIZE 480
#define BKP_LOGGER_MAXBUFFER 512

/*********************************************************************
 * Type def & enum
*********************************************************************/

enum {BKP_LOGGER_DEBUG,BKP_LOGGER_INFO,BKP_LOGGER_WARNING,BKP_LOGGER_ERROR,BKP_LOGGER_FATAL};
enum {BKP_LOGGER_TERMOUT = 1,BKP_LOGGER_FILEOUT = 2,BKP_LOGGER_DRAWOUT = 4, BKP_LOGGER_ALLOUT = 100};
enum {BKP_LOGGER_ONONE,BKP_LOGGER_OWRITE,BKP_LOGGER_OAPPEND};

/*********************************************************************
 * Macro 
*********************************************************************/

/*********************************************************************
 * Struct 
*********************************************************************/

/*********************************************************************
 * Global 
*********************************************************************/

/*********************************************************************
 * Struct 
*********************************************************************/

/*********************************************************************
 * Functions 
*********************************************************************/

int bkp_logger_init(int level,int output,const char * logdir,char append);
void bkp_logger_write(int level,const char * message, ...);
void bkp_logger_terminate(void);
void bkp_logger_setFlushAt(int line); //by default flush everyline, line < BKP_LOGGER_MAXBUFFER

#endif

