#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/_windowing.h"
#include "include/bkp_logger.h"

/**************************************************************************
*	Defines & Maro
**************************************************************************/

/**************************************************************************
*	Structs, Enum, Unio and Typesdef	
**************************************************************************/

/**************************************************************************
*	Globals	
**************************************************************************/

int	stc_win_width ;
int	stc_win_height;
int	stc_fb_width ;
int	stc_fb_height ;

/***************************************************************************
*	Prototypes 	
**************************************************************************/

/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/

int hw_init_all(int V, int v)
{

	bkp_logger_write(BKP_LOGGER_INFO,"Starting GLFW %s\n",glfwGetVersionString());

	glfwSetErrorCallback(hw_glfw_error_callback); 
	if(GLFW_FALSE == glfwInit())
	{
		bkp_logger_write(BKP_LOGGER_FATAL,"Could not Init GLFW3!\n");
		return BKP_FALSE;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,V);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,v);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4); // anti-aliasing  4 is ok

	return BKP_TRUE;
}

/*--------------------------------------------------------------------------------*/
void hw_log_gl_params()
{
	GLenum params[] = {
		GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		GL_MAX_CUBE_MAP_TEXTURE_SIZE,
		GL_MAX_DRAW_BUFFERS,
		GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
		GL_MAX_TEXTURE_IMAGE_UNITS,
		GL_MAX_TEXTURE_SIZE,
		GL_MAX_VARYING_FLOATS,
		GL_MAX_VERTEX_ATTRIBS,
		GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
		GL_MAX_VERTEX_UNIFORM_COMPONENTS,
		GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV,
		GL_MAX_SAMPLES,
		GL_MAX_VIEWPORT_DIMS,
		GL_STEREO,
	};

	const char * names[] = {
		"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
		"GL_MAX_CUBE_MAP_TEXTURE_SIZE",
		"GL_MAX_DRAW_BUFFERS",
		"GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
		"GL_MAX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_TEXTURE_SIZE",
		"GL_MAX_VARYING_FLOATS",
		"GL_MAX_VERTEX_ATTRIBS",
		"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_VERTEX_UNIFORM_COMPONENTS",
		"GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV",
		"GL_MAX_SAMPLES",
		"GL_MAX_VIEWPORT_DIMS",
		"GL_STEREO",
	};

	bkp_logger_write(BKP_LOGGER_INFO,"GL Context Params : \n");
	int i;
	for(i = 0; i < 12; ++i)
	{
		int v = 0;
		glGetIntegerv(params[i], &v);
		bkp_logger_write(BKP_LOGGER_INFO,"%s : %i\n",names[i], v);
	}
	
	int v[2];
	v[0] = v[1] = 0;
	glGetIntegerv(params[12], v);
	bkp_logger_write(BKP_LOGGER_INFO,"%s : %i : %i\n",names[10], v[0], v[1]);
	unsigned char s = 0;
	glGetBooleanv(params[13], &s);
	bkp_logger_write(BKP_LOGGER_INFO,"%s : %i\n",names[11], s);
	bkp_logger_write(BKP_LOGGER_INFO,"--------------------------\n\n");

	return;
}

/*--------------------------------------------------------------------------------*/
GLFWwindow * hw_openwindow(int * w0, int  * h0, const char * sztitle, int fullscreen)
{
	//GLFWmonitor * mon = glfwGetPrimaryMonitor();
	const GLFWvidmode * vmode;
	int i;
	int mn;
	int w = * w0, h = * h0;
	GLFWmonitor ** mon = glfwGetMonitors(&mn);
	GLFWmonitor * primary = NULL;

	bkp_logger_write(BKP_LOGGER_INFO,"Monitor info:\n");
	for(i = mn - 1; i >= 0; --i)
	{
		vmode = glfwGetVideoMode(mon[i]);
		bkp_logger_write(BKP_LOGGER_INFO,"#%d %ix%i\n",i,vmode->width, vmode->height);
	}
	bkp_logger_write(BKP_LOGGER_INFO,"--------------------------\n\n");

	if(fullscreen == BKP_TRUE)
	{
		fprintf(stderr,"full screen\n");
		primary = mon[0];
	}

	GLFWwindow * window = NULL;

	if(w != 0 && h != 0)
	{
		window = glfwCreateWindow(w, h, sztitle, primary, NULL);

		stc_win_width = w;
		stc_win_height = h;
		stc_fb_width = w;
		stc_fb_height = h;
	}
	else
	{
		window = glfwCreateWindow(vmode->width, vmode->height, sztitle, primary, NULL);
		stc_win_width = vmode->width;
		stc_win_height = vmode->height;
		stc_fb_width = vmode->width ;
		stc_fb_height = vmode->height;
	}


	*w0 = stc_fb_width;
	*h0 = stc_fb_height;

	return window;
}

/*--------------------------------------------------------------------------------*/

void hw_glfw_error_callback(int error,const char * description)
{
	bkp_logger_write(BKP_LOGGER_ERROR,"[GLFW] : [code : %i ] -> %s\n",error,description);
	return;
}

/*--------------------------------------------------------------------------------*/
void hw_glfw_window_size_callback(GLFWwindow * window, int width, int height)
{
	stc_win_width = width;
	stc_win_height = height;
	return;
}

/*--------------------------------------------------------------------------------*/
void hw_glfw_framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	stc_fb_width = width;
	stc_fb_height = height;
	return;
}


/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/


