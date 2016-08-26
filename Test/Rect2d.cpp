
#include "Rect2d.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>

#include <glbinding/gl/gl.h>


#include "GLSLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

#include "VertexBuffer.h"


using namespace gl;


GLenum err;

#define GL_CHECK_ERRORS 
//#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

Rect2d::Rect2d()
	:texture_id_(0)
{
	//init shader;
	std::string vert_file = "./OpenGL/Shaders/rect.vert";
	std::string frag_file = "./OpenGL/Shaders/rect.frag";

	this->shader_ = new GLSLShader();

	shader_->LoadFromFile( GL_VERTEX_SHADER, vert_file );
	shader_->LoadFromFile( GL_FRAGMENT_SHADER, frag_file );
	shader_->CreateAndLinkProgram();
	
	shader_->Use();
	{
		shader_->AddUniform("MVP");
		shader_->AddUniform("Texture");
		shader_->AddUniform("vWindowColor");
		glUniform1i((*shader_)("Texture"), 0);
		shader_->AddAttribute("vVertex");
		shader_->AddAttribute("vTexCoord");
	}
	shader_->UnUse();

	GL_CHECK_ERRORS
    
	typedef struct { float x, y, z; } xyz;
	typedef struct { float s, t; } st;
	typedef struct { xyz position; st texcoord; } vertex;
	
	xyz v[] = 
	{ { -1, -1, 1 },{ 1, -1, 1 },{ 1, 1, 1 },{ -1, 1, 1 }, };
	
	st n[] = { { 0, 1 },{ 1, 1 },{ 1, 0 }, { 0, 0 } };
	
	vertex vertices[4] = {
		{ v[0],n[0] },
		{ v[1],n[1] },
		{ v[2],n[2] },
		{ v[3],n[3] },};

	GLuint indices[6] = { 0, 1, 2, 0, 2, 3 };

	this->vertex_buffer_ = new VertexBuffer("vVertex:3f,vTexCoord:2f");
	this->vertex_buffer_->push_back(vertices, 4, indices, 6);

}

Rect2d::~Rect2d()
{

	delete this->vertex_buffer_;
	delete this->shader_;

}

void Rect2d::resize(int width, int height)
{
	this->w_ = width;
	this->h_ = height;
}

void Rect2d::draw( glm::mat4& MVP )
{
	shader_->Use();
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_id_);
	{
		glUniformMatrix4fv( (*shader_)("MVP"), 1, 0, glm::value_ptr(MVP) );
		glUniform2fv((*shader_)("vWindowColor"), 1, glm::value_ptr(this->window_color_normal_));
		this->vertex_buffer_->render(GL_TRIANGLES);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	shader_->UnUse();


}

void Rect2d::updateTexture(cv::Mat & data)
{
	if ( texture_id_ == 0 )
	{
		glGenTextures(1, &texture_id_);
		//glDeleteTextures(1, &m_texture);
	}
		 
	//glGenTextures(1, &m_texture);

	glBindTexture(GL_TEXTURE_2D, texture_id_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int a = data.type();
	GLenum type;
	switch ( a )
	{
	case CV_16U:
		type = GL_UNSIGNED_SHORT;
		this->max_val_ = 65535;
		this->min_val_ = 0;
		break;

	case CV_16S:
		type = GL_SHORT;
		this->max_val_ = 32767;
		this->min_val_ = -32768;
		break;
	
	default:
		type = GL_UNSIGNED_BYTE;
		this->max_val_ = 255;
		this->min_val_ = 0;
		this->window_color_[0] = 127.5;
		this->window_color_[1] = 256;
		break;
	}
	this->window_color_[0] = 1471;
	this->window_color_[1] = 2340;

	this->editWindowColor(glm::dvec2( 0, 0 ));
	glTexImage2D( GL_TEXTURE_2D,0, GL_LUMINANCE16F_ARB, data.cols, data.rows,0, GL_LUMINANCE, type, data.ptr() );

	glBindTexture(GL_TEXTURE_2D, 0);
	
}

void Rect2d::editWindowColor(glm::dvec2 delta)
{

	this->window_color_ += delta;

	this->window_color_[0] = glm::clamp(this->window_color_[0], float(this->min_val_), float(this->max_val_));

	this->window_color_[1] = glm::clamp(this->window_color_[1], float(1), float(this->max_val_ - this->min_val_ + 1));

	this->window_color_normal_[1] = window_color_[1] / max_val_;
	this->window_color_normal_[0] = window_color_[0] / max_val_;

	std::cout << this->window_color_[0] << ", " << this->window_color_[1] << std::endl;

}

