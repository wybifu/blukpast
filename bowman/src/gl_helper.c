#include <stdlib.h>
#include <stdio.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/gl_helper.h"
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

/***************************************************************************
*	Prototypes 	
**************************************************************************/

/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/
int compilerSL(GLuint nsh,unsigned char compileorlink)
{
	int params = -1;
	if(compileorlink == 0)
	{
		glCompileShader(nsh);
		glGetShaderiv(nsh,GL_COMPILE_STATUS, &params);
	}
	else
	{
		glGetProgramiv(nsh,GL_LINK_STATUS, &params);
		is_valid(nsh);
	}

	if(GL_TRUE != params)
	{
		int max_l = 2048;
		int len = 0;
		char log[2048];

		if(compileorlink == 0)
		{
			bkp_logger_write(BKP_LOGGER_ERROR,"GL shader %i did not compile\n",nsh);
			glGetShaderInfoLog(nsh,max_l,&len,log);
			bkp_logger_write(BKP_LOGGER_ERROR,"%s\n",log);
		}
		else
		{
			bkp_logger_write(BKP_LOGGER_ERROR,"Linking error program  %i did not link\n",nsh);
			glGetProgramInfoLog(nsh,max_l,&len,log);
			bkp_logger_write(BKP_LOGGER_ERROR,"program info log for GL index %u:\n\t\t%s\n",nsh,log);
		}
		return 0;
	}

	return 1;
}

/*--------------------------------------------------------------------------------*/
void print_all_SL(GLuint nsh)
{
	int params = -1;

	bkp_logger_write(BKP_LOGGER_INFO,"------------- shader programme %i info ---------------\n",nsh);
	glGetProgramiv(nsh,GL_LINK_STATUS,&params);
	bkp_logger_write(BKP_LOGGER_INFO,"\t\tGL_LINK_STATUS = %i:\n",params);

	glGetProgramiv(nsh, GL_ATTACHED_SHADERS,&params);
	bkp_logger_write(BKP_LOGGER_INFO,"\t\tGL_ATTACHED_SHADERS = %i:\n",params);

	glGetProgramiv(nsh, GL_ACTIVE_ATTRIBUTES, &params);
	bkp_logger_write(BKP_LOGGER_INFO,"\t\tGL_ACTIVE_ATTRIBUTES = %i:\n",params);
	GLuint i;
	for(i = 0; i < (GLuint)params; ++i)
	{
		int max_l = 64;
		int len = 0;
		int size = 0;
		char name[max_l];
		GLenum type;
		glGetActiveAttrib(nsh, i, max_l, &len, &size, &type, name);
		if(size > 1)
		{
			int j;
			for(j = 0; j < size; ++j)
			{
				char long_name[max_l];
				sprintf(long_name, "%s[%i]",name,j);
				int location = glGetAttribLocation(nsh, long_name);
				bkp_logger_write(BKP_LOGGER_INFO, "\t\t%i) type:%s name: %s location: %i\n", i, GL_type_to_string(type),
						long_name, location);
			}
		}
		else
		{
			int location = glGetAttribLocation(nsh, name);
			bkp_logger_write(BKP_LOGGER_INFO, "\t\t%i) type:%s name: %s location: %i\n", i, GL_type_to_string(type),
					name, location);
		}
	}

	glGetProgramiv(nsh, GL_ACTIVE_UNIFORMS, &params);
	bkp_logger_write(BKP_LOGGER_INFO,"\t\tGL_ACTIVE_UNIFORMS = %i:\n",params);
	for(i = 0; i < (GLuint)params; ++i)
	{
		int max_l = 64;
		int len = 0;
		int size = 0;
		char name[max_l];
		GLenum type;
		glGetActiveUniform(nsh, i, max_l, &len, &size, &type, name);

		if(size > 1)
		{
			int j;
			for(j = 0; j < size; ++j)
			{
				char long_name[max_l];
				sprintf(long_name, "%s[%i]",name,j);
				int location = glGetUniformLocation(nsh, long_name);
				bkp_logger_write(BKP_LOGGER_INFO, "\t\t%i) type:%s name: %s location: %i\n", i, GL_type_to_string(type),
						long_name, location);
			}
		}
		else
		{
			int location = glGetUniformLocation(nsh, name);
			bkp_logger_write(BKP_LOGGER_INFO, "\t\t%i) type:%s name: %s location: %i\n", i, GL_type_to_string(type),
					name, location);
		}
	}

	glGetProgramiv(nsh,GL_LINK_STATUS, &params);
	int max_l = 2048;
	int len = 0;
	char log[2048];
	if(GL_TRUE != params)
	{
		glGetProgramInfoLog(nsh,max_l,&len,log);
		bkp_logger_write(BKP_LOGGER_ERROR,"program info log for GL index %u:\n\t\t%s\n",nsh,log);
	}
	bkp_logger_write(BKP_LOGGER_INFO,"------------- end info ---------------\n");

}

/*--------------------------------------------------------------------------------*/
GLboolean is_valid(GLuint nsh)
{
	int params = -1;
	glValidateProgram(nsh);

	glGetProgramiv(nsh,GL_VALIDATE_STATUS, &params);
	bkp_logger_write(BKP_LOGGER_INFO,"program %i GL_VALIDATE_STATUS = %i:\n",nsh,params);
	if(GL_TRUE != params)
	{
		bkp_logger_write(BKP_LOGGER_ERROR,"program %i not valide\n",nsh);
		return GL_FALSE;
	}
	
	return GL_TRUE;
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
/*--------------------------------------------------------------------------------*/

