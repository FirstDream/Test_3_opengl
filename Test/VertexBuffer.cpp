#include "VertexBuffer.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include "Platform.h"

using namespace gl;

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------


VertexBuffer::VertexBuffer(const char * format)
{
	size_t i, index = 0, stride = 0;
	const char *start = 0, *end = 0;
	GLchar *pointer = 0;


	this->format_ = strdup(format);

	for (i = 0; i<MAX_VERTEX_ATTRIBUTE; ++i)
	{
		this->attributes_[i] = 0;
	}

	start = format;

	do
	{
		char *desc = 0;
		VertexAttribute *attribute;
		GLuint attribute_size = 0;
		end = (char *)(strchr(start + 1, ','));

		if (end == NULL)
		{
			desc = strdup(start);
		}
		else
		{
			desc = strndup(start, end - start);
		}
		attribute = VertexAttribute::parse(desc);
		start = end + 1;
		free(desc);

		attribute->setPointer(pointer);

		switch (attribute->getType())
		{
		case GL_BOOL:           attribute_size = sizeof(GLboolean); break;
		case GL_BYTE:           attribute_size = sizeof(GLbyte); break;
		case GL_UNSIGNED_BYTE:  attribute_size = sizeof(GLubyte); break;
		case GL_SHORT:          attribute_size = sizeof(GLshort); break;
		case GL_UNSIGNED_SHORT: attribute_size = sizeof(GLushort); break;
		case GL_INT:            attribute_size = sizeof(GLint); break;
		case GL_UNSIGNED_INT:   attribute_size = sizeof(GLuint); break;
		case GL_FLOAT:          attribute_size = sizeof(GLfloat); break;
		default:                attribute_size = 0;
		}
		gl::GLuint attr_size = attribute->getSize();
		stride  += attr_size*attribute_size;
		pointer += attr_size*attribute_size;
		this->attributes_[index] = attribute;
		index++;
	} while (end && (index < MAX_VERTEX_ATTRIBUTE));

	for (i = 0; i<index; ++i)
	{
		this->attributes_[i]->setStride(stride);
	}


	this->VAO_id_ = 0;


	this->vertices_ = vector_new(stride);
	this->vertices_id_ = 0;
	this->GPU_vsize_ = 0;

	this->indices_ = vector_new(sizeof(GLuint));
	this->indices_id_ = 0;
	this->GPU_isize_ = 0;

	this->items_ = vector_new(sizeof(glm::ivec4));
	this->state_ = STATUS_DIRTY;
	this->mode_ = GL_TRIANGLES;
}

VertexBuffer::~VertexBuffer()
{
	size_t i;

	for (i = 0; i<MAX_VERTEX_ATTRIBUTE; ++i)
	{
		if (this->attributes_[i])
		{
			delete this->attributes_[i];
		}
	}

	if ( this->VAO_id_ )
	{
		glDeleteVertexArrays(1, &this->VAO_id_);
	}
	this->VAO_id_ = 0;


	vector_delete( this->vertices_ );
	this->vertices_ = 0;
	if (this->vertices_id_)
	{
		glDeleteBuffers(1, &this->vertices_id_);
	}
	this->vertices_id_ = 0;

	vector_delete(this->indices_);
	this->indices_ = 0;
	if (this->indices_id_)
	{
		glDeleteBuffers(1, &this->indices_id_);
	}
	this->indices_id_ = 0;

	vector_delete( this->items_);

	if ( this->format_)
	{
		free( this->format_);
	}
	this->format_ = 0;
	this->state_ = 0;
}

size_t VertexBuffer::size()
{
	return vector_size(this->items_);

}

const char * VertexBuffer::format()
{
	return this->format_;
}

void VertexBuffer::print()
{
	int i = 0;
	static char *gltypes[9] = {
		"GL_BOOL",
		"GL_BYTE",
		"GL_UNSIGNED_BYTE",
		"GL_SHORT",
		"GL_UNSIGNED_SHORT",
		"GL_INT",
		"GL_UNSIGNED_INT",
		"GL_FLOAT",
		"GL_VOID"
	};

	fprintf(stderr, "%zd vertices, %zd indices\n",
		vector_size(this->vertices_), vector_size(this->indices_));
	while (this->attributes_[i])
	{
		int j = 8;
		switch (this->attributes_[i]->getType())
		{
		case GL_BOOL:           j = 0; break;
		case GL_BYTE:           j = 1; break;
		case GL_UNSIGNED_BYTE:  j = 2; break;
		case GL_SHORT:          j = 3; break;
		case GL_UNSIGNED_SHORT: j = 4; break;
		case GL_INT:            j = 5; break;
		case GL_UNSIGNED_INT:   j = 6; break;
		case GL_FLOAT:          j = 7; break;
		default:                j = 8; break;
		}
		fprintf(stderr, "%s : %dx%s (+%p)\n",
			this->attributes_[i]->getName(),
			this->attributes_[i]->getSize(),
			gltypes[j],
			this->attributes_[i]->getPointer());

		i += 1;
	}
}

void VertexBuffer::render(GLenum mode)
{
	size_t vcount = this->vertices_->size;
	size_t icount = this->indices_->size;

	this->render_setup(mode);
	if (icount)
	{
		glDrawElements(mode, icount, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(mode, 0, vcount);
	}
	this->render_finish();
}

void VertexBuffer::render_item(size_t index)
{
	glm::ivec4 * item = (glm::ivec4 *)vector_get(this->items_, index);
	assert(index < vector_size(this->items_));

	if (this->indices_->size)
	{
		size_t start = item->z;
		size_t count = item->w;
		glDrawElements(this->mode_, count, GL_UNSIGNED_INT, (void *)(start * sizeof(GLuint)));
	}
	else if (this->vertices_->size)
	{
		size_t start = item->x;
		size_t count = item->y;
		glDrawArrays(this->mode_, start*this->vertices_->item_size, count);
	}
}

void VertexBuffer::clear()
{
	this->state_ = STATUS_FROZEN;
	vector_clear(this->indices_);
	vector_clear(this->vertices_);
	vector_clear(this->items_);
	this->state_ = STATUS_DIRTY;
}

size_t VertexBuffer::push_back(const void * vertices, const size_t vcount, const GLuint * indices, const size_t icount)
{
	return this->insert( vector_size( this->items_), vertices, vcount, indices, icount);
}

size_t VertexBuffer::insert(const size_t index, const void * vertices, const size_t vcount, const GLuint * indices, const size_t icount)
{
	size_t vstart, istart, i;
	glm::ivec4 item;
	assert(vertices);
	assert(indices);

	this->state_ = STATUS_FROZEN;

	// Push back vertices
	vstart = vector_size(this->vertices_);
	this->push_back_vertices( vertices, vcount);

	// Push back indices
	istart = vector_size(this->indices_);
	this->push_back_indices(indices, icount);

	// Update indices within the vertex buffer
	for (i = 0; i<icount; ++i)
	{
		*(GLuint *)(vector_get(this->indices_, istart + i)) += vstart;
	}

	// Insert item
	item.x = vstart;
	item.y = vcount;
	item.z = istart;
	item.w = icount;
	vector_insert(this->items_, index, &item);

	this->state_ = STATUS_DIRTY;
	return index;
}

void VertexBuffer::erase(const size_t index)
{
	glm::ivec4 * item;
	int vstart;
	size_t vcount, istart, icount, i;

	assert(index < vector_size(this->items_));

	item = (glm::ivec4 *)vector_get(this->items_, index);
	vstart = item->x;
	vcount = item->y;
	istart = item->z;
	icount = item->w;

	// Update items
	for (i = 0; i<vector_size(this->items_); ++i)
	{
		glm::ivec4 * item = (glm::ivec4 *)vector_get(this->items_, i);
		if (item->x > vstart)
		{
			item->x -= vcount;
			item->z -= icount;
		}
	}

	this->state_ = STATUS_FROZEN;
	this->erase_indices(istart, istart + icount);
	this->erase_vertices(vstart, vstart + vcount);
	vector_erase(this->items_, index);
	this->state_ = STATUS_DIRTY;
}

void VertexBuffer::render_setup(GLenum mode)
{
	size_t i;

	// Unbind so no existing VAO-state is overwritten,
	// (e.g. the GL_ELEMENT_ARRAY_BUFFER-binding).
	glBindVertexArray(0);

	if (this->state_ != STATUS_CLEAN)
	{
		this->upload();
		this->state_ = STATUS_CLEAN;
	}

	if (this->VAO_id_ == 0)
	{
		// Generate and set up VAO

		glGenVertexArrays(1, &this->VAO_id_);
		glBindVertexArray(this->VAO_id_);

		glBindBuffer(GL_ARRAY_BUFFER, this->vertices_id_);

		for (i = 0; i<MAX_VERTEX_ATTRIBUTE; ++i)
		{
			VertexAttribute *attribute = this->attributes_[i];
			if (attribute == 0)
			{
				continue;
			}
			else
			{
				attribute->enable();
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (this->indices_->size)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indices_id_);
		}
	}

	// Bind VAO for drawing
	glBindVertexArray(this->VAO_id_);


	this->mode_ = mode;
}

void VertexBuffer::upload()
{
	size_t vsize, isize;

	if (this->state_ == STATUS_FROZEN)
	{
		return;
	}

	if (!this->vertices_id_)
	{
		glGenBuffers(1, &this->vertices_id_);
	}
	if (!this->indices_id_)
	{
		glGenBuffers(1, &this->indices_id_);
	}

	vsize = this->vertices_->size * this->vertices_->item_size;
	isize = this->indices_ ->size * this->indices_->item_size;


	// Always upload vertices first such that indices do not point to non
	// existing data (if we get interrupted in between for example).

	// Upload vertices
	glBindBuffer(GL_ARRAY_BUFFER, this->vertices_id_);
	if (vsize != this->GPU_vsize_)
	{
		glBufferData(GL_ARRAY_BUFFER, vsize, this->vertices_->items, GL_DYNAMIC_DRAW);
		this->GPU_vsize_ = vsize;
	}
	else
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, vsize, this->vertices_->items);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Upload indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indices_id_);
	if ( isize != this->GPU_isize_ )
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, this->indices_->items, GL_DYNAMIC_DRAW);
		this->GPU_isize_ = isize;
	}
	else
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, isize, this->indices_->items);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VertexBuffer::render_finish()
{
	glBindVertexArray(0);
}

void VertexBuffer::push_back_indices(const GLuint * indices, const size_t icount)
{
	this->state_ |= STATUS_DIRTY;
	vector_push_back_data(this->indices_, indices, icount);
}

void VertexBuffer::push_back_vertices(const void * vertices, const size_t vcount)
{
	this->state_ |= STATUS_DIRTY;
	vector_push_back_data(this->vertices_, vertices, vcount);
}

void VertexBuffer::insert_indices(const size_t index, const GLuint * indices, const size_t icount)
{
	assert(this->indices_);
	assert(index < this->indices_->size + 1);

	this->state_ |= STATUS_DIRTY;
	vector_insert_data(this->indices_, index, indices, icount);
}

void VertexBuffer::insert_vertices(const size_t index, const void * vertices, const size_t vcount)
{
	size_t i;
	assert(this->vertices_);
	assert(index < this->vertices_->size + 1);

	this->state_ |= STATUS_DIRTY;

	for (i = 0; i < this->indices_->size; ++i)
	{
		if (*(GLuint *)(vector_get( this->indices_, i)) > index)
		{
			*(GLuint *)(vector_get( this->indices_, i)) += index;
		}
	}

	vector_insert_data(this->vertices_, index, vertices, vcount);
}

void VertexBuffer::erase_indices(const size_t first, const size_t last)
{
	assert(this->indices_);
	assert(first < this->indices_->size);
	assert((last) <= this->indices_->size);

	this->state_ |= STATUS_DIRTY;
	vector_erase_range(this->indices_, first, last);
}

void VertexBuffer::erase_vertices(const size_t first, const size_t last)
{
	size_t i;
	assert(this->vertices_);
	assert(first < this->vertices_->size);
	assert(last <= this->vertices_->size);
	assert(last > first);

	this->state_ |= STATUS_DIRTY;
	for (i = 0; i < this->indices_->size; ++i)
	{
		if (*(GLuint *)(vector_get(this->indices_, i)) > first)
		{
			*(GLuint *)(vector_get(this->indices_, i)) -= (last - first);
		}
	}
	vector_erase_range(this->vertices_, first, last);
}
