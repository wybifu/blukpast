#ifndef __BKP_DEF_H_
#define __BKP_DEF_H_

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "bkp_float.h"

/*********************************************************************
 * Defines 
*********************************************************************/

#define BBOXES 
//#define PLAYER_BBOXES 

#define BKP_2D_MAX_DIR 4

enum {BKP_FALSE, BKP_TRUE, BKP_DONE};
enum {BKP_DECELERATE, BKP_ACCELERATE};
enum {BKP_UP, BKP_LEFT, BKP_DOWN, BKP_RIGHT};
enum {BKP_MOTION_STAND, BKP_MOTION_FALL, BKP_MOTION_FLOAT};

/*********************************************************************
 * Type def & enum
*********************************************************************/

typedef union 
{
	struct
	{
		float m00, m01, m02, m03,
		  m10, m11, m12, m13,
		  m20, m21, m22, m23,
		  m30, m31, m32, m33;
	};
	float m[16];
}BKP_Mat4;

/*_____________________________________________________*/

typedef union 
{
	struct
	{
	 	float x,
			  y,
			  z,
			  w;
	};
	struct
	{
	 	float r,
			  g,
			  b,
			  a;
	};
	float v[4];

}BKP_Vec4;
typedef union 
{
	struct
	{
	 	int x,
			  y,
			  z,
			  w;
	};
	struct
	{
	 	int r,
			  g,
			  b,
			  a;
	};
	int v[4];

}BKP_Vec4i;

/*_____________________________________________________*/

typedef union 
{
	struct
	{
	 	float x,
			  y,
			  z;
	};
	struct
	{
	 	float r,
			  g,
			  b;
	};
	struct
	{
	 	float a_,
			  b_,
			  c_;
	};
	float v[3];

}BKP_Vec3;
typedef union 
{
	struct
	{
	 	int x,
			  y,
			  z;
	};
	struct
	{
	 	int r,
			g,
			b;
	};
	struct
	{
	 	int a_,
			b_,
			c_;
	};
	int v[3];

}BKP_Vec3i;

/*_____________________________________________________*/

typedef union 
{
	struct
	{
	 	float x,
			  y;
	};
	struct
	{
	 	float w,
			  h;
	};
	struct
	{
	 	float a_,
			  b_;
	};
	struct
	{
		float x,
			  y;
	}point;
	float v[2];

}BKP_Vec2;

typedef union 
{
	struct
	{
	 	int x,
			y;
	};
	struct
	{
	 	int w,
			h;
	};
	struct
	{
	 	int a_,
		  	b_;
	};
	struct
	{
		int x,
			y;
	}point;
	int v[2];

}BKP_Vec2i;

/*________________________________________________________________________________*/
typedef union BKP_Rect
{
	struct
	{
		float x, y, w, h;
	};
	struct
	{
		float x1, y1, x2, y2;
	};
	struct
	{
		BKP_Vec2 point;
		BKP_Vec2 dimension; 
	};
	struct
	{
		BKP_Vec2 pt1;
		BKP_Vec2 pt2; 
	};
	float v[4];

} BKP_Rect;

typedef union BKP_Recti
{
	struct
	{
		int x, y, w, h;
	};
	struct
	{
		int x1, y1, x2, y2;
	};
	struct
	{
		BKP_Vec2i point;
		BKP_Vec2i dimension; 
	};
	struct
	{
		BKP_Vec2i pt1;
		BKP_Vec2i pt2; 
	};
	int v[4];

} BKP_Recti;

/*_____________________________________________________*/
typedef struct BKP_2Dline
{
	BKP_Vec2 pt1, 			
			 pt2,
			 eq,
			 colinear,
			 normal;
	int isXaxis;
}BKP_2Dline;


/*_____________________________________________________*/
/*_____________________________________________________*/
/*_____________________________________________________*/

/*********************************************************************
 * Macro 
*********************************************************************/

/*********************************************************************
 * Struct 
*********************************************************************/

/*********************************************************************
 * Global 
*********************************************************************/

extern char g_bkp_strdir[4][8];

/*********************************************************************
 * Struct 
*********************************************************************/

/*********************************************************************
 * Functions 
*********************************************************************/

const char * GL_type_to_string(GLenum type);
BKP_Vec2 bkp_vec2(float x, float y);
BKP_Vec3 bkp_vec3(float x, float y, float z);
BKP_Vec4 bkp_vec4(float x, float y, float z, float w);
BKP_Rect bkp_rect(float x, float y, float w, float h);

BKP_Vec2i bkp_vec2i(int x, int y);
BKP_Vec3i bkp_vec3i(int x, int y, int z);
BKP_Vec4i bkp_vec4i(int x, int y, int z, int w);
BKP_Recti bkp_recti(int x, int y, int w, int h);

#endif

