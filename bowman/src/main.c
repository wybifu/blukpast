#include <stdlib.h>
#include <stdio.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/blukpast.h"
#include "include/macro.h"
#include "include/graphics.h"

#include "include/helper.h"
#include "include/bkp_physics.h"
#include "include/bkp_collider.h"
#include "include/bkp_platform.h"

/**************************************************************************
*	Defines & Maro
**************************************************************************/

/**************************************************************************
*	Structs, Enum, Unio and Typesdef	
**************************************************************************/

/**************************************************************************
*	Globals	
**************************************************************************/

double _g_previous_seconds;
int _g_frame_count;
double _stc_reloadshader = 0;

/***************************************************************************
*	Prototypes 	
**************************************************************************/

void _update_fps_counter(BKP_ScreenText * fps);
void _update_memUsage(BKP_ScreenText * pfps);
void manage_player(BKP_Entity_Agent * a);
//void setMapCollider(void);
BKP_Rect setMapPlatform(void);
void unsetMapPlatform(void);
void unsetMapCollider(void);
void init_player(BKP_Entity_Agent * a);

void mouse_callback(GLFWwindow * window, double xpos, double ypos);
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);

/**************************************************************************
*	Main	
**************************************************************************/

//BKP_InputMouse Gmouse;

double _g_previous_seconds;
BKP_Entity_Agent * G[3] ;
BKP_ColliderMap * Map;
BKP_PlatformMap * Platforms;
BKP_Vec2i g_scr;
BKP_Vec2 TWH; 			//screen ratio based on game artist TWH.w = sprite.w * screen.w / expected.w same with TWH.h

//------------- Main fucntion ---------------
int main(int argc, char **argv)
{

#ifdef DEBUG
	fprintf(stderr,"DEBUG set\n");
#else
	fprintf(stderr,"DEBUG notset\n");
#endif


	//bkp_input_setMouse(GLFW_CURSOR, GLFW_CURSOR_DISABLED, mouse_callback, scroll_callback); 
	//bkp_setLogInfo("logdir/",BKP_LOGGER_DEBUG, BKP_LOGGER_FILEOUT | BKP_LOGGER_TERMOUT, BKP_LOGGER_OAPPEND) //change default log
	// void bkp_setOpenGLInfo(3, 1)    //minimal version 3.1
	//	bkp_setWindowInfo(1024, 768, BKP_TRUE,"BKP Test");  //fullscreen 1024x768
	bkp_setWindowInfo(0, 0, BKP_FALSE,"BKP Test"); // window mode, auto detect screen resolution 
	bkp_setWindowInfo(0, 0, BKP_TRUE,"BKP Test"); // window mode, auto detect screen resolution 
	
	if(bkp_startEngine(argv) != BKP_TRUE)
		return EXIT_FAILURE;

	bkp_input_Init();

	g_scr = bkp_graphics_getWindowDimensions();
	TWH = bkp_graphics_getTWH(1920, 1080);

	bkp_input_setKeyPauseTimer(.05);

	G[0] = (BKP_Entity_Agent *) bkp_memory_getTicket(sizeof(BKP_Entity_Agent));
	G[1] = (BKP_Entity_Agent *) bkp_memory_getTicket(sizeof(BKP_Entity_Agent));

  	G[0]->spritesheet = bkp_graphics_2DloadSurface("data/graphics/girl_run.png");
  	G[1]->spritesheet = bkp_graphics_2DloadSurface("data/graphics/platform.png");
	init_player(G[0]);

	//setMapCollider();
	BKP_Rect world = setMapPlatform();

	bkp_graphics_2DinitCamera(bkp_rect(g_scr.w / 4, g_scr.h * 2 / 5, 0.0f, 0.0f), world, BKP_CAMERAFUNC_SMOOTHSTD);
	bkp_graphics_camera2DsetFunc(BKP_CAMERAFUNC_FOCUS);
//	bkp_graphics_camera2DsetFunc(BKP_CAMERAFUNC_FIXEDSETFOCUS);
	bkp_graphics_2DsetCameraPanic(150,50);

	/*
	float sx = g_scr.w  / 3,
		  sy = g_scr.h / 3;
	bkp_graphics_2DinitCamera(bkp_rect(g_scr.w / 2 - sx / 2, g_scr.h /2 - sy /2,sx,sy),world, BKP_CAMERAFUNC_LOCKON);
	bkp_graphics_camera2DsetFunc(BKP_CAMERAFUNC_FOCUS);
	bkp_graphics_2DtoggleCamera();
	*/

	BKP_Font * myfont;

	myfont =  bkp_graphics_newFont("data/fonts/DejaVuSans.ttf", 64, 128);

	BKP_ScreenText * fps = bkp_graphics_appendTextPool("do not care", 64, myfont, bkp_vec2(20 * TWH.w,64 * TWH.h), bkp_vec2(.3 * TWH.w,.3 * TWH.h), bkp_vec4(0.98f,0.98f,0.2f,1.0f));
	BKP_ScreenText * mem = bkp_graphics_appendTextPool("can't see me", 128, myfont, bkp_vec2(20 * TWH.w,96 * TWH.h), bkp_vec2(.3 * TWH.w,.3 * TWH.h), bkp_vec4(1.0f,1.0f,1.0f,1.0f));


	fprintf(stderr,"player at %.4f\n",G[0]->dyn.gbox.x);

	while(G[0]->input->Cancel == 0)
	{
		bkp_input_capture(G[0]->input);

		manage_player(G[0]);
		bkp_graphics_camera2D(G[0]->dyn);

		Ugp_draw();
		_update_fps_counter( fps);
		_update_memUsage(mem);

	}

	bkp_graphics_releaseTextPool(fps);
	bkp_graphics_releaseTextPool(mem);

	bkp_graphics_freeFont(myfont);
	unsetMapCollider();
	unsetMapPlatform();
	bkp_memory_releaseTicket(G[0]->input);
	bkp_memory_releaseTicket(G[0]->spritesheet);
	bkp_memory_releaseTicket(G[1]->spritesheet);
	bkp_memory_releaseTicket(G[0]);
	bkp_memory_releaseTicket(G[1]);

	bkp_logger_write(BKP_LOGGER_INFO,"\t________________________________\n");
	bkp_logger_write(BKP_LOGGER_INFO,"\t*POOLS* Allocated : %.1f Kb\n",(float)bkp_memory_allocated() / 1024);
	bkp_logger_write(BKP_LOGGER_INFO,"\t*POOLS* Used : %.1f Kb\n",(float)bkp_memory_usage() / 1024);
	bkp_logger_write(BKP_LOGGER_INFO,"\t*POOLS* Free : %.1f Kb\n",(float)bkp_memory_free() / 1024);

	bkp_stopEngine();

	return EXIT_SUCCESS;
}

/**************************************************************************
*	Implementations	
**************************************************************************/
/*--------------------------------------------------------------------------------*/
void manage_player(BKP_Entity_Agent * a)
{
	//BKP_Collision2DReturn ret = bkp_collider_2Dcollision(&a->dyn, 

	BKP_Collision2DReturn ret = bkp_platform_2Dcollision(&a->dyn, 
			Platforms, 
			bkp_vec2(a->dyn.speed.x, a->dyn.speed.y));

	if(BKP_TRUE == ret.collides)
		a->spritesheet->colors = bkp_vec4(0xff, 0xff, 0x0, 0xc0);
	else
		a->spritesheet->colors = bkp_vec4(0x0, 0xff, 0xff, 0xc0);

	a->dyn.gbox.x +=  ret.mov.x;
	a->dyn.gbox.y +=  ret.mov.y;


	a->dyn.movement = BKP_FALSE;

	
	if(!bkp_platform_stillOnPlatform2D(&a->dyn))
		if(a->enable_gravity == BKP_TRUE)
			a->dyn.speed.y += a->dyn.force.y;


	double ticks = glfwGetTime();
	if((a->input->left.pressed > 0) || (a->input->right.pressed > 0))
	{
		float dx;

		a->dyn.movement = BKP_TRUE;

		if(a->input->left.pressed > 0)
		{
			a->dyn.cpts[BKP_RIGHT].p = NULL; 

			if(a->dyn.speed.x > 0)
				a->dyn.speed.x = 0.0f;
			a->flip = BKP_GRAPHICS_FLIPH;
			dx = -a->dyn.force.x;
			a->dyn.dir = BKP_LEFT;
		}
		else
		{
			a->dyn.cpts[BKP_LEFT].p = NULL;
			if(a->dyn.speed.x < 0)
				a->dyn.speed.x = 0.0f;
			a->flip = BKP_GRAPHICS_FLIPNONE;
			dx = a->dyn.force.x;
			a->dyn.dir = BKP_RIGHT;
		}

		if(BKP_MOTION_FALL == a->dyn.motion)
		{
			a->dyn.speed.x += dx;
		}
		else
		{
			BKP_Vec2 acc = bkp_platform_horizontalMove(&a->dyn);
			a->dyn.speed.x += acc.x;
			a->dyn.gbox.y += acc.y;

			if(ticks - a->a_timer > a->a_anim)
			{
				a->spriteInfo.x += 1;
				a->a_timer = ticks;
				if(a->spriteInfo.x >= 16)
					a->spriteInfo.x = 0;
			}
		}
		//bkp_input_setKeyPause(&a->input->right);
	}

	static int flag_0001 = 0;
	if(a->input->Action0.pressed > 0)
	{ flag_0001 = 1;

		if(a->dyn.jumped < a->dyn.maxjump && a->input->Action0.pressed < 2)
		{
			a->dyn.speed.y = -a->dyn.jmp_force;
		}
	}
	else if(flag_0001)
	{ flag_0001 = 0;

		++a->dyn.jumped;
	}

	/* 
	if(a->dyn.onStairs == BKP_TRUE)
		bkp_platform_2DonStairs(&a->dyn, a->jmp_force); a modifier pour escalader
		*/

	if(a->dyn.speed.x > a->dyn.maxSpeed.x) a->dyn.speed.x = a->dyn.maxSpeed.x;
	if(a->dyn.speed.x < -a->dyn.maxSpeed.x) a->dyn.speed.x = -a->dyn.maxSpeed.x;
	if(a->dyn.speed.y > a->dyn.maxSpeed.y) a->dyn.speed.y = a->dyn.maxSpeed.y;
	if(a->dyn.speed.y < -a->dyn.maxSpeed.y) a->dyn.speed.y = -a->dyn.maxSpeed.y;


	if(a->dyn.movement == BKP_FALSE && 0 != a->dyn.speed.x)
	{
		
		a->dyn.speed.x = 0.0f;
	//	a->dyn.cpts[BKP_LEFT].l = a->dyn.cpts[BKP_RIGHT].l = NULL;
		if(a->dyn.speed.x < 0) a->dyn.speed.x += a->dyn.dec.x;
		if(a->dyn.speed.x > 0) a->dyn.speed.x -= a->dyn.dec.x;

	//	if(a->dyn.speed.x < 0) a->dyn.speed.x += bkp_platform_horizontalStop(a->dyn);
	//	if(a->dyn.speed.x > 0) a->dyn.speed.x -= bkp_platform_horizontalStop(a->dyn);
		
		if(a->dyn.speed.x > 0 && a->dyn.speed.x <= a->dyn.dec.x) a->dyn.speed.x = 0;
		if(a->dyn.speed.x < 0 && a->dyn.speed.x >= -a->dyn.dec.x) a->dyn.speed.x = 0;
	}

	return;
}
/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
void init_player(BKP_Entity_Agent * a)
{

	a->input = bkp_input_New();
	a->a_anim = 0.030;
	a->a_timer = glfwGetTime();
	a->flip = BKP_GRAPHICS_FLIPNONE;

	a->spriteInfo = bkp_rect(0, 0, 400, 600);

	a->dyn.gbox = bkp_rect(0 * TWH.w, -2000 * TWH.h, 300 * TWH.w, 450 * TWH.h);
	//a->dyn.gbox = bkp_rect(-300 * TWH.w, 0 * TWH.h, 300 * TWH.w, 450 * TWH.h);
	a->dyn.pbox = bkp_rect(25 * TWH.w , 20 * TWH.h, a->dyn.gbox.w - (25 * TWH.w), a->dyn.gbox.h - (20 * TWH.h));

	//a->dyn.force = bkp_vec2(.20f, .50f);
	a->dyn.dir = BKP_RIGHT;
	a->dyn.force = bkp_vec2(1.0f * TWH.w, (BKP_GLOBAL_GRAVITY_FORCE )  * TWH.h);
	a->dyn.dec = bkp_vec2(.80f, .50f);
	a->dyn.speed = bkp_vec2(.0f , .0f);
	a->dyn.maxSpeed = bkp_vec2( 16.0f * TWH.w, 20.0f * TWH.h);
	a->dyn.onStairs = BKP_FALSE;
	a->dyn.dmaxSpeed = a->dyn.maxSpeed;
	a->dyn.motion = BKP_MOTION_FALL;
	a->dyn.stairsSpeed = a->dyn.maxSpeed.x * 2 / 6 * TWH.w;
	a->dyn.maxjump = 1;

	a->dyn.jumped = a->dyn.maxjump;
	a->dyn.jmp_force = 14.0f * TWH.h;
	a->enable_gravity = BKP_TRUE;
	//a->enable_gravity = BKP_FALSE;

	a->dyn.cpts[BKP_UP].pts = bkp_rect( a->dyn.gbox.w * 104 / a->spriteInfo.w,  a->dyn.gbox.h * 105 / a->spriteInfo.h,
		 a->dyn.gbox.w * 295 /a->spriteInfo.w,  a->dyn.gbox.h * 105 / a->spriteInfo.h);
	a->dyn.cpts[BKP_UP].l = NULL;
	a->dyn.cpts[BKP_UP].p = NULL;

	a->dyn.cpts[BKP_LEFT].pts = bkp_rect( a->dyn.gbox.w * 100 /a->spriteInfo.w,  a->dyn.gbox.h * 100 / a->spriteInfo.h,
			a->dyn.gbox.w * 100 /a->spriteInfo.w,  a->dyn.gbox.h * 550 / a->spriteInfo.h);
	a->dyn.cpts[BKP_LEFT].l = NULL;
	a->dyn.cpts[BKP_LEFT].p = NULL;
	
	a->dyn.cpts[BKP_DOWN].pts = bkp_rect( a->dyn.gbox.w * 104 /a->spriteInfo.w, a->dyn.gbox.h * 556 / a->spriteInfo.h,
			a->dyn.gbox.w * 295 /a->spriteInfo.w,  a->dyn.gbox.h * 556 / a->spriteInfo.h);
	a->dyn.cpts[BKP_DOWN].l = NULL;
	a->dyn.cpts[BKP_DOWN].p = NULL;

	a->dyn.cpts[BKP_RIGHT].pts = bkp_rect( a->dyn.gbox.w * 300 /a->spriteInfo.w,  a->dyn.gbox.h * 100 / a->spriteInfo.h,
			a->dyn.gbox.w * 300 /a->spriteInfo.w,  a->dyn.gbox.h * 550 / a->spriteInfo.h);
	a->dyn.cpts[BKP_RIGHT].l = NULL;
	a->dyn.cpts[BKP_RIGHT].p = NULL;
	
	float h = a->dyn.cpts[BKP_DOWN].pts.y2 - a->dyn.cpts[BKP_UP].pts.y1, 
		  w = a->dyn.cpts[BKP_RIGHT].pts.x1 - a->dyn.cpts[BKP_LEFT].pts.x1;

	a->dyn.half = bkp_vec2(w / 2, h / 2);

	a->dyn.climbpts = bkp_vec2(a->dyn.cpts[BKP_LEFT].pts.y1 + h / 4,a->dyn.cpts[BKP_LEFT].pts.y2 - h  / 5);
	
	return;
}

/*--------------------------------------------------------------------------------*/
void setMapCollider(void)
{
	Map = NULL;
	return;
	//float y = (float) g_scr.h - 200 * TWH.h;
	float y = (float) g_scr.h - 20 * TWH.h;
	float x = -1000.0f * TWH.w;

	Map = bkp_collider_2DcreateMap();
	bkp_collider_2DMap_lpush(Map, bkp_rect(x, y - 20, 8192 * TWH.w , y - 20),bkp_vec3(x,y,0),NULL);
	bkp_collider_2DMap_lpush(Map, bkp_rect(-950 * TWH.w, 0,  -950 * TWH.w, y),bkp_vec3(0,0,0),NULL);
	bkp_collider_2DMap_lpush(Map, bkp_rect(8100 * TWH.w, 0,  8100 * TWH.w, y),bkp_vec3(0,0,0),NULL);

	BKP_Rect world = bkp_collider_getWorldBound2D(Map);
	fprintf(stdout,"%.2f, %.2f, %.2f, %.2f\n",world.x1, world.y1, world.x2, world.y2);

	/*
	float sx = g_scr.w  / 3,
		  sy = g_scr.h / 3;

	//bkp_graphics_2DsetScroller(bkp_rect(g_scr.w / 2 - sx / 2,g_scr.h /2 - sy /2,sx,sy),world, BKP_FALSE, bkp_vec2(-1.0f, -1.0f));
	bkp_graphics_2DsetBasicScroller(bkp_rect(g_scr.w / 2 - sx / 2, g_scr.h /2 - sy /2,sx,sy),world, BKP_FALSE, bkp_vec2(-1.0f, -1.0f));
	//bkp_graphics_2DtoggleScroller();
	*/


	return;
}

/*--------------------------------------------------------------------------------*/
BKP_Rect setMapPlatform(void)
{
	float y = (float) g_scr.h - 70 * TWH.h;
	//float x = -1000.0f * TWH.w;
	int w_limit_up 	 	= -1400 * TWH.h,
//		w_limit_down 	= y + 200 * TWH.h,
		w_limit_left	= -1000 * TWH.w,
		w_limit_right	= 8192 * TWH.w ;

	Platforms = bkp_plateform_2DcreateMap();

	BKP_PlatformInfo pinfo = {};
	pinfo.color = bkp_vec4i(0x0, 0x00, 0x80, 0xff);
	pinfo.face_flag = 0xf;
	pinfo.visible_fill = BKP_TRUE;
	
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(200 * TWH.w, w_limit_up, 8192 * TWH.w, 100 * TWH.h ), bkp_vec3(0,0,0),pinfo,NULL);
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(w_limit_left, y, 8192 * TWH.w, 100 * TWH.h ), bkp_vec3(0,0,0),pinfo,NULL);
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(w_limit_left, w_limit_up , 10 * TWH.w, abs(y - w_limit_up)), bkp_vec3(0,0,0),pinfo,NULL);
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(w_limit_right + w_limit_left, w_limit_up, 10 * TWH.w, abs(y - w_limit_up)), bkp_vec3(0,0,0),pinfo,NULL);

//just to be very high
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(w_limit_left, -3000 * TWH.h, 100 * TWH.w, 60 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);

	y -= 200 * TWH.h;
	pinfo.visible_fill = BKP_TRUE;
	pinfo.color = bkp_vec4i(0x0, 0x50, 0x10, 0xff);

	bkp_platform_2DMap_lpush(Platforms, bkp_rect(w_limit_left, y, 100 * TWH.w, 60 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);

	bkp_platform_2DMap_lpush(Platforms, bkp_rect(500 * TWH.w, y, 1000 * TWH.w, 40 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y -= 200 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(1700 * TWH.w, y, 1000 * TWH.w, 40 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y -= 200 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(800 * TWH.w, y, 600 * TWH.w, 40 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y -= 230 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(400 * TWH.w, y, 300 * TWH.w, 40 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y -= 200 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(920 * TWH.w, y, 100 * TWH.w, 40 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y -= 100 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(1100 * TWH.w, y, 200 * TWH.w, 40 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y -= 140 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(1300 * TWH.w, y, 100 * TWH.w, 40 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y -= 340 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(1400 * TWH.w, y, 2000 * TWH.w, 200 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);

	bkp_platform_2DMap_lpush(Platforms, bkp_rect(600 * TWH.w, y, 400 * TWH.w, 100 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);

/*stairs*/
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(3400 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(3600 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(3800 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(4000 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(4200 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(4400 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(4600 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(4800 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(5000 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);

	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(5200 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(5400 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(5600 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(5800 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(6000 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y += 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(6200 * TWH.w, y, 300 * TWH.w, 20 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);

	y = (float) g_scr.h - 300 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(6800 * TWH.w, y, 400 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y -= 250 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(6400 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y -= 250 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(6900 * TWH.w, y, 300 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);

/*mini stairs*/
	y = (float) g_scr.h - 100 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(4000 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y -= 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(4200 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);
	y -= 50 * TWH.h;
	bkp_platform_2DMap_lpush(Platforms, bkp_rect(4400 * TWH.w, y, 200 * TWH.w, 50 * TWH.h), bkp_vec3(0,0,0),pinfo,NULL);


	return bkp_platform_get2DWoldBound(Platforms,bkp_vec2(20.0f * TWH.w, 40 * TWH.h));

	/*basic scrooler
	float sx = g_scr.w  / 3,
		  sy = g_scr.h / 3;
	bkp_graphics_2DsetBasicScroller(bkp_rect(g_scr.w / 2 - sx / 2, g_scr.h /2 - sy /2,sx,sy),world, BKP_FALSE, bkp_vec2(-1.0f, -1.0f));
	*/
}

/*--------------------------------------------------------------------------------*/
void unsetMapPlatform(void)
{
	if(Platforms == NULL)
		return;

	bkp_platform_2DdestroyMap(Platforms);
	Platforms = NULL;

	return;
}

/*--------------------------------------------------------------------------------*/
void unsetMapCollider()
{
	if(Map == NULL)
		return;

	bkp_collider_2DdestroyMap(Map);
	Map = NULL;

	return;
}

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
void _update_fps_counter(BKP_ScreenText * pfps)
{
	double current_seconds;
	double elapsed_seconds;

	current_seconds = glfwGetTime();
	elapsed_seconds = current_seconds - _g_previous_seconds;

	if(0.25 < elapsed_seconds)
	{
		_g_previous_seconds = current_seconds;
		double fps = (double) _g_frame_count / elapsed_seconds;
		sprintf(pfps->text,"fps : %.4f, Frame Time : %.5f", fps, 1 / fps);
		_g_frame_count = 0;
	}
	++_g_frame_count;

	return;
}

/*--------------------------------------------------------------------------------*/
void _update_memUsage(BKP_ScreenText * stext)
{
	ssize_t s = bkp_memory_using();
	sprintf(stext->text,"memory pool : %.2fMb | %ldKb |%ld bytes", (float)(s/ 1024) / 1024, s / 1024, s);
	return;
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	/*
	if(FOV >= 1.0f && FOV <= MAX_FOV)
		FOV -= yoffset;
	
	if(FOV < 1.0f)
		FOV = 1.0f;
	else if(FOV > MAX_FOV)
		FOV = MAX_FOV;
	*/
	fprintf(stderr,"scrolling\n");
	return;
}

/*--------------------------------------------------------------------------------*/
void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
	//Gmouse.pos.x = xpos;
	//Gmouse.pos.y = ypos;

	/*
	if( BKP_TRUE == firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = BKP_FALSE;
	}

	float xoffset = xpos - lastX,
		  yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;
	*/
//	fprintf(stderr,"%d,%d\n", (int)Gmouse.pos.x, (int)Gmouse.pos.y);
	
	return;
}
/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

	
