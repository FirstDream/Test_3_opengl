
#include "Rect2d.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>

#include <glbinding/gl/gl.h>


#include "GLSLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace gl;


GLenum err;

#define GL_CHECK_ERRORS 
//#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

Rect2d::Rect2d()
: a_vertex(-1)
, m_vao(0)
, m_indices(0)
, m_vertices(0)
, m_texture(0)

{

	//init shader;
	std::string vert_file = "./OpenGL/Shaders/rect.vert";
	std::string frag_file = "./OpenGL/Shaders/rect.frag";

	this->m_shader = new GLSLShader();

	m_shader->LoadFromFile( GL_VERTEX_SHADER, vert_file );
	m_shader->LoadFromFile( GL_FRAGMENT_SHADER, frag_file );
	m_shader->CreateAndLinkProgram();
	
	m_shader->Use();
	{
		m_shader->AddUniform("MVP");
		m_shader->AddUniform("Texture");
		glUniform1i((*m_shader)("Texture"), 0);
		m_shader->AddAttribute("vVertices");
		m_shader->AddAttribute("vTexCoord");
	}
	m_shader->UnUse();

	GL_CHECK_ERRORS
    
		// create 
	static const GLfloat vertices_pos[16] =
    {
		-1.0f, -1.0f, 1.0f, 1.0f, // 0
		 1.0f, -1.0f, 1.0f, 1.0f, // 1
		 1.0f,  1.0f, 1.0f, 1.0f, // 2
		-1.0f,  1.0f, 1.0f, 1.0f  // 3
    };

	static const GLfloat texture_pos[8] =
	{
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
	};

    static const GLubyte indices_data[6] = 
	{ 0, 1, 2, 0, 2, 3 };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
	GL_CHECK_ERRORS
    glGenBuffers(1, &m_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
	GL_CHECK_ERRORS
    glBufferData(GL_ARRAY_BUFFER, 24 *sizeof(GLfloat), nullptr, GL_STATIC_DRAW);
	GL_CHECK_ERRORS
	glBufferSubData(GL_ARRAY_BUFFER, 0, 16 * sizeof(GLfloat), vertices_pos);
	GL_CHECK_ERRORS
	glBufferSubData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), 16 * sizeof(GLfloat), texture_pos);
	GL_CHECK_ERRORS
	glEnableVertexAttribArray( (*m_shader)["vVertices"]);
	glVertexAttribPointer( (*m_shader)["vVertices"], 4, GL_FLOAT, GL_FALSE, 0, NULL);
	GL_CHECK_ERRORS
	glEnableVertexAttribArray((*m_shader)["vTexCoord"]);
	glVertexAttribPointer((*m_shader)["vTexCoord"], 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertices_pos));
	GL_CHECK_ERRORS
    glGenBuffers(1, &m_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_data), indices_data, GL_STATIC_DRAW);
	GL_CHECK_ERRORS
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	GL_CHECK_ERRORS
    // view
    m_view = glm::lookAt( glm::vec3(0.f, 0.0f,0.0f),glm::vec3( 0.f, 0.0f, 1.0f ), glm::vec3( 0.f, 1.f, 0.f) );
}

Rect2d::~Rect2d()
{
    glDeleteBuffers(1, &m_vertices);
    glDeleteBuffers(1, &m_indices);
	
	delete this->m_shader;

}

void Rect2d::resize(int width, int height)
{
    m_projection = glm::perspective(40.f, static_cast<GLfloat>(width) / static_cast<GLfloat>(height), 0.1f, 10000.0f);

    glViewport(0, 0, width, height);
	GL_CHECK_ERRORS
	this->w_ = width;
	this->h_ = height;
}

void Rect2d::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//this->m_shader->Use();
    //const glm::mat4 MVP = m_projection * m_view;
	//err = glGetError();
	//GL_CHECK_ERRORS
    //glUniformMatrix4fv( (*m_shader)( "MVP" ), 1, GL_FALSE, glm::value_ptr( MVP ) );
	//GL_CHECK_ERRORS
	//glBindVertexArray(this->m_vao);
	//glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL );
	//GL_CHECK_ERRORS
	//glBindVertexArray(0);
	//this->m_shader->UnUse();
	//GL_CHECK_ERRORS

	glMatrixMode(GL_PROJECTION);
	//glOrtho(0, w_, 0, h_, -1, 1);
	glm::mat4  proj = glm::perspective(60.f, static_cast<GLfloat>(w_) / static_cast<GLfloat>(h_), 0.1f, 10000.0f);
	glLoadMatrixf(glm::value_ptr(m_projection));
	glMatrixMode(GL_MODELVIEW);
	glm::mat4 view = glm::lookAt(glm::vec3(0.f, 0.0f, 0.0f), glm::vec3(0.f, 0.0f, 1.0f), glm::vec3(0.f, 1.f, 0.f));
	glLoadMatrixf(glm::value_ptr(m_view));
	
	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	{
		glVertex3d( -2.0, -2.0, 1.0 );
		glVertex3d(  2.0, -2.0, 1.0 );
		glVertex3d(  2.0,  2.0, 1.0);
		glVertex3d( -2.0,  2.0, 1.0 );
	}
	glEnd();
}

void Rect2d::updateTexture(cv::Mat & data)
{
	if ( m_texture == 0 )
	{
		glGenTextures(1, &m_texture);
		//glDeleteTextures(1, &m_texture);
	}
		 
	//glGenTextures(1, &m_texture);

	glBindTexture(GL_TEXTURE_2D, m_texture);

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
		break;

	case CV_16S:
		type = GL_SHORT;
		break;
	
	default:
		type = GL_BYTE;
		break;
	}

	glTexImage2D( GL_TEXTURE_2D,0, GL_R16F, data.cols, data.rows,0, GL_LUMINANCE, type, data.ptr() );
	glBindTexture(GL_TEXTURE_2D, 0);
	
}

