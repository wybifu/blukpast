#include <stdlib.h>
#include <stdio.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/bkp_numcal.h"

/**************************************************************************
*	Globals	
**************************************************************************/

/***************************************************************************
*	Prototypes 	
**************************************************************************/

/**************************************************************************
*	Implementations	
**************************************************************************/

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_setIdentity()
{
	return bkp_numcal_mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);
}

/*--------------------------------------------------------------------------------*/
void bkp_numcal_printV4(const char * title, vec4_t m)
{
	if(title != NULL)
		fprintf(stderr,"\n\t---%s---\n[", title);
	else
		fprintf(stderr,"\n\t---Vector---\n[");

	fprintf(stderr," %.4f  %.4f %.4f %.4f]\n",m.x, m.y, m.z, m.w);
	fprintf(stderr,"\t -------------------\n");
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_numcal_printV3(const char * title, vec3_t m)
{
	if(title != NULL)
		fprintf(stderr,"\n\t---%s---\n[", title);
	else
		fprintf(stderr,"\n\t---Vector---\n[");

	fprintf(stderr," %.4f  %.4f %.4f]\n",m.x, m.y, m.z);
	fprintf(stderr,"\t -------------------\n");
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_numcal_print(const char * title, mat4_t m, int dim)
{
	int i,j;

	if(title != NULL)
		fprintf(stderr,"\n\t---%s---\n[", title);
	else
		fprintf(stderr,"\n\t---Matrix---\n[");

	for(i = 0; i < dim; ++i)
	{
		for(j = 0; j < dim; ++j)
			fprintf(stderr," %c%.4f  ",(i>0 && j == 0) ? ' ' :'\0', m.m[i * dim +j]);
		if(i >= dim -1)
			fprintf(stderr,"]\n");
		else
			fprintf(stderr,"\n");
	}
	fprintf(stderr,"\t -------------------\n");
	return;
}


/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_mult4(mat4_t A, mat4_t B)
{
	mat4_t R;
	R.m[0] = A.m[0]  * B.m[0] + A.m[1] * B.m[4] + A.m[2] * B.m[8] + A.m[3] * B.m[12];
	R.m[1] = A.m[0]  * B.m[1] + A.m[1] * B.m[5] + A.m[2] * B.m[9] + A.m[3] * B.m[13];
	R.m[2] = A.m[0]  * B.m[2] + A.m[1] * B.m[6] + A.m[2] * B.m[10] + A.m[3] * B.m[14];
	R.m[3] = A.m[0]  * B.m[3] + A.m[1] * B.m[7] + A.m[2] * B.m[11] + A.m[3] * B.m[15];

	R.m[4] = A.m[4]  * B.m[0] + A.m[5] * B.m[4] + A.m[6] * B.m[8] + A.m[7] * B.m[12];
	R.m[5] = A.m[4]  * B.m[1] + A.m[5] * B.m[5] + A.m[6] * B.m[9] + A.m[7] * B.m[13];
	R.m[6] = A.m[4]  * B.m[2] + A.m[5] * B.m[6] + A.m[6] * B.m[10] + A.m[7] * B.m[14];
	R.m[7] = A.m[4]  * B.m[3] + A.m[5] * B.m[7] + A.m[6] * B.m[11] + A.m[7] * B.m[15];
	
	R.m[8] = A.m[8]  * B.m[0] + A.m[9] * B.m[4] + A.m[10] * B.m[8] + A.m[11] * B.m[12];
	R.m[9] = A.m[8]  * B.m[1] + A.m[9] * B.m[5] + A.m[10] * B.m[9] + A.m[11] * B.m[13];
	R.m[10] = A.m[8]  * B.m[2] + A.m[9] * B.m[6] + A.m[10] * B.m[10] + A.m[11] * B.m[14];
	R.m[11] = A.m[8]  * B.m[3] + A.m[9] * B.m[7] + A.m[10] * B.m[11] + A.m[11] * B.m[15];

	R.m[12] = A.m[12]  * B.m[0] + A.m[13] * B.m[4] + A.m[14] * B.m[8] + A.m[15] * B.m[12];
	R.m[13] = A.m[12]  * B.m[1] + A.m[13] * B.m[5] + A.m[14] * B.m[9] + A.m[15] * B.m[13];
	R.m[14] = A.m[12]  * B.m[2] + A.m[13] * B.m[6] + A.m[14] * B.m[10] + A.m[15] * B.m[14];
	R.m[15] = A.m[12]  * B.m[3] + A.m[13] * B.m[7] + A.m[14] * B.m[11] + A.m[15] * B.m[15];

	return R;
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_ortho(float left, float right, float bottom, float top, float near, float far)
{
	float rl = 1.0f / (right - left),
		  tb = 1.0f / (top - bottom),
		  fn = -1.0f / (far - near);

	return bkp_numcal_mat4(
			2 * rl, 0	  , 0			 , 0,
			0     , 2 * tb, 0			 , 0,
			0		 , 0  		, 2 * fn , 0,
			-(right + left) * rl,-(top + bottom) * tb,(far + near) * fn,		 1
			);

	/*
	return bkp_numcal_mat4(
			2 * rl, 0	  , 0			 , -(right + left) * rl,
			0     , 2 * tb, 0			 , -(top + bottom) * tb,
			0		 , 0  		, 2 * fn , (far + near) * fn,
			0		 , 0		, 0			, 1
			);
			*/
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_perspective(float v_angle, float ratio, float near, float far)
{
	float fovy = v_angle ;
	float f = 1.0f / tanf(fovy / 2.0f);
	float r = ratio;
	float nd = near;
	float fd = far;

	return bkp_numcal_mat4(
			   f / r, 0,                   		 0,  0,
				 0  , f, 				 		 0,  0,
				 0  , 0, (fd + nd) / (nd -fd)     , -1,
				 0  , 0, (2 * fd * nd) / (nd -fd) ,  0
			);

	return bkp_numcal_mat4(
			f / r, 0,                    0,                        0,
				 0, f, 0, 0,
				 0, 0, (fd + nd) / (nd -fd), (2 * fd * nd) / (nd -fd),
				 0, 0, 					 -1, 0
			);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_lookAt(vec3_t from, vec3_t to, vec3_t up)
{
	vec3_t z = bkp_numcal_v3norm(bkp_numcal_v3sub(to, from));
	vec3_t x = bkp_numcal_v3norm(bkp_numcal_v3cross(z,up));
	vec3_t y = bkp_numcal_v3cross(x,z);

	return bkp_numcal_mat4(
			x.x				, y.x			  , -z.x				, 0,
			x.y				, y.y			  , -z.y				, 0,
			x.z				, y.z			  , -z.z				, 0,
			-bkp_numcal_v3dot(x, from), -bkp_numcal_v3dot(y, from), bkp_numcal_v3dot(z, from), 1
			);

	return bkp_numcal_mat4(
			x.x, x.y, x.z, -bkp_numcal_v3dot(from, x),
			y.x, y.y, y.z, -bkp_numcal_v3dot(from, y),
			z.x, z.y, z.z, -bkp_numcal_v3dot(from, z),
			0  , 0  , 0  , 1
			);
}

/*--------------------------------------------------------------------------------*/
vec3_t bkp_numcal_v3add(vec3_t A, vec3_t B) {return bkp_numcal_vec3(A.x + B.x, A.y + B.y, A.z + B.z);}
/*--------------------------------------------------------------------------------*/
vec3_t bkp_numcal_v3adds(vec3_t A, float s) {return bkp_numcal_vec3(A.x + s, A.y + s, A.z + s);}
/*--------------------------------------------------------------------------------*/
vec3_t bkp_numcal_v3sub(vec3_t A, vec3_t B) {return bkp_numcal_vec3(A.x - B.x, A.y - B.y, A.z - B.z);}
/*--------------------------------------------------------------------------------*/
vec3_t bkp_numcal_v3subs(vec3_t A, float s) {return bkp_numcal_vec3(A.x - s, A.y - s, A.z - s);}
/*--------------------------------------------------------------------------------*/
vec3_t bkp_numcal_v3mul(vec3_t A, vec3_t B) {return bkp_numcal_vec3(A.x * B.x, A.y * B.y, A.z * B.z);}
/*--------------------------------------------------------------------------------*/
vec3_t bkp_numcal_v3scalar(vec3_t A, float s) {return bkp_numcal_vec3(A.x * s, A.y * s, A.z * s);}
/*--------------------------------------------------------------------------------*/
vec3_t bkp_numcal_v3div(vec3_t A, vec3_t B) {return bkp_numcal_vec3(A.x / B.x, A.y / B.y, A.z / B.z);}
/*--------------------------------------------------------------------------------*/
vec3_t bkp_numcal_v3divs(vec3_t A, float s) {return bkp_numcal_vec3(A.x / s, A.y / s, A.z / s);}
/*--------------------------------------------------------------------------------*/
float bkp_numcal_v3length(vec3_t v)	{return sqrtf(v.x*v.x + v.y*v.y + v.z * v.z);}
/*--------------------------------------------------------------------------------*/
float bkp_numcal_v3dot(vec3_t A,vec3_t B)	{return A.x * B.x + A.y * B.y + A.z * B.z;}
/*--------------------------------------------------------------------------------*/
float bkp_numcal_v3angle(vec3_t A, vec3_t B)
{
	return acosf(bkp_numcal_v3dot(A,B) / (bkp_numcal_v3length(A) * bkp_numcal_v3length(B)));
}
/*--------------------------------------------------------------------------------*/
vec3_t bkp_numcal_v3norm(vec3_t v)
{
	float len = bkp_numcal_v3length(v);
	//fprintf(stderr,"len new = %.3f old = %.3f\n" ,len, sqrtf(0 + 0 + 1));

	if (len > 0)
		return bkp_numcal_vec3(v.x/len, v.y/len, v.z/len);
	else
		return bkp_numcal_vec3(0,0,0);
}

/*--------------------------------------------------------------------------------*/
vec3_t bkp_numcal_v3proj(vec3_t v, vec3_t onto)
{
	return bkp_numcal_v3scalar(onto, bkp_numcal_v3dot(v,onto) / bkp_numcal_v3dot(onto,onto));
}

/*--------------------------------------------------------------------------------*/
vec3_t bkp_numcal_v3cross(vec3_t A, vec3_t B)
{
	return bkp_numcal_vec3(
		A.y * B.z - A.z * B.y,
		A.z * B.x - A.x * B.z,
		A.x * B.y - A.y * B.x
		);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_setRotateX(float angle)
{
	float s = sinf(angle),
		  c = cosf(angle);

	return bkp_numcal_mat4(
			1,  0, 0, 0,
			0,  c,-s, 0,
			0,  s, c, 0,
			0,  0, 0, 1
		);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_rotateX(mat4_t m,float angle)
{
	mat4_t r = bkp_numcal_setRotateX(angle);
	return bkp_numcal_mult4(m,r);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_setRotateY(float angle)
{
	float s = sinf(angle),
		  c = cosf(angle);

	return bkp_numcal_mat4(
			 c,  0,  s, 0,
		 	 0,  1,  0, 0,
			-s,  0,  c, 0,
			 0,  0,  0, 1
		);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_rotateY(mat4_t m,float angle)
{
	mat4_t r = bkp_numcal_setRotateY(angle);
	return bkp_numcal_mult4(m,r);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_setRotateZ(float angle)
{
	float s = sinf(angle),
		  c = cosf(angle);

	return bkp_numcal_mat4(
			 c, -s, 0, 0,
			 s,  c, 0, 0,
			 0,  0, 1, 0,
			 0,  0, 0, 1
		);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_rotateZ(mat4_t m,float angle)
{
	mat4_t r = bkp_numcal_setRotateZ(angle);
	return bkp_numcal_mult4(m,r);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_setRotate(float angle, vec3_t axis)
{
	vec3_t normalized_axis = bkp_numcal_v3norm(axis);
	float x = normalized_axis.x, y = normalized_axis.y, z = normalized_axis.z;
	float c = cosf(angle), s = sinf(angle);

	return bkp_numcal_mat4(
			c + x*x*(1 - c)  , y*x*(1 - c) + z*s , z*x*(1 - c) - y*s , 0.0f,
			x*y*(1 - c) - z*s, c + y*y*(1 - c)   , z*y*(1 - c) + x*s , 0.0f,
			x*z*(1 - c) + y*s, y*z*(1 - c) - x*s , c + z*z*(1 - c)   , 0.0f,
				 		 0.0f,				 0.0f,			     0.0f, 1.0f
			);
}

/*--------------------------------------------------------------------------------*/

mat4_t bkp_numcal_setRotateT(float angle, vec3_t axis)
{
	vec3_t normalized_axis = bkp_numcal_v3norm(axis);
	float x = normalized_axis.x, y = normalized_axis.y, z = normalized_axis.z;
	float c = cosf(angle), s = sinf(angle);

	return bkp_numcal_mat4(
			c + x*x*(1 - c)  , x*y*(1 - c) - z*s, x*z*(1 - c) + y*s, 0,
			y*x*(1 - c) + z*s, c + y*y*(1 - c),   y*z*(1 - c) - x*s, 0,
			z*x*(1 - c) - y*s, z*y*(1 - c) + x*s, c + z*z*(1 - c),	 0,
							0,				   0,				0,	 1
			);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_rotate(mat4_t m, float angle, vec3_t axis)
{
	mat4_t r = bkp_numcal_setRotate(angle, axis);
	return bkp_numcal_mult4(m,r);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_rotateT(mat4_t m, float angle, vec3_t axis)
{
	mat4_t r = bkp_numcal_setRotateT(angle, axis);
	return bkp_numcal_mult4(r,m);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_setTranslate(vec3_t v)
{
	return bkp_numcal_mat4(
			1.0f  , 0.0f  , 0.0f  , 0.0f,
			0.0f  , 1.0f  , 0.0f  , 0.0f,
			0.0f  , 0.0f  , 1.0f  , 0.0f,
			v.x, v.y, v.z, 1.0f 
			);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_translate(mat4_t m, vec3_t v)
{
	mat4_t t = bkp_numcal_setTranslate(v);
	return bkp_numcal_mult4(m,t);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_setScale(vec3_t v)
{
	return bkp_numcal_mat4(
			v.x, 0, 0, 0,
			0, v.y, 0, 0,
			0, 0, v.z, 0,
			0, 0, 0, 1
			);
}

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_scale(mat4_t m, vec3_t v)
{
	mat4_t s = bkp_numcal_setScale(v);
	return bkp_numcal_mult4(m,s);
}

/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
mat4_t bkp_numcal_mat4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
{
	return (mat4_t)
	{
		.m[0]  = m00, .m[1]  = m01, .m[2]  = m02, .m[3]  = m03,
		.m[4]  = m10, .m[5]  = m11, .m[6]  = m12, .m[7]  = m13,
		.m[8]  = m20, .m[9]  = m21, .m[10] = m22, .m[11] = m23,
		.m[12] = m30, .m[13] = m31, .m[14] = m32, .m[15] = m33,
	};
}

/*--------------------------------------------------------------------------------*/
vec2_t bkp_numcal_vec2(float x, float y)
{
	return (vec2_t) { .v[0] = x,  .v[1] = y};
}

/*--------------------------------------------------------------------------------*/

vec3_t bkp_numcal_vec3(float x, float y, float z )
{
	return (vec3_t) { .v[0] = x,  .v[1] = y, .v[2] = z};
}

/*--------------------------------------------------------------------------------*/

vec4_t bkp_numcal_vec4(float x, float y, float z ,float w)
{
	return (vec4_t) { .v[0] = x,  .v[1] = y, .v[2] = z, .v[3] = w };
}

/*--------------------------------------------------------------------------------*/
float bkp_numcal_min2f(float a, float b)
{
	return (a > b) ? b : a;
}
/*--------------------------------------------------------------------------------*/
float bkp_numcal_max2f(float a, float b)
{
	return (a > b) ? a : b;
}
/*--------------------------------------------------------------------------------*/
float bkp_numcal_minvec2(vec2_t v)
{
	return (v.v[0] < v.v[1]) ? v.v[0] : v.v[1];
}
/*--------------------------------------------------------------------------------*/

float bkp_numcal_maxvec2(vec2_t v)
{
	return (v.v[0] < v.v[1]) ? v.v[1] : v.v[0];
}

/*--------------------------------------------------------------------------------*/
float bkp_numcal_minvec3(vec3_t v)
{
	float min = v.v[0];
	int i;

	for(i = 1; i < 3; ++i)
		if(v.v[i] < min) min = v.v[i];

	return min;
}
/*--------------------------------------------------------------------------------*/
float bkp_numcal_maxvec3(vec3_t v)
{
	float max = v.v[0];
	int i;

	for(i = 1; i < 3; ++i)
		if(v.v[i] > max) max = v.v[i];

	return max;
}

/*--------------------------------------------------------------------------------*/
float bkp_numcal_minvec4(vec4_t v)
{
	float min = v.v[0];
	int i;

	for(i = 1; i < 4; ++i)
		if(v.v[i] < min) min = v.v[i];

	return min;
}

float bkp_numcal_maxvec4(vec4_t v)
{
	float max = v.v[0];
	int i;

	for(i = 1; i < 4; ++i)
		if(v.v[i] > max) max = v.v[i];

	return max;
}

/*--------------------------------------------------------------------------------*/
vec4_t bkp_numcal_mat4vec2(mat4_t m, vec2_t v)
{
	vec4_t r;
	r.v[0] = m.m[0]  * v.v[0] + m.m[1]  * v.v[1];
	r.v[1] = m.m[4]  * v.v[0] + m.m[5]  * v.v[1];
	r.v[2] = m.m[8]  * v.v[0] + m.m[9]  * v.v[1];
	r.v[3] = m.m[12] * v.v[0] + m.m[13] * v.v[1];

	/*
	r.v[0] /= r.v[3];
	r.v[1] /= r.v[3];
	r.v[2] /= r.v[3];
	*/

	return r;
}

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
