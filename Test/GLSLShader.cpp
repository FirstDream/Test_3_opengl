#include "GLSLShader.h"
#include <iostream>

#include <glbinding/gl/gl.h>

using namespace gl;

GLSLShader::GLSLShader()
{
	totalShaders_				=0;
	shaders_[VERTEX_SHADER]		=0;
	shaders_[FRAGMENT_SHADER]	=0;
	shaders_[GEOMETRY_SHADER]	=0;

	attributeList_.clear();
	uniformLocationList_.clear();
}

GLSLShader::~GLSLShader()
{
	attributeList_.clear();	
	uniformLocationList_.clear();
}

void GLSLShader::DeleteShaderProgram() 
{	
	glDeleteProgram( program_ );
}

void GLSLShader::LoadFromString( GLenum type, const std::string& source ) 
{	
	GLuint shader = glCreateShader ( type );

	const char* ptmp = source.c_str();
	glShaderSource ( shader, 1, &ptmp, NULL );
	
	//check whether the shader loads fine
	GLint status;
	glCompileShader ( shader );
	glGetShaderiv ( shader, GL_COMPILE_STATUS, &status );

	//gl_false;
	if (status == 0 ) 
	{
		GLint infoLogLength;		
		glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLogLength );
		GLchar *infoLog= new GLchar[infoLogLength];
		glGetShaderInfoLog ( shader, infoLogLength, NULL, infoLog );
		std::cerr<< "Compile log: " <<infoLog <<std::endl;
		delete [] infoLog;
	}

	shaders_[totalShaders_++]= shader;
}


void GLSLShader::CreateAndLinkProgram() 
{
	program_ = glCreateProgram ();

	if (shaders_[VERTEX_SHADER] != 0) {
		glAttachShader ( program_, shaders_[VERTEX_SHADER] );
	}
	if (shaders_[FRAGMENT_SHADER] != 0) {
		glAttachShader ( program_, shaders_[FRAGMENT_SHADER] );
	}
	if (shaders_[GEOMETRY_SHADER] != 0) {
		glAttachShader ( program_, shaders_[GEOMETRY_SHADER] );
	}
	
	//link and check whether the program links fine
	GLint status;
	glLinkProgram ( program_ );
	glGetProgramiv ( program_, GL_LINK_STATUS, &status );
	
	//GL_FALSE
	if (status == 0) 
	{
		GLint infoLogLength;
		
		glGetProgramiv ( program_, GL_INFO_LOG_LENGTH, &infoLogLength );
		GLchar *infoLog= new GLchar[ infoLogLength ];
		glGetProgramInfoLog ( program_, infoLogLength, NULL, infoLog );
		std::cerr<<"Link log: "<<infoLog<< std::endl;
		delete [] infoLog;
	}

	glDeleteShader( shaders_[VERTEX_SHADER]	  );
	glDeleteShader( shaders_[FRAGMENT_SHADER] );
	glDeleteShader( shaders_[GEOMETRY_SHADER] );
}

void GLSLShader::Use() 
{
	glUseProgram( program_ );
}

void GLSLShader::UnUse() 
{
	glUseProgram(0);
}

void GLSLShader::AddAttribute( const std::string& attribute ) 
{
	attributeList_[attribute]= glGetAttribLocation( program_, attribute.c_str() );	
}

//An indexer that returns the location of the attribute
GLuint GLSLShader::operator []( const std::string& attribute ) 
{
	return attributeList_[attribute];
}

void GLSLShader::AddUniform(const std::string& uniform) 
{
	uniformLocationList_[uniform] = glGetUniformLocation(program_, uniform.c_str());
}

GLuint GLSLShader::operator()( const std::string& uniform)
{
	return uniformLocationList_[uniform];
}

#include <fstream>
void GLSLShader::LoadFromFile( GLenum whichShader, const std::string& filename )
{
	std::ifstream fp;
	fp.open( filename.c_str(), std::ios_base::in );
	if(fp) 
	{		 
		std::string line, buffer;
		while(std::getline(fp, line))
		{
			buffer.append(line);
			buffer.append("\r\n");
		}		 
		//copy to source
		LoadFromString(whichShader, buffer);		
	} else {
		std::cerr<<"Error loading shader: "<<filename<< std::endl;
	}
}
