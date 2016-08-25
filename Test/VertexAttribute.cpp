#include "VertexAttribute.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glbinding/gl/gl.h>

#include "Platform.h"

using namespace gl;
 //----------------------------------------------------------------------------
 //----------------------------------------------------------------------------


VertexAttribute::VertexAttribute(gl::GLchar * name, gl::GLint size, gl::GLenum type, gl::GLboolean normalized, gl::GLsizei stride, gl::GLvoid * pointer)
{

	this->name_ = (GLchar *)strdup(name);
	this->index_ = -1;
	this->size_ = size;
	this->type_ = type;
	this->normalized_ = normalized;
	this->stride_ = stride;
	this->pointer_ = pointer;
}


VertexAttribute::~VertexAttribute()
{
	free( this->name_);
}

void VertexAttribute::enable()
{
	if ( this->index_ == -1)
	{
		GLint program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		if (program == 0)
		{
			return;
		}
		this->index_ = glGetAttribLocation(program, this->name_);
		if (this->index_ == -1)
		{
			return;
		}
	}
	glEnableVertexAttribArray(this->index_);
	glVertexAttribPointer(this->index_, this->size_, this->type_, this->normalized_, this->stride_, this->pointer_);
}

VertexAttribute * VertexAttribute::parse(char * format)
{
	GLenum type = (GLenum)0;
	int size;
	int normalized = 0;
	char ctype;
	char *name;

	VertexAttribute *attr;

	char *p = strchr(format, ':');
	if (p != NULL)
	{
		name = strndup(format, p - format);
		if (*(++p) == '\0')
		{
			fprintf(stderr, "No size specified for '%s' attribute\n", name);
			free(name);
			return nullptr;
		}
		size = *p - '0';

		if (*(++p) == '\0')
		{
			fprintf(stderr, "No format specified for '%s' attribute\n", name);
			free(name);
			return nullptr;
		}
		ctype = *p;

		if (*(++p) != '\0')
		{
			if (*p == 'n')
			{
				normalized = 1;
			}
		}

	}
	else
	{
		fprintf(stderr, "Vertex attribute format not understood ('%s')\n", format);
		return nullptr;
	}

	switch (ctype)
	{
	case 'b': type = GL_BYTE;           break;
	case 'B': type = GL_UNSIGNED_BYTE;  break;
	case 's': type = GL_SHORT;          break;
	case 'S': type = GL_UNSIGNED_SHORT; break;
	case 'i': type = GL_INT;            break;
	case 'I': type = GL_UNSIGNED_INT;   break;
	case 'f': type = GL_FLOAT;          break;
	default:  type = (GLenum)0;         break;
	}

	attr = new VertexAttribute(name, size, type, normalized, 0, 0);
	free(name);
	return attr;
}

