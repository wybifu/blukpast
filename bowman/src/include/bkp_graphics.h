#ifndef __BKP_GRAPHICS_H_
#define __BKP_GRAPHICS_H_

#include "bkp_camera.h"

/*********************************************************************
 * Defines 
*********************************************************************/

/*********************************************************************
 * Type def & enum
*********************************************************************/

enum {BKP_GRAPHICS_FLIPNONE,BKP_GRAPHICS_FLIPH,BKP_GRAPHICS_FLIPV,BKP_GRAPHICS_FLIPHV};
enum {BKP_GRAPHICS_2D_ENGINE, BKP_GRAPHICS_3D_ENGINE};

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

GLFWwindow * bkp_graphics_getWindow(void);
BKP_Vec2i bkp_graphics_getWindowDimensions(void);
BKP_Vec2 bkp_graphics_getTWH(int w, int h);
int bkp_graphics_init(int w, int h, int full,const char *sztitle, int oglV, int oglv);
void bkp_graphics_close(void);
void bkp_graphics_drawSurface(BKP_Surface2D * s,BKP_Rect * dest,BKP_Rect * src,BKP_Rotate *R,int flip , int * alpha);
void bkp_graphics_drawRectangle(BKP_Rect * dest,BKP_Rotate *R, int filled);
void bkp_graphics_ClearColori(unsigned int r, unsigned int g, unsigned int b, unsigned int a);
void bkp_graphics_setClearColorf(float r, float g, float b, float a);
void bkp_graphics_2DsetColorf(float r, float g, float b, float a);
void bkp_graphics_2DsetColori(unsigned int r, unsigned int g, unsigned int b, unsigned int a);
void bkp_graphics_setColorvi(BKP_Vec4i co);
void bkp_graphics_2DsetColorvf(float r, float g, float b, float a);
void bkp_graphics_setSurfaceColori(BKP_Surface2D * s, unsigned int r, unsigned int g, unsigned int b,unsigned int a);
void bkp_graphics_setSurfaceColorf(BKP_Surface2D * s, float r, float g, float b, float a);
BKP_Surface2D * bkp_graphics_2DloadSurface(const char * file);
void bkp_graphics_2DreleaseSurface(BKP_Surface2D *s);
void bkp_graphics_2DReady(int clear);
void bkp_graphics_2DFlush(void);
void bkp_graphics_2DTranslateView(BKP_Vec2 v);
void bkp_graphics_2DTranslateViewAuto(BKP_Vec2 v);

int bkp_graphics_getCameraFunc(void);
void bkp_graphics_2DinitCamera(BKP_Rect dim, BKP_Rect world,int camera_function);

void bkp_graphics_2DsetCameraDim(BKP_Rect dim); 
void bkp_graphics_2DsetCameraWorld( BKP_Rect world);
void bkp_graphics_2DsetCameraAuto(float auto_speed);
void bkp_graphics_2DsetCameraSpeed(BKP_Vec2 speed);
void bkp_graphics_2DsetCameraPanic(float up, float down);

BKP_Rect bkp_graphics_2DgetCameraDim(void);
BKP_Vec2 bkp_graphics_2DgetCameraPos(void);
BKP_Rect bkp_graphics_2DgetCamerafocus(void);
BKP_Rect bkp_graphics_2DgetCameraWorld(void);
BKP_Vec2 bkp_graphics_2DgetCamerafocusLength(void);
BKP_Vec2 bkp_graphics_2DgetCamerapanic(void);

void bkp_graphics_2DtoggleCamera(void);
void bkp_graphics_camera2Dinitial(BKP_Dynamics2D P);

void bkp_graphics_camera2DsetFunc(int function);
int bkp_graphics_camera2D(BKP_Dynamics2D P);

BKP_Font * bkp_graphics_newFont(const char * path, int size, int encoding);
void bkp_graphics_freeFont(BKP_Font * font);
void bkp_graphics_renderText(const char * str, BKP_Font * font, BKP_Vec2 pos, BKP_Vec2 scale, BKP_Vec4 color);
//BKP_ScreenText * bkp_graphics_appendTextPool(char * str, BKP_Font * font, BKP_Vec2 pos, BKP_Vec2 scale, BKP_Vec4 color);
BKP_ScreenText * bkp_graphics_appendTextPool(const char * str, size_t ss, BKP_Font * font, BKP_Vec2 pos, BKP_Vec2 scale, BKP_Vec4 color);
void bkp_graphics_releaseTextPool(BKP_ScreenText * p);
void bkp_graphics_OnScreentext(void);

#endif

