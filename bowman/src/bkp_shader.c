#include <stdlib.h>
#include <stdio.h>

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

#include "include/bkp_shader.h"
#include "include/macro.h"
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

static char * load_txtfile(const char * path);
static GLuint create_shader(const GLchar * src_ver, const GLchar * src_fra);

/**************************************************************************
*	Main	
**************************************************************************/



/**************************************************************************
*	Implementations	
**************************************************************************/

/*--------------------------------------------------------------------------------*/
GLuint bkp_shader_create(const GLchar * src_ver, const GLchar * src_fra)
{
	return create_shader(src_ver, src_fra);
}

/*--------------------------------------------------------------------------------*/
GLuint bkp_shader_createFromFile(const GLchar * path_ver, const GLchar * path_fra)
{
	GLchar * src_ver= load_txtfile(path_ver);
	GLchar * src_fra= load_txtfile(path_fra);

	if(src_ver == NULL || src_fra == NULL)
	{
		if(src_ver != NULL)
			free(src_ver);
		if(src_fra != NULL)
			free(src_fra);
		return 0;
	}

	GLuint sh = create_shader((const GLchar * ) src_ver,(const GLchar * ) src_fra);

	free(src_ver);
	free(src_fra);

	return sh;
}

/*--------------------------------------------------------------------------------*/
void bkp_shader_deleteProgram(GLuint sh)
{
	if(GL_TRUE == glIsProgram(sh))
		glDeleteProgram(sh);

	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_shader_printInfo(GLuint nsh)
{
	if(nsh == 0)
		return;

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
		bkp_logger_write(BKP_LOGGER_INFO,"program info log for GL index %u:\n\t\t%s\n",nsh,log);
	}
	bkp_logger_write(BKP_LOGGER_INFO,"------------- end info ---------------\n");
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_shader_setVec3(const GLuint sh, const char * szvar, BKP_Vec3 v)
{
	glUniform3f(glGetUniformLocation(sh, szvar), v.x, v.y, v.z);
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_shader_setVec4(const GLuint sh, const char * szvar, BKP_Vec4 v)
{
	glUniform4f( glGetUniformLocation(sh, szvar), v.x, v.y, v.z, v.w);
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_shader_setMat4(const GLuint sh, const char * szvar, BKP_Mat4 m)
{
	glUniformMatrix4fv( glGetUniformLocation(sh, szvar), 1,GL_FALSE, m.m);
	return;
}

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
static char * load_txtfile(const char * path)
{
	FILE *inf = NULL;
	size_t s_file,
		   val;
	char *str = NULL;

	inf = fopen(path,"rb");
	if(inf==NULL)
	{
		bkp_logger_write(BKP_LOGGER_WARNING,"FILE '%s' not found \n", path);
		return str;
	}

	fseek(inf,0,SEEK_END);
	s_file = ftell(inf);

	ALLOC(char,str,(s_file+1));

	fseek(inf,0,SEEK_SET);
	val=fread(str,s_file,sizeof(char),inf);
	if((val<1)&&(ferror(inf)!=0))
	{
		bkp_logger_write(BKP_LOGGER_ERROR,"I/O error in %s %d\n",__func__,__LINE__-3);
		free(str);
		return NULL;
	}
	fclose(inf);

	str[s_file]='\0';

	return str;
}

/*--------------------------------------------------------------------------------*/
static GLuint create_shader(const GLchar * src_ver, const GLchar * src_fra)
{
	int ok;
	char infoLog[512];

	GLuint sh_ver = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sh_ver, 1, &src_ver, NULL);
	glCompileShader(sh_ver);
	glGetShaderiv(sh_ver, GL_COMPILE_STATUS, &ok);
	if(!ok)
	{
		glGetShaderInfoLog(sh_ver, 512, NULL, infoLog);
		bkp_logger_write(BKP_LOGGER_ERROR,"ERROR::SHADER::VERTEX::COMPILATION_FAILED %s\n" ,infoLog );
		return 0;
	}

	GLuint sh_fra = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sh_fra, 1, &src_fra, NULL);
	glCompileShader(sh_fra);
	glGetShaderiv(sh_fra, GL_COMPILE_STATUS, &ok);
	if(!ok)
	{
		glGetShaderInfoLog(sh_fra, 512, NULL, infoLog);
		bkp_logger_write(BKP_LOGGER_ERROR,"ERROR::SHADER::FRAGMENT::COMPILATION_FAILED %s\n" ,infoLog );
		return 0;
	}

	GLuint sh = glCreateProgram();
	glAttachShader(sh, sh_ver);
	glAttachShader(sh, sh_fra);
	glLinkProgram(sh);

	glGetProgramiv(sh, GL_LINK_STATUS, &ok);
	if(!ok) 
	{
		glGetProgramInfoLog(sh, 512, NULL, infoLog);
		bkp_logger_write(BKP_LOGGER_ERROR,"ERROR::SHADER::PROGRAM::LINKING%s\n" ,infoLog );
		return 0;
	}

	glDeleteShader(sh_ver);
	glDeleteShader(sh_fra);

	return sh;
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

