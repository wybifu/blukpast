#include <stdio.h>

#include <ft2build.h> 
#include FT_FREETYPE_H

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/bkp_shader.h"
#include "include/bkp_graphics.h"
#include "include/bkp_logger.h"
#include "include/_windowing.h"
#include "include/macro.h"
#include "include/bkp_image.h"
#include "include/gl_helper.h"
#include "include/bkp_memory.h"

/**************************************************************************
*	Defines & Maro
**************************************************************************/


/**************************************************************************
*	Structs, Enum, Unio and Typesdef	
**************************************************************************/

struct TexterGen
{
	GLuint loc_projection;
	GLuint vao, vbo;	
	GLuint sh_text;
	FT_Library ft;
};

/*________________________________________________________________________________*/
struct TextBuffer
{
	BKP_ScreenText * head;
	ssize_t count;
};


/**************************************************************************
*	Globals	
**************************************************************************/

BKP_Color stc_clearColor;
unsigned int g_max_color_val = 255;
int stc_gEngine = BKP_GRAPHICS_2D_ENGINE;

static GLFWwindow * stc_Window = NULL;
static BKP_Graphics_2DGen * stc_2D = NULL; 
static int stc_default_line_text_length = 32;

static int	stc_fb_width ;
static int	stc_fb_height ;
static struct TexterGen * stc_ScreenText = NULL;
static struct TextBuffer * stc_Btext = NULL;

/***************************************************************************
*	Prototypes 	
**************************************************************************/

static int  bkp_graphics_2DInit(void);
static void _shutdown_2D(void);
static void _init_onScreenText(void);
static void _close_onScreenText(void);

/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/

GLFWwindow * bkp_graphics_getWindow(void) { return stc_Window; }

/*--------------------------------------------------------------------------------*/
BKP_Vec2i bkp_graphics_getWindowDimensions(void) { return bkp_vec2i(stc_fb_width, stc_fb_height); }

/*--------------------------------------------------------------------------------*/
BKP_Vec2 bkp_graphics_getTWH(int w, int h) { return bkp_vec2((float)stc_fb_width / w, (float)stc_fb_height / h); }

/*--------------------------------------------------------------------------------*/
int bkp_graphics_init(int w, int h, int full,const char *sztitle, int oglV, int oglv)
{
	
	if(hw_init_all(oglV,oglv) != BKP_TRUE)
		return BKP_FALSE;

	stc_Window = hw_openwindow(&w,&h,sztitle,full);
	stc_fb_width = w;
	stc_fb_height = h;

	if(NULL == stc_Window)
	{
		bkp_logger_write(BKP_LOGGER_FATAL,"Could not open stc_Window with GLFW3!\n");
		glfwTerminate();
		return BKP_FALSE;
	}

	glfwSetWindowSizeCallback(stc_Window, hw_glfw_window_size_callback);
	glfwSetFramebufferSizeCallback(stc_Window, hw_glfw_framebuffer_size_callback);

	glfwMakeContextCurrent(stc_Window);
	glewExperimental = GL_TRUE;
	glewInit();

	hw_log_gl_params();
	bkp_logger_write(BKP_LOGGER_INFO,"Opening window with GLFW3 [OK]\n");

	const GLubyte * renderer = glGetString(GL_RENDERER);
	const GLubyte * version = glGetString(GL_VERSION);

	bkp_logger_write(BKP_LOGGER_INFO,"Renderer : %s\n",renderer);
	bkp_logger_write(BKP_LOGGER_INFO,"OpenGL version : %s\n",version);

	int VM;
	int Vm;
	glGetIntegerv(GL_MAJOR_VERSION,&VM);
	glGetIntegerv(GL_MINOR_VERSION,&Vm);
	bkp_logger_write(BKP_LOGGER_INFO,"OpenGL version: %d.%d\n",VM,Vm);

	if(VM < oglV || (VM == oglV && Vm < oglv))
	{
		bkp_logger_write(BKP_LOGGER_FATAL,"Bad OpenGL version required >= %d.%d, Your version : %d.%d\n",oglV,oglv,VM,Vm);
		glfwTerminate();
		return BKP_FALSE;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	if(stc_gEngine == BKP_GRAPHICS_2D_ENGINE)
	{
		if(bkp_graphics_2DInit() == BKP_FALSE)
		{
			BKP_DIE_L("2D Graphics Engine Initializing [Failed]\n",0xFF);
		}
		bkp_logger_write(BKP_LOGGER_INFO,"2D Graphics Engine start [OK]\n");
	}
	else
	{ BKP_DIE_L("2D Graphics Engine supported yet [Failed]\n",0xFF);
	}

	_init_onScreenText();

	return BKP_TRUE;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_close()
{
	if(stc_ScreenText != NULL)
	{
		_close_onScreenText();
		bkp_logger_write(BKP_LOGGER_INFO,"On screen Texter closed [OK]\n");
	}
	if(stc_gEngine == BKP_GRAPHICS_2D_ENGINE)
	{
		_shutdown_2D();
		bkp_logger_write(BKP_LOGGER_INFO,"2D Graphics  Engine stopped [OK]\n");
	}

	glfwTerminate();
	bkp_logger_write(BKP_LOGGER_INFO,"Window closed [OK]\n");

	return;
}


/*--------------------------------------------------------------------------------*/
void bkp_graphics_drawSurface(BKP_Surface2D * s,BKP_Rect * dest,BKP_Rect * src,BKP_Rotate *R,int flip , int * alpha)
{
	glUseProgram(stc_2D->sh_sprite);
	glActiveTexture(GL_TEXTURE0 + s->texunit_id);
	glBindTexture(GL_TEXTURE_2D, s->texid);

	if(alpha != NULL)
	{
		float alpha_factor = (*alpha) / (float)g_max_color_val ;
		glUniform1f(s->loc_alpha, alpha_factor);
	}

	glUniform1i(s->loc_texture, s->texunit_id);

	float xdest, ydest, wdest, hdest;
	float xfcoef = 1.0f,
		  yfcoef = 1.0f;

	if(src != NULL)
	{
		s->vt_[0] = (float) (src->point.x * src->dimension.w) / s->w;
		s->vt_[1] = (float) src->dimension.w / s->w;
		s->vt_[2] = (s->h - (float) ((1 + src->point.y) * src->dimension.h)) / s->h;
		s->vt_[3] = (float) src->dimension.h / s->h;
	}
	else
	{
		s->vt_[0] = 0.0f;
		s->vt_[1] = 1.0f;
		s->vt_[2] = 0.0f;
		s->vt_[3] = 1.0f;
	}

	if(dest != NULL)
	{
		xdest = dest->point.x;
		ydest = dest->point.y;
		wdest = dest->dimension.w;
		hdest = dest->dimension.h;
	}
	else
	{
		xdest = 0;
		ydest = 0;
		wdest = s->w;
		hdest = s->h;
	}

	if(flip == BKP_GRAPHICS_FLIPH || flip == BKP_GRAPHICS_FLIPHV)
	{
		xdest = xdest + wdest ;
		xfcoef = -1.0f;
	}
	if(flip == BKP_GRAPHICS_FLIPV || flip == BKP_GRAPHICS_FLIPHV)
	{
		ydest = ydest + hdest;
		yfcoef = -1.0f;
	}


	mat4_t model = bkp_numcal_setIdentity();
	model = bkp_numcal_scale(model, bkp_numcal_vec3( xfcoef * wdest,  yfcoef * hdest, 1.0f));
	if( NULL != R)
	{
		float cx =  (float)(xdest - R->center.x);
		float cy =  (float)(ydest - R->center.y);

		model = bkp_numcal_translate(model, bkp_numcal_vec3(cx, cy ,0.0f));
		model = bkp_numcal_rotate(model, R->angle, bkp_numcal_vec3(0.0f, 0.0f, 1.0f));
		model = bkp_numcal_translate(model, bkp_numcal_vec3(-cx ,-cy ,0.0f));
	}
	model = bkp_numcal_translate(model, bkp_numcal_vec3(xdest ,ydest ,0.0f));


	glUniformMatrix4fv(s->loc_view, 1, GL_FALSE, stc_2D->mtx_view.m);
	glUniformMatrix4fv(s->loc_model, 1, GL_FALSE, model.m);
	glUniform4fv(s->st_offset,1,s->vt_);


	glBindVertexArray(s->vao);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	if(alpha != NULL)
	{
		float alpha_factor = 1.0f ;
		glUniform1f(s->loc_alpha, alpha_factor);
	}
	/*

	if(flip != BKP_GRAPHICS_FLIPNONE)
	{
		glDisable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_FRONT);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
	}
	else
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	*/

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_drawRectangle(BKP_Rect * dest,BKP_Rotate * R, int filled)
{
	glUseProgram(stc_2D->sh_rect);

	BKP_Mat4 model = bkp_numcal_setIdentity();

	model = bkp_numcal_scale(model, bkp_numcal_vec3(dest->dimension.w,  dest->dimension.h, 1.0f));

	if( NULL != R)
	{
		float cx =  (float)(dest->point.x - R->center.x);
		float cy =  (float)(dest->point.y - R->center.y);

		model = bkp_numcal_translate(model, bkp_numcal_vec3(cx, cy ,0.0f));
		model = bkp_numcal_rotate(model, R->angle, R->axis);
		model = bkp_numcal_translate(model, bkp_numcal_vec3(-cx ,-cy ,0.0f));
	}
	model = bkp_numcal_translate(model, bkp_numcal_vec3(dest->point.x,  dest->point.y, 0.0f));

	glUniformMatrix4fv(stc_2D->loc_model, 1, GL_FALSE, model.m);
	glUniformMatrix4fv(stc_2D->loc_view, 1, GL_FALSE, stc_2D->mtx_view.m);

	glBindVertexArray(stc_2D->vao);
	if(filled == BKP_TRUE)
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	else
		glDrawArrays(GL_LINE_LOOP, 0, 4);
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_setSurfaceColori(BKP_Surface2D *s, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
{
	float r = (float)red / g_max_color_val;
	float g = (float)green / g_max_color_val;
	float b = (float)blue / g_max_color_val;
	float a = (float)alpha / g_max_color_val;

	glUseProgram(stc_2D->sh_sprite);
	glUniform4f(s->loc_color, r, g, b, a);

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_setSurfaceColorf(BKP_Surface2D * s, float r, float g, float b, float a)
{
	glUseProgram(stc_2D->sh_sprite);
	glUniform4f(s->loc_color, r, g, b, a);

	return;
}


/*--------------------------------------------------------------------------------*/
void bkp_graphics_2DsetColori(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
{
	float r = (float)red / g_max_color_val;
	float g = (float)green / g_max_color_val;
	float b = (float)blue / g_max_color_val;
	float a = (float)alpha / g_max_color_val;

	glUseProgram(stc_2D->sh_rect);
	glUniform4f(stc_2D->loc_color_rect, r, g, b, a);

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_setColorvi(BKP_Vec4i co)
{
	float r = (float)co.r/ g_max_color_val;
	float g = (float)co.g/ g_max_color_val;
	float b = (float)co.b/ g_max_color_val;
	float a = (float)co.a/ g_max_color_val;

	glUseProgram(stc_2D->sh_rect);
	glUniform4f(stc_2D->loc_color_rect, r, g, b, a);
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_2DsetColorf(float r, float g, float b, float a)
{
	glUseProgram(stc_2D->sh_rect);
	glUniform4f(stc_2D->loc_color_rect, r, g, b, a);

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_setColorvf(BKP_Vec4 co)
{
	glUseProgram(stc_2D->sh_rect);
	glUniform4f(stc_2D->loc_color_rect, co.r, co.g, co.b, co.a);
	return;
}


/*--------------------------------------------------------------------------------*/
void bkp_graphics_ClearColori(unsigned int r, unsigned int g, unsigned int b, unsigned int a)
{
	stc_clearColor.r = (float)r / g_max_color_val;
	stc_clearColor.g = (float)g / g_max_color_val;
	stc_clearColor.b = (float)b / g_max_color_val;
	stc_clearColor.a = (float)a / g_max_color_val;
	glClearColor(stc_clearColor.r, stc_clearColor.g, stc_clearColor.b, stc_clearColor.a);
	
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_setClearColorf(float r, float g, float b, float a)
{
	stc_clearColor.r = r;
	stc_clearColor.g = g;
	stc_clearColor.b = b;
	stc_clearColor.a = a;

	return;
}

/*--------------------------------------------------------------------------------*/
BKP_Surface2D * bkp_graphics_2DloadSurface(const char * file)
{
	unsigned  int width, height;
	unsigned char * image = bkp_image_loadPNG(file,&width,&height);

	if(image == NULL)
	{
		bkp_logger_write(BKP_LOGGER_WARNING,"Image '%s' Not loaded!\n",file);
		return NULL;
	}
	if((width & (width - 1)) != 0 || (height & (height - 1) )!= 0)
		bkp_logger_write(BKP_LOGGER_WARNING,"Image dimension not power of 2!\n");

	bkp_image_vflip(image, width, height);

	BKP_Surface2D * s = NULL;
	s = (BKP_Surface2D *) bkp_memory_getTicket(sizeof(BKP_Surface2D));

	s->points = bkp_numcal_mat4(
		0.0f,  1.0f,   0.0f,  0.0f,
		0.0f,  0.0f,   0.0f,  1.0f,
		1.0f,  0.0f,   1.0f,  1.0f,
		1.0f,  1.0f,   1.0f,  0.0f);

	s->w = (GLfloat) width;
	s->h = (GLfloat) height;
	s->x = 0;
	s->y = 0;
	
	GLuint point_vbo = 0;
	glGenBuffers(1,&point_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s->points.m),s->points.m,GL_STATIC_DRAW);

	GLuint vao = stc_2D->vao_num;
	stc_2D->vao_num += 1;

	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	s->vao = vao;

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)0);

	s->texunit_id =  stc_2D->texunit_id;
	stc_2D->texunit_id += 1;

	GLuint tex = stc_2D->tex_num;
	++stc_2D->tex_num;
	glGenTextures(1, &tex);

    glUseProgram(stc_2D->sh_sprite);

	s->loc_texture = glGetUniformLocation(stc_2D->sh_sprite, "sprite");
	s->loc_color = glGetUniformLocation(stc_2D->sh_sprite, "spriteColor");
	s->loc_alpha = glGetUniformLocation(stc_2D->sh_sprite, "alpha_factor");
	s->loc_projection = glGetUniformLocation(stc_2D->sh_sprite, "projection");
	s->loc_view = glGetUniformLocation(stc_2D->sh_sprite, "view");
	s->loc_model = glGetUniformLocation(stc_2D->sh_sprite, "model");
	glUniformMatrix4fv(s->loc_projection, 1, GL_FALSE, stc_2D->mtx_projection.m);
	glUniformMatrix4fv(s->loc_view, 1, GL_FALSE, stc_2D->mtx_view.m);
	glUniform1f(s->loc_alpha, 1.0f);

	glActiveTexture(GL_TEXTURE0 + s->texunit_id);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0,
			GL_RGBA, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	free(image);

	int st_offset_loc = glGetUniformLocation(stc_2D->sh_sprite, "vt_");
	s->st_offset = st_offset_loc;

	s->vt_[0] = 0.0;
	s->vt_[1] = 0.0;
	s->vt_[2] = 0.0;
	s->vt_[3] = 0.0;

	s->texid = tex;
	s->point_vbo = point_vbo;

	bkp_graphics_setSurfaceColori(s, 0xff, 0xff, 0xff, 0xff);

	return s;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_2DreleaseSurface(BKP_Surface2D *s)
{
	glDeleteTextures(1,&s->texid);
	glDeleteBuffers(1,&s->point_vbo);
	glDeleteVertexArrays(1,&s->vao);
	bkp_memory_releaseTicket(s);
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_2DReady(int clear)
{
	if(clear == BKP_TRUE)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0,0,stc_fb_width,stc_fb_height);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_2DFlush()
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glfwSwapBuffers(stc_Window);
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_2DTranslateView(BKP_Vec2 v)
{
	stc_2D->mtx_view = bkp_numcal_setTranslate(bkp_vec3(v.x,v.y,0.0f));
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_2DTranslateViewAuto(BKP_Vec2 v)
{
	stc_2D->mtx_view = bkp_numcal_translate(stc_2D->mtx_view, bkp_numcal_vec3(v.x, v.y ,0.0f));
	return;
}

/*--------------------------------------------------------------------------------*/
int bkp_graphics_getCameraFunc(void) { return stc_2D->camera.func; }

/*--------------------------------------------------------------------------------*/
void bkp_graphics_2DinitCamera(BKP_Rect dim, BKP_Rect world,int camera_function)
{
	stc_2D->camera.func_default 	= camera_function;
	stc_2D->camera.func				= camera_function;

	stc_2D->camera.speedInitial = bkp_vec2(CAMERA_SPEED_INITX, CAMERA_SPEED_INITY);
	stc_2D->camera.on = BKP_TRUE;
	stc_2D->camera.dim = dim;
	stc_2D->camera.pos = bkp_vec2(0.0f, 0.0f);
	stc_2D->camera.to	 = bkp_vec2(0.0f, 0.0f);
	stc_2D->camera.world = world;
	stc_2D->camera.ready = bkp_vec2i(BKP_FALSE, BKP_FALSE);
	stc_2D->camera.minSpeed = bkp_vec2(CAMERA_MIN_SPEEDX, CAMERA_MIN_SPEEDY);

	stc_2D->camera.focus.left 	= 0.0f;
	stc_2D->camera.focus.right 	= 300.0f;
	stc_2D->camera.focus.up	=  0.0f;
	stc_2D->camera.focus.down	=  0.0f;
	stc_2D->camera.focus.length = bkp_vec2(dim.x * 2 / 3, dim.y / 5);
	stc_2D->camera.marginUp =  CAMERA_PANIC_UP;
	stc_2D->camera.marginDown =  stc_fb_height - CAMERA_PANIC_DOWN;

	stc_2D->camera.screen = bkp_vec2(stc_fb_width, stc_fb_height);
	stc_2D->camera.dir = BKP_DOWN;
	stc_2D->camera.vslow = bkp_vec2(dim.x / 2, 0.0f);

	stc_2D->camera.limitleft = world.x1;
	stc_2D->camera.limitup = world.y1;
	stc_2D->camera.limitright= world.x2 - stc_fb_width;
	stc_2D->camera.limitdown = world.y2 - stc_fb_height;

	stc_2D->camera.focus.panicup 	= CAMERA_PANIC_UP;
	stc_2D->camera.focus.panicdown	= stc_fb_height - CAMERA_PANIC_DOWN;

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_2DtoggleCamera()
{
	stc_2D->camera.on = !stc_2D->camera.on ;
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_2DsetCameraDim(BKP_Rect dim) { stc_2D->camera.dim = dim; return; }
void bkp_graphics_2DsetCameraWorld( BKP_Rect world) { stc_2D->camera.world = world; return; }
void bkp_graphics_2DsetCameraAuto( float auto_speed) { stc_2D->camera.auto_speed = auto_speed; return; }
/*--------------------------------------------------------------------------------*/
void bkp_graphics_2DsetCameraSpeed(BKP_Vec2 speed) 
{ 
	stc_2D->camera.wspeed = speed;
	BKP_Vec4 a = bkp_numcal_mat4vec2(stc_2D->mtx_projection, stc_2D->camera.wspeed);
	stc_2D->camera.speed = bkp_vec2(a.x, a.y);
	return; 
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_2DsetCameraPanic(float up, float down)
{
	stc_2D->camera.marginUp = up;
	stc_2D->camera.marginDown = stc_fb_height - down;
}

BKP_Rect bkp_graphics_2DgetCameraDim() { return stc_2D->camera.dim; }
/*--------------------------------------------------------------------------------*/
BKP_Vec2 bkp_graphics_2DgetCameraPos() { return stc_2D->camera.pos; }

/*--------------------------------------------------------------------------------*/
BKP_Rect bkp_graphics_2DgetCameraWorld(void){return stc_2D->camera.world;}

/*--------------------------------------------------------------------------------*/
BKP_Rect bkp_graphics_2DgetCamerafocus()
{ 
	BKP_Rect r ;
	r.pt1 = bkp_vec2(stc_2D->camera.focus.left,stc_2D->camera.focus.up);
	r.pt2 = bkp_vec2(stc_2D->camera.focus.right,stc_2D->camera.focus.down);
	return r;
}

/*--------------------------------------------------------------------------------*/
BKP_Vec2 bkp_graphics_2DgetCamerafocusLength(void){return stc_2D->camera.focus.length;}

/*--------------------------------------------------------------------------------*/
BKP_Vec2 bkp_graphics_2DgetCamerapanic(void){return bkp_vec2(stc_2D->camera.focus.panicup, stc_2D->camera.focus.panicdown);}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_camera2DsetFunc(int function)
{
	stc_2D->camera.func = function;
	return;
}

/*--------------------------------------------------------------------------------*/
int bkp_graphics_camera2D(BKP_Dynamics2D P)
{
	int ret = camera2D(&stc_2D->camera, P);
	if(BKP_TRUE == ret)
	{
		bkp_graphics_2DsetCameraSpeed(stc_2D->camera.delta);
		bkp_graphics_2DTranslateView(stc_2D->camera.speed); // manual
	}
	return ret;
}

/*--------------------------------------------------------------------------------*/
BKP_Font * bkp_graphics_newFont(const char * path, int size, int encoding)
{
	if(stc_ScreenText == NULL)
	{
		bkp_logger_write(BKP_LOGGER_ERROR,"FreetType: not initialized cannot creat BKP_Font\n");
		return NULL;
	}

	BKP_Font * font = (BKP_Font *) bkp_memory_getTicket(sizeof(BKP_Font));
	font->tchr = (struct Character_ *) bkp_memory_getTicket(sizeof(struct Character_) * 128);

	font->texunit_id =  stc_2D->texunit_id;
	stc_2D->texunit_id += 1;

	FT_Face face;
	//if(FT_New_Face(ft, "data/fonts/DejaVuSans.ttf", 0, &face))
	if(FT_New_Face(stc_ScreenText->ft, path, 0, &face))
		bkp_logger_write(BKP_LOGGER_ERROR,"FreetType: Could not load font '%s'\n", path);

	FT_Set_Pixel_Sizes(face, 0, size);

	glActiveTexture(GL_TEXTURE0 + font->texunit_id);

	for(GLubyte c = 0; c < 128; ++c)
	{
		if(FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			bkp_logger_write(BKP_LOGGER_ERROR,"FreetType: Failed to load Glyph '%d'\n",c);
			continue;
		}

		/*
		GLuint texture = stc_2D->tex_num;
		++stc_2D->tex_num;
		*/
		GLuint texture;

		glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
				GL_TEXTURE_2D, 0, GL_RED, 
				face->glyph->bitmap.width, 
				face->glyph->bitmap.rows,
				0, GL_RED, GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
				);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		
		font->tchr[c] = (struct Character_) { 
			bkp_vec2i(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			bkp_vec2i(face->glyph->bitmap_left, face->glyph->bitmap_top),
			texture,
			bkp_vec2i(face->glyph->advance.x,face->glyph->advance.y) };
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);

	//stc_2D->loc_view = glGetUniformLocation(stc_2D->sh_rect, "view");
	stc_ScreenText->loc_projection = glGetUniformLocation(stc_ScreenText->sh_text, "projection");
	glUseProgram(stc_ScreenText->sh_text);
	glUniformMatrix4fv(stc_ScreenText->loc_projection, 1, GL_FALSE, stc_2D->mtx_projection.m);

	return font;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_renderText(const char * str, BKP_Font * font, BKP_Vec2 pos, BKP_Vec2 scale, BKP_Vec4 color)
{
	glUseProgram(stc_ScreenText->sh_text);
	glUniform4f(glGetUniformLocation(stc_ScreenText->sh_text, "textColor"), color.r, color.g, color.b, color.a);
	//glActiveTexture(GL_TEXTURE0 + font->texunit_id);
	glActiveTexture(GL_TEXTURE0);
	
	glBindVertexArray(stc_ScreenText->vao);

	GLfloat x = pos.x;

//	glCullFace(GL_BACK);
//	glFrontFace(GL_CW);

	const char *p = str;
	for(;*p != '\0'; ++p)
	{
		struct Character_ chr = font->tchr[(int)*p];

		GLfloat xpos = x + chr.Bearing.x * scale.x;
		GLfloat ypos = pos.y - chr.Bearing.y * scale.y;

		GLfloat w = chr.Size.x * scale.x;
		GLfloat h = chr.Size.y * scale.y;

		font->vertices = bkp_numcal_mat4(
             xpos,     ypos + h,   0.0, 1.0 ,
             xpos,     ypos,       0.0, 0.0 ,
             xpos + w, ypos,       1.0, 0.0 ,
             xpos + w, ypos + h,   1.0, 1.0 );


		glBindTexture(GL_TEXTURE_2D, chr.texid);
		glBindBuffer(GL_ARRAY_BUFFER, stc_ScreenText->vbo);

		//glUniform1i(glGetUniformLocation(stc_ScreenText->sh_text, "text"),chr.texid);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(font->vertices), font->vertices.m);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		x += (chr.Advance.x >> 6) * scale.x;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_freeFont(BKP_Font * font)
{
	for(GLubyte c = 0; c < 128; ++c)
		glDeleteTextures(1,&font->tchr[c].texid);
	bkp_memory_releaseTicket(font->tchr);
	bkp_memory_releaseTicket(font);

	return;
}

/*--------------------------------------------------------------------------------*/
BKP_ScreenText * bkp_graphics_appendTextPool(const char * str, size_t ss, BKP_Font * font, BKP_Vec2 pos, BKP_Vec2 scale, BKP_Vec4 color)
{
	BKP_ScreenText * p = (BKP_ScreenText*) bkp_memory_getTicket(sizeof(BKP_ScreenText));

	if(p == NULL)
		return NULL;

	if(ss < 1)
		ss = stc_default_line_text_length;

	char * pstr = (char *) bkp_memory_getTicket(sizeof(char) * ss); 
	int c = 0;

	for(; *str ; ++c,++str) pstr[c] = *str;
	pstr[c] = 0;

	p->text = pstr;
	p->font = font;
	p->pos = pos;
	p->scale = scale;
	p->color = color;
	p->next = p->prev = NULL;

	if(NULL != stc_Btext->head)
	{
		p->next = stc_Btext->head;
		stc_Btext->head->prev = p;
	}

	stc_Btext->head = p;
	stc_Btext->count += 1;

	return p;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_releaseTextPool(BKP_ScreenText * p)
{
	if(NULL == p)
		return;

	if(p->next != NULL)
		p->next->prev = p->prev;
	if(p->prev != NULL)
		p->prev->next = p->next;

	bkp_memory_releaseTicket(p->text);
	bkp_memory_releaseTicket(p);
	stc_Btext->count -=1;

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_graphics_OnScreentext()
{
	if(NULL == stc_Btext->head) return;

	BKP_ScreenText * ps = stc_Btext->head;			// if text not initialize will segfault. Warning

	for(; NULL != ps; ps = ps->next)
		bkp_graphics_renderText(ps->text, ps->font, ps->pos, ps->scale, ps->color);

	return;
}

/******************** static functions ********************************/
/*--------------------------------------------------------------------------------*/
static int  bkp_graphics_2DInit()
{
	/*
	glFrontFace(GL_CCW);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	*/

	
	/*ICI COMMENTER/UNCOMMENT  to see square around sprite */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	stc_2D = (BKP_Graphics_2DGen *) bkp_memory_getTicket(sizeof(BKP_Graphics_2DGen));

	stc_2D->points_rectangle = bkp_numcal_mat4(
			0.0f,  1.0f,   0.0f,  1.0f,
			0.0f,  0.0f,   0.0f,  1.0f,
			1.0f,  0.0f,   0.0f,  1.0f,
			1.0f,  1.0f,   0.0f,  1.0f);
	
	stc_2D->texunit_id = 0;
	stc_2D->vbo = 0;


	glGenBuffers(1,&stc_2D->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, stc_2D->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(stc_2D->points_rectangle.m), stc_2D->points_rectangle.m, GL_STATIC_DRAW);
	
	stc_2D->vao = 0;
	stc_2D->vao_num = 1;
	stc_2D->tex_num= 0;
	glGenVertexArrays(1,&stc_2D->vao);
	glBindVertexArray(stc_2D->vao);


	glBindBuffer(GL_ARRAY_BUFFER, stc_2D->vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,NULL);

	stc_2D->mtx_projection = bkp_numcal_ortho(0.0f, (float) stc_fb_width, (float) stc_fb_height, 0.0f, -1.0f, 1.0f); 
	//stc_2D->mtx_projection = bkp_numcal_ortho(0.0f, (float) stc_fb_width, 0.0f, (float) stc_fb_height, -1.0f, 1.0f); 
	stc_2D->mtx_view	   = bkp_numcal_setIdentity(); 

	stc_2D->sh_rect = bkp_shader_createFromFile("data/shaders/rect.vert","data/shaders/rect.frag");
	glBindAttribLocation(stc_2D->sh_rect, 0, "vertices");
#ifdef DEBUG 
	bkp_shader_printInfo(stc_2D->sh_rect);
#endif


	glUseProgram(stc_2D->sh_rect);
	stc_2D->loc_view = glGetUniformLocation(stc_2D->sh_rect, "view");
	stc_2D->loc_projection = glGetUniformLocation(stc_2D->sh_rect, "projection");
	stc_2D->loc_model = glGetUniformLocation(stc_2D->sh_rect, "model");
	stc_2D->loc_color_rect = glGetUniformLocation(stc_2D->sh_rect, "rect_color");
	glUniformMatrix4fv(stc_2D->loc_projection, 1, GL_FALSE, stc_2D->mtx_projection.m);
	glUniformMatrix4fv(stc_2D->loc_view , 1, GL_FALSE, stc_2D->mtx_view.m);

	bkp_graphics_ClearColori(0x21, 0x21, 0x21, 0xFF);
	bkp_graphics_2DsetColori(0xff,0xff,0xff,0xff);

	//stc_2D->sh_sprite = bkp_shader_create(sprite_vert,sprite_frag,NULL");
	stc_2D->sh_sprite = bkp_shader_createFromFile("data/shaders/sprite.vert","data/shaders/sprite.frag");
	glBindAttribLocation(stc_2D->sh_sprite, 0, "vertices");

	stc_2D->sh_sprite_alpha = BKP_FALSE;
#ifdef DEBUG 
	bkp_shader_printInfo(stc_2D->sh_sprite);
#endif

	//default camera 30% of the screen

	return BKP_TRUE;
}

/*--------------------------------------------------------------------------------*/
static void _shutdown_2D()
{
	if(GL_TRUE == glIsProgram(stc_2D->sh_rect))
	{
		bkp_logger_write(BKP_LOGGER_INFO,"deleting shader programe %d\n",stc_2D->sh_rect);
		glDeleteProgram(stc_2D->sh_rect);
	}
	if(GL_TRUE == glIsProgram(stc_2D->sh_sprite))
	{
		bkp_logger_write(BKP_LOGGER_INFO,"deleting shader programe %d\n",stc_2D->sh_sprite);
		glDeleteProgram(stc_2D->sh_sprite);
	}
	if(GL_TRUE == glIsProgram(stc_2D->sh_sprite_alpha))
	{
		bkp_logger_write(BKP_LOGGER_INFO,"deleting shader programe %d\n",stc_2D->sh_sprite_alpha);
		glDeleteProgram(stc_2D->sh_sprite_alpha);
	}
	bkp_memory_releaseTicket(stc_2D);
}

/*--------------------------------------------------------------------------------*/
static void _init_onScreenText(void)
{

	FT_Library ft;
	bkp_logger_write(BKP_LOGGER_INFO,"Initializing FreeType2 Library\n");
	if(FT_Init_FreeType(&ft))
	{
		bkp_logger_write(BKP_LOGGER_ERROR,"FreetType: could not initialized , No text will be rendered\n");
		return;
	}

	stc_ScreenText = (struct TexterGen *) bkp_memory_getTicket(sizeof(struct TexterGen));

	GLuint vao = stc_2D->vao_num;
	stc_2D->vao_num += 1;
	GLuint vbo = 0;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	stc_ScreenText->sh_text = bkp_shader_createFromFile("data/shaders/text.vert","data/shaders/text.frag");
	glBindAttribLocation(stc_ScreenText->sh_text, 0, "vertices");
#ifdef DEBUG 
	bkp_shader_printInfo(stc_ScreenText->sh_text);
#endif

	stc_ScreenText->ft = ft;
	stc_ScreenText->vao = vao;
	stc_ScreenText->vbo = vbo;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	stc_Btext = (struct TextBuffer *) bkp_memory_getTicket(sizeof(struct TextBuffer));
	stc_Btext->head = NULL;
	stc_Btext->count = 0;

	return;
}

/*--------------------------------------------------------------------------------*/
static void _close_onScreenText(void)
{
	if(GL_TRUE == glIsProgram(stc_ScreenText->sh_text))
	{
		bkp_logger_write(BKP_LOGGER_INFO,"deleting shader programe %d\n",stc_ScreenText->sh_text);
		glDeleteProgram(stc_ScreenText->sh_text);
	}

	glDeleteBuffers(1,&stc_ScreenText->vbo);
	glDeleteVertexArrays(1,&stc_ScreenText->vao);

	FT_Done_FreeType(stc_ScreenText->ft);

	bkp_memory_releaseTicket(stc_ScreenText);
	bkp_memory_releaseTicket(stc_Btext);

	return;
}

/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/

