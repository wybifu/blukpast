/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/bkp_def.h"

/**************************************************************************
*	Defines & Maro
**************************************************************************/

/**************************************************************************
*	Structs, Enum, Unio and Typesdef	
**************************************************************************/

/**************************************************************************
*	Globals	
**************************************************************************/
char g_bkp_strdir[4][8] = {"UP", "LEFT", "DOWN", "RIGHT"};

/***************************************************************************
*	Prototypes 	
**************************************************************************/

/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/

const char * GL_type_to_string(GLenum type)
{
	switch (type)
	{
		case GL_BOOL: return "bool";
		case GL_INT: return "int";
		case GL_FLOAT: return "float";
		case GL_FLOAT_VEC2: return "vec2";
		case GL_FLOAT_VEC3: return "vec3";
		case GL_FLOAT_VEC4: return "vec4";
		case GL_FLOAT_MAT2: return "mat2";
		case GL_FLOAT_MAT3: return "mat3";
		case GL_FLOAT_MAT4: return "mat4";
		case GL_SAMPLER_2D: return "sampler2D";
		case GL_SAMPLER_3D: return "sampler3D";
		case GL_SAMPLER_CUBE: return "samplerCube";
		case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
		default: break;
	}
	return "other";
}

/*--------------------------------------------------------------------------------*/

BKP_Rect bkp_rect(float x, float y, float w, float h)
{
	return (BKP_Rect) { .v[0] = x,  .v[1] = y, .v[2] = w, .v[3] = h };
}

BKP_Recti bkp_recti(int x, int y, int w, int h)
{
	return (BKP_Recti) { .v[0] = x,  .v[1] = y, .v[2] = w, .v[3] = h };
}
/*--------------------------------------------------------------------------------*/

BKP_Vec2 bkp_vec2(float x, float y)
{
	return (BKP_Vec2) { .v[0] = x,  .v[1] = y};
}

BKP_Vec2i bkp_vec2i(int x, int y)
{
	return (BKP_Vec2i) { .v[0] = x,  .v[1] = y};
}


/*--------------------------------------------------------------------------------*/
BKP_Vec3 bkp_vec3(float x, float y, float z )
{
	return (BKP_Vec3) { .v[0] = x,  .v[1] = y, .v[2] = z};
}

BKP_Vec3i bkp_vec3i(int x, int y, int z )
{
	return (BKP_Vec3i) { .v[0] = x,  .v[1] = y, .v[2] = z};
}

/*--------------------------------------------------------------------------------*/

BKP_Vec4 bkp_vec4(float x, float y, float z ,float w)
{
	return (BKP_Vec4) { .v[0] = x,  .v[1] = y, .v[2] = z, .v[3] = w };
}
BKP_Vec4i bkp_vec4i(int x, int y, int z ,int w)
{
	return (BKP_Vec4i) { .v[0] = x,  .v[1] = y, .v[2] = z, .v[3] = w };
}

/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

