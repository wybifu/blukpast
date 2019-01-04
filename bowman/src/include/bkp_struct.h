#ifndef __BKP_STRUCT_H_
#define __BKP_STRUCT_H_

#include "bkp_def.h"
#include "bkp_numcal.h"
#include "bkp_input.h"

/*********************************************************************
 * Defines 
*********************************************************************/

#define BKP_Color BKP_Vec4

/*********************************************************************
 * Type def & enum
*********************************************************************/

typedef struct BKP_Platform2D BKP_Platform2D;
typedef struct BKP_Dynamics2D BKP_Dynamics2D;
typedef struct BKP_CollisionPts2D BKP_CollisionPts2D;
typedef struct BKP_CollisionLine2D BKP_CollisionLine2D;

/*________________________________________________________________________________*/
typedef struct BKP_AABB
{
	float x,y;
	float w,h;
}BKP_AABB;

/*________________________________________________________________________________*/

/*________________________________________________________________________________*/

/*________________________________________________________________________________*/
typedef struct BKP_Camera
{
	int on;
	int auto_speed;   // if 0.0 center on player, else autoscroll until it become impossible then center on player and block on side
	BKP_Rect dim;		//scrolling box, x1/y1 = leftmargin, x2/y2 = right margin
	BKP_Rect world;
	BKP_Vec2 pos;
	BKP_Vec2 delta;
	BKP_Vec2 wspeed;	//world speed
	BKP_Vec2 speed;	//opengl speed
	BKP_Vec2 speedInitial;
	BKP_Vec2 to;
	BKP_Vec2 diff;
	BKP_Vec2 vslow;
	BKP_Vec2 screen;
	BKP_Vec2 minSpeed;
	BKP_Vec2i ready;

	int dir;
	int func_default;
	int func;

	float marginUp, marginDown;
	float limitleft, limitright;
	float limitup, limitdown;

	struct focus
	{
		float left, right;
		float up, down;
		float panicup, panicdown;
		BKP_Vec2 length;	
		

	}focus;

	

}BKP_Camera;

/*________________________________________________________________________________*/
typedef struct BKP_Box
{
	BKP_Vec4 col;
	float x,y,z;
	float w,h,d;
	float line_w;
	int filled;
} BKP_Box;

/*________________________________________________________________________________*/
typedef struct BKP_Surface2D
{
	BKP_Mat4 points;  
	BKP_Vec4 colors;

	GLfloat vt_[4];

	GLfloat x,y;
	GLfloat w,h;
	GLuint vao;
	GLuint texid;
	GLuint point_vbo;
	GLuint st_offset;

	GLuint texunit_id;

	GLuint loc_texture;
	GLuint loc_color;


	GLuint loc_view;
	GLuint loc_projection;
	GLuint loc_model;

	GLuint loc_alpha;

	GLint alpha;

} BKP_Surface2D;

/*________________________________________________________________________________*/
typedef struct BKP_Graphics_2DGen
{
	BKP_Mat4 points_text;  
	BKP_Mat4 points_rectangle;  
	BKP_Mat4 mtx_projection;  
	BKP_Mat4 mtx_view;  
	BKP_Camera camera;

	GLuint vbo;
	GLuint vao;
	GLuint vao_num;
	GLuint tex_num;

	GLuint sh_box;
	GLuint sh_rect;
	GLuint sh_sprite;
	GLuint sh_sprite_alpha;

	GLuint texunit_id;

	GLuint loc_color_rect;
	GLuint loc_model;
	GLuint loc_projection;
	GLuint loc_view;
	GLuint loc_alpha;

	GLuint color_loc_rect, color_rect;

	// ICI attention , thread not safe

}BKP_Graphics_2DGen; 

/*________________________________________________________________________________*/
struct Character_
{
	BKP_Vec2i Size;
	BKP_Vec2i Bearing;
	GLuint texid;
	BKP_Vec2i Advance;
};

/*________________________________________________________________________________*/
typedef struct BKP_Font
{
	//GLfloat vertices[4][4];
	BKP_Mat4 vertices;
	struct Character_ * tchr;
	GLuint texunit_id;
	GLuint loc_projection;
}BKP_Font;

/*________________________________________________________________________________*/
typedef struct BKP_PhysProp
{
	float sticky;
	float rigidity;
	float slide;
	int allowin;
}BKP_PhysProp;

/*________________________________________________________________________________*/
typedef struct BKP_Rotate
{
	BKP_Vec3 center;
	BKP_Vec3 axis;
	float angle, angle_deg;
	float sin_a, cos_a;
}BKP_Rotate;

/*________________________________________________________________________________*/
typedef struct BKP_CollisionPts2D
{
	BKP_Rect pts;
	BKP_CollisionLine2D * l;
	BKP_Platform2D * p;
	
}BKP_CollisionPts2D;

/*________________________________________________________________________________*/
struct BKP_Dynamics2D
{
	BKP_Rect gbox, pbox; // graphics & physics
	BKP_CollisionPts2D cpts[4]; // fine collision points
	BKP_Vec2 speed, force, dec, maxSpeed;
	BKP_Vec2 climbpts;
	BKP_Vec2 dmaxSpeed;
	BKP_Vec2 stairs;
	BKP_Vec2 half;

	float jmp_force;
	float stairsSpeed;

	int motion,
		dir,
		movement,
		sliding,
		onStairs,
		jumped, maxjump;

};

/*________________________________________________________________________________*/
typedef struct BKP_PlatformInfo
{
	BKP_Vec4i color;
	int face_flag;
	int visible, visible_fill;

}BKP_PlatformInfo;

/*________________________________________________________________________________*/
struct BKP_Platform2D
{
	BKP_Surface2D * spritesheet;
	BKP_Rect spriteInfo;

	BKP_Rotate rotate;
	BKP_Rect bbox;
	BKP_Vec4i color;
	BKP_PhysProp * physprop;
	BKP_Dynamics2D dyn;

	void * membrane;

	float dx,dy;
	int fblock[4];
	int isRotated;
	int contact;
	int id;
	int spriteflip;
	int visible;
	int visible_fill;

};


/*________________________________________________________________________________*/
typedef struct BKP_Entity_Agent
{
	BKP_Surface2D * spritesheet;
	BKP_Input * input;
	BKP_Rect spriteInfo;
	BKP_Dynamics2D dyn;

	float dx,dy;

	int domain, type,//type = class , removing kingdom and phylum between domain and class
		order, family,
		genus, species;
	int name;

	int enable_gravity;
	int enable_jump;
	float gravity_force;
	int flip;

	double a_timer, a_anim;

}BKP_Entity_Agent;

/*________________________________________________________________________________*/
typedef struct BKP_ScreenText
{
	char * text;
	BKP_Vec4 color;
	BKP_Font * font;
	BKP_Vec2 pos;
	BKP_Vec2 scale;

	struct BKP_ScreenText * next;
	struct BKP_ScreenText * prev;
}BKP_ScreenText;

/*________________________________________________________________________________*/
typedef struct BKP_CollisionLine2D
{
	BKP_Rotate rotate;
	BKP_Rect pts;
	BKP_2Dline line;
	BKP_Vec2 dim;
	BKP_Rect bbox;   //bounding box for line
	void * membrane;
	int id;
	int isRotated;
	int nreference;
	int contact;
	int allow_runup;

	BKP_PhysProp * phys;

}BKP_CollisionLine2D;

/*________________________________________________________________________________*/
typedef struct BKP_Collision2DReturn
{
	vec2_t mov; 
	int dirs[BKP_2D_MAX_DIR];
	int collides;
	int inside;
}BKP_Collision2DReturn;

/*________________________________________________________________________________*/
/*________________________________________________________________________________*/

/*********************************************************************
 * Macro 
*********************************************************************/

/*********************************************************************
 * Struct 
*********************************************************************/

/*********************************************************************
 * Global 
*********************************************************************/
//should creat another file for global graph
extern BKP_Color stc_clearColor;

extern BKP_InputMouse Gmouse;

extern unsigned int g_max_color_val;

/*********************************************************************
 * Struct 
*********************************************************************/

/*********************************************************************
 * Functions 
*********************************************************************/
void bkp_resetCollision2DReturn(BKP_Collision2DReturn * result, BKP_Vec2 mov);
BKP_Collision2DReturn bkp_createCollision2DReturn(BKP_Vec2 mov);

#endif

