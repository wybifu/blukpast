#include <stdlib.h>
#include <stdio.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/bkp_input.h"
#include "include/bkp_logger.h"
#include "include/bkp_graphics.h"
#include "include/macro.h"
#include "include/bkp_memory.h"

/**************************************************************************
*	Defines & Maro
**************************************************************************/

/**************************************************************************
*	Structs, Enum, Unio and Typesdef	
**************************************************************************/

/**************************************************************************
*	Globals	
**************************************************************************/

GLFWwindow * window = NULL;
float stc_pressure_tm ; 
float stc_keypause_tm ; 
int stc_mouse_mode = -1;
int stc_mouse_cursor_mode = GLFW_CURSOR_NORMAL;

call_b3 stc_mouse_callback;
call_b3 stc_scroll_callback;

/***************************************************************************
*	Prototypes 	
**************************************************************************/

static void _check_key(BKP_InputKey * p, double ticks);

/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/
void bkp_input_Init()
{
	window = bkp_graphics_getWindow();
	stc_pressure_tm = BKP_INPUT_PRESSURE_TIMER;
	stc_keypause_tm = BKP_INPUT_KEYPAUSE_TIMER;


	if(window == NULL)
	{
		BKP_DIE_L("GLFW not initialized cannot init input engine",0x16);
	}

	if(stc_mouse_mode != -1)
	{
//		glfwSetInputMode(window,GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetInputMode(window, stc_mouse_mode, stc_mouse_cursor_mode);
		if(stc_mouse_callback != NULL)
			glfwSetCursorPosCallback(window, stc_mouse_callback);
		if(stc_scroll_callback != NULL)
			glfwSetScrollCallback(window, stc_scroll_callback);
	}

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_input_setMouse(int mode, int cursormode, call_b3 mouse_callback, call_b3 scroll_callback)
{
	stc_mouse_mode = mode;
	stc_mouse_cursor_mode = cursormode;
	stc_mouse_callback = mouse_callback;
	stc_scroll_callback = scroll_callback;

	return;
}

/*--------------------------------------------------------------------------------*/
BKP_Input* bkp_input_New()
{
	BKP_Input* input = NULL;
	input = (BKP_Input *) bkp_memory_getTicket(sizeof(BKP_Input));
	bkp_input_reset(input);

	return input;
}

/*--------------------------------------------------------------------------------*/
void bkp_input_reset(BKP_Input *input)
{
	input->keypressed = 0;
	input->Start = input->Cancel = 0;
	bkp_input_resetkeys(input);

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_input_resetkeys(BKP_Input *p)
{
	p->Action0.pressed = 0; p->Action0.s_time = 0.0; p->Action0.s_pause = 0.0; p->Action0.pause = 0;
	p->Action1.pressed = 0; p->Action1.s_time = 0.0; p->Action1.s_pause = 0.0; p->Action1.pause = 0;
	p->Action2.pressed = 0; p->Action2.s_time = 0.0; p->Action2.s_pause = 0.0; p->Action2.pause = 0;
	p->Action3.pressed = 0; p->Action3.s_time = 0.0; p->Action3.s_pause = 0.0; p->Action3.pause = 0;

	p->Gachette0.pressed = 0; p->Gachette0.s_time = 0.0; p->Gachette0.s_pause = 0.0; p->Gachette0.pause = 0;
	p->Gachette1.pressed = 0; p->Gachette1.s_time = 0.0; p->Gachette1.s_pause = 0.0; p->Gachette1.pause = 0;
	p->Gachette2.pressed = 0; p->Gachette2.s_time = 0.0; p->Gachette2.s_pause = 0.0; p->Gachette2.pause = 0;
	p->Gachette3.pressed = 0; p->Gachette3.s_time = 0.0; p->Gachette3.s_pause = 0.0; p->Gachette3.pause = 0;

	p->left.pressed = 0; p->left.s_time = 0.0; p->left.s_pause = 0.0; p->left.pause = 0;
	p->right.pressed = 0; p->right.s_time = 0.0; p->right.s_pause = 0.0; p->right.pause = 0;
	p->up.pressed = 0; p->up.s_time = 0.0; p->up.s_pause = 0.0; p->up.pause = 0;
	p->down.pressed = 0; p->down.s_time = 0.0; p->down.s_pause = 0.0; p->down.pause = 0;
	p->Cancel = 0;

	p->Mouse.l_click.pressed = p->Mouse.l_click.s_time = 0; p->Mouse.l_click.pause = p->Mouse.l_click.s_pause = 0;
	p->Mouse.r_click.pressed = p->Mouse.r_click.s_time = 0; p->Mouse.r_click.pause = p->Mouse.r_click.s_pause = 0;

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_input_capture(BKP_Input *p)
{
	double ticks = glfwGetTime();
	glfwPollEvents();

	if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		p->keypressed= 1;
		p->Cancel = 1;
		return;
	}

	if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A) || GLFW_PRESS == glfwGetKey(window,GLFW_KEY_LEFT))
	{
		p->keypressed= 1;
		_check_key(&p->left,ticks);
	}
	else if(p->left.pressed > 0)
	{
		p->left.pressed = 0;
		p->left.s_time = 0;
	}

	if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D) || GLFW_PRESS == glfwGetKey(window,GLFW_KEY_RIGHT))
	{
		p->keypressed= 1;
		_check_key(&p->right,ticks);
	}
	else if(p->right.pressed > 0)
	{
		p->right.pressed = 0;
		p->right.s_time = 0;
	}

	if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W) || GLFW_PRESS == glfwGetKey(window,GLFW_KEY_UP))
	{
		p->keypressed= 1;
		_check_key(&p->up,ticks);
	}
	else if(p->up.pressed > 0)
	{
		p->up.pressed = 0;
		p->up.s_time = 0;
	}

	if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S) || GLFW_PRESS == glfwGetKey(window,GLFW_KEY_DOWN))
	{
		p->keypressed= 1;
		_check_key(&p->down,ticks);
	}
	else if(p->down.pressed > 0)
	{
		p->down.pressed = 0;
		p->down.s_time = 0;
	}

	if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE))
	{
		p->keypressed= 1;
		_check_key(&p->Action0,ticks);
	}
	else if(p->Action0.pressed > 0)
	{
		p->Action0.pressed = 0;
		p->Action0.s_time = 0;
	}

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_input_setPressureTimer(float tm)
{
	stc_pressure_tm = tm;
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_input_setKeyPauseTimer(float tm)
{
	stc_keypause_tm = tm;
	return;
}

/*--------------------------------------------------------------------------------*/
/* for short pressure on a key  put in the function that check key e.g: if(left.presse) ...*/
void bkp_input_setKeyPause(BKP_InputKey *key)
{
	key->pressed = 0;
	key->pause = 1;
	key->s_pause = glfwGetTime();

	return;
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
static void _check_key(BKP_InputKey * p, double ticks)
{
	if(p->pause != 0)
	{
		if(ticks - p->s_pause >= stc_keypause_tm)
			p->pause = 0;
	}
	if(p->pause == 0)
	{
		if(p->pressed == 0)
			p->s_time = ticks;

		if(ticks - p->s_time < stc_pressure_tm)
			p->pressed = 1;
		else
			p->pressed = 2;
	}
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

