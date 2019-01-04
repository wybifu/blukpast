#ifndef __BKP_INPUT_H_
#define __BKP_INPUT_H_

#include "bkp_def.h"

/*********************************************************************
 * Defines 
*********************************************************************/

#define BKP_INPUT_PRESSURE_TIMER .2 
#define BKP_INPUT_KEYPAUSE_TIMER .05

/*********************************************************************
 * Type def & enum
*********************************************************************/

typedef void (*call_b3)(GLFWwindow * window, double xoffset, double yoffset);

/*********************************************************************
 * Macro 
*********************************************************************/

/*********************************************************************
 * Struct 
*********************************************************************/

typedef struct BKP_InputKey
{
	double s_time;
	double s_pause;
	int pressed;
	int pause;

}BKP_InputKey;

/*------------------------------ Input ----------------------------*/
typedef struct InputMouse
{
	BKP_InputKey l_click, r_click, m_click;
	float scrollup, scrolldown;
	BKP_Vec2 pos;

}BKP_InputMouse;

/*------------------------------ Input ----------------------------*/
typedef struct BKP_Input 
{
	int keypressed;
	int Start,Cancel;

	BKP_InputKey Action0,Action1,Action2,Action3;
	BKP_InputKey Gachette0,Gachette1,Gachette2,Gachette3;
	BKP_InputKey left,right,up,down;
	BKP_InputMouse Mouse;

}BKP_Input;


/*********************************************************************
 * Global 
*********************************************************************/

/*********************************************************************
 * Struct 
*********************************************************************/

/*********************************************************************
 * Functions 
*********************************************************************/

void bkp_input_Init(void);
BKP_Input * bkp_input_New(void);
void bkp_input_reset(BKP_Input *input);
void bkp_input_resetkeys(BKP_Input *p);
void bkp_input_capture(BKP_Input * p);
void bkp_input_mouseCapture(BKP_Input * p);
void bkp_input_setPressureTimer(float tm);
void bkp_input_setKeyPauseTimer(float tm);
void bkp_input_setKeyPause(BKP_InputKey *key);
void bkp_input_setMouse(int mode, int cursormode, call_b3 mouse_callback, call_b3 scroll_callback);

#endif

