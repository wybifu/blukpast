#ifndef __BKP_NUMCAL_H_
#define __BKP_NUMCAL_H_

#include "bkp_def.h"
#include "math.h"

/*********************************************************************
 * Defines 
*********************************************************************/

#define BKP_NUMCAL_PI 3.14159265358979323846
#define BKP_NUMCAL_DEG2RAD(value)  (value)  /  180.0 * BKP_NUMCAL_PI  

/*********************************************************************
 * Type def & enum
*********************************************************************/

#define vec2_t BKP_Vec2 
#define vec3_t BKP_Vec3 
#define vec4_t BKP_Vec4
#define mat4_t BKP_Mat4  

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
mat4_t bkp_numcal_mat4(float m00, float m10, float m20, float m30,
		float m01, float m11, float m21, float m31,
		float m02, float m12, float m22, float m32,
		float m03, float m13, float m23, float m33);

//operation sur matrice
vec2_t bkp_numcal_vec2(float x, float y);
vec3_t bkp_numcal_vec3(float x, float y, float z);
vec4_t bkp_numcal_vec4(float x, float y, float z ,float w);

mat4_t bkp_numcal_setIdentity(void);
vec3_t bkp_numcal_v3add(vec3_t A, vec3_t B) ;
vec3_t bkp_numcal_v3adds(vec3_t A,float s);
vec3_t bkp_numcal_v3sub(vec3_t A, vec3_t B) ;
vec3_t bkp_numcal_v3subs(vec3_t A, float s);
vec3_t bkp_numcal_v3mul(vec3_t A, vec3_t B) ;
vec3_t bkp_numcal_v3scalar(vec3_t A, float s);
vec3_t bkp_numcal_v3div(vec3_t A, vec3_t B) ;
vec3_t bkp_numcal_v3divs(vec3_t A, float s);
vec3_t bkp_numcal_v3norm(vec3_t v);
vec3_t bkp_numcal_v3proj(vec3_t v, vec3_t onto);
vec3_t bkp_numcal_v3cross(vec3_t A, vec3_t B);

float bkp_numcal_v3length(vec3_t v);	
float bkp_numcal_v3dot(vec3_t A,vec3_t B)	;
float bkp_numcal_v3angle(vec3_t A, vec3_t b);

mat4_t bkp_numcal_mult4(mat4_t A, mat4_t B);

//transformation sur model
mat4_t bkp_numcal_setRotateX(float angle);
mat4_t bkp_numcal_setRotateY(float angle);
mat4_t bkp_numcal_setRotateZ(float angle);
mat4_t bkp_numcal_setRotate(float angle, vec3_t axis);
mat4_t bkp_numcal_rotate(mat4_t m, float angle, vec3_t axis);
mat4_t bkp_numcal_setTranslate(vec3_t v);
mat4_t bkp_numcal_setScale(vec3_t v);

mat4_t bkp_numcal_rotateX(mat4_t m, float angle);
mat4_t bkp_numcal_rotateY(mat4_t m, float angle);
mat4_t bkp_numcal_rotateZ(mat4_t m, float angle);
mat4_t bkp_numcal_translate(mat4_t m, vec3_t v);
mat4_t bkp_numcal_scale(mat4_t m, vec3_t v);

//view model
mat4_t bkp_numcal_lookAt(vec3_t from, vec3_t to, vec3_t up);
mat4_t bkp_numcal_ortho(float left, float right, float bottom, float top, float back, float front);
mat4_t bkp_numcal_perspective(float v_angle, float ratio, float near, float far);

//affichage
void bkp_numcal_print(const char * name, mat4_t m, int n);
void bkp_numcal_printV4(const char * title, vec4_t m);
void bkp_numcal_printV3(const char * title, vec3_t m);

//calcule d'extrema de vecteur
float bkp_numcal_min2f(float a, float b);
float bkp_numcal_max2f(float a, float b);
float bkp_numcal_minvec2(vec2_t v);
float bkp_numcal_maxvec2(vec2_t v);
float bkp_numcal_minvec3(vec3_t v);
float bkp_numcal_maxvec3(vec3_t v);
float bkp_numcal_minvec4(vec4_t v);
float bkp_numcal_maxvec4(vec4_t v);

//produit de matrices par vecteur 
vec4_t bkp_numcal_mat4vec2(mat4_t m, vec2_t v);
vec4_t bkp_numcal_mat4vec3(mat4_t m, vec3_t v);
vec4_t bkp_numcal_mat4vec4(mat4_t m, vec4_t v);


#endif

