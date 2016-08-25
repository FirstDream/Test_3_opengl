#pragma once
// #ifndef __GLSL_SHADER__H
// #define __GLSL_SHADER__H

#include <map>
#include <string>

#include <glbinding/gl/types.h>


class GLSLShader
{
public:
	GLSLShader();
	~GLSLShader();	
	
	void LoadFromString( gl::GLenum whichShader, const std::string& source );
	void LoadFromFile(gl::GLenum whichShader, const std::string& filename);
	void CreateAndLinkProgram();
	
	void Use();
	void UnUse();
	
	void AddAttribute( const std::string& attribute );
	void AddUniform(   const std::string& uniform );

	//An indexer that returns the location of the attribute/uniform
	gl::GLuint operator[]( const std::string& attribute );
	gl::GLuint operator()( const std::string& uniform );

	void DeleteShaderProgram();

private:
	enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER};
	gl::GLuint	program_;
	int totalShaders_;
	gl::GLuint shaders_[3];			//0->vertexshader, 1->fragmentshader, 2->geometryshader

	std::map<std::string, gl::GLuint> attributeList_;
	std::map<std::string, gl::GLuint> uniformLocationList_;
};	

//#endif