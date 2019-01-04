#ifndef __SCROLLER_H_
#define __SCROLLER_H_

#include "bkp_struct.h"

/*********************************************************************
 * Defines 
*********************************************************************/

#define CAMERA_PANIC_UP 100
#define CAMERA_PANIC_DOWN 20
#define CAMERA_SPEED_INITX 16
#define CAMERA_SPEED_INITY 64
#define CAMERA_MIN_SPEEDX 2 
#define CAMERA_MIN_SPEEDY .8
#define CAMERA_SPEEDY 16 

/*********************************************************************
 * Type def & enum
*********************************************************************/

enum {BKP_CAMERAFUNC_FOCUS = 0, BKP_CAMERAFUNC_MOVETO, BKP_CAMERAFUNC_FIXEDSETFOCUS, 
	BKP_CAMERAFUNC_FIXED, BKP_CAMERAFUNC_LOCKON, BKP_CAMERAFUNC_SMOOTHSTD};

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

int camera2D(BKP_Camera * S, BKP_Dynamics2D P);

int camera2DFocus(BKP_Camera * S, BKP_Dynamics2D P);
int camera2DMoveTo(BKP_Camera * S, BKP_Dynamics2D P);
int camera2DFixed(BKP_Camera * S, BKP_Dynamics2D P);
int camera2DFixedSetFocus(BKP_Camera * S, BKP_Dynamics2D P);
int scroll2DInitial(BKP_Camera * S, BKP_Dynamics2D P);
int camera2DLockon(BKP_Camera * S, BKP_Dynamics2D P);
int camera2DSmoothStd(BKP_Camera * S, BKP_Dynamics2D P);


#endif

