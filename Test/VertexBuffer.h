#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__

#include <glbinding/gl/types.h>

#include "vector.h"
#include "VertexAttribute.h"

class VertexBuffer
{
    
public:

	enum VERTEX_BUFFER_STATUS
	{
		STATUS_CLEAN  = 0,
		STATUS_DIRTY  = 1,
		STATUS_FROZEN = 2
	};

 /* Creates an empty vertex buffer. 
   @param  format a string describing vertex format.*/
	explicit VertexBuffer( const char *format );
	~VertexBuffer( );

 /*  Returns the number of items in the vertex buffer*/
  size_t size();


 /*  Returns vertex format*/
  const char * format();


 /* Print information about a vertex buffer*/
  void print();

/**
 * Render vertex buffer.
 * @param  mode  render mode
 */
  void render (gl::GLenum mode );

/**
 * Render a specified item from the vertex buffer.
 * @param  index index of the item to be rendered
 */
  void render_item ( size_t index );


  /* Clear all items.*/
  void clear();

/**
 * Append a new item to the collection.
 * @param  vcount   number of vertices
 * @param  vertices raw vertices data
 * @param  icount   number of indices
 * @param  indices  raw indices data
 */
  size_t push_back( const void * vertices, const size_t vcount, const gl::GLuint * indices, const size_t icount );


/**
 * Insert a new item into the vertex buffer.
 * @param  index     location before which to insert item
 * @param  vertices  raw vertices data
 * @param  vcount    number of vertices
 * @param  indices   raw indices data
 * @param  icount    number of indices
 */
  size_t insert( const size_t index, const void * vertices,  const size_t vcount, const gl::GLuint * indices, const size_t icount );

/**
 * Erase an item from the vertex buffer.
 * @param  index    index of the item to be deleted
 */
  void erase( const size_t index );

private:

	/**
	* Upload buffer to GPU memory.
	*/
	void upload();


   /* Prepare vertex buffer for render.
 * @param  mode  render mode
 */
  void render_setup (gl::GLenum mode );

/**
 * Finish rendering by setting back modified states
 */
  void render_finish ();
  /**
 * Appends indices at the end of the buffer.
 * @param  indices  indices to be appended
 * @param  icount   number of indices to be appended
 *
 * @private
 */
  void push_back_indices ( const gl::GLuint * indices, const size_t icount );


/**
 * Appends vertices at the end of the buffer.
 *
 * @note Internal use
 * @param  self     a vertex buffer
 * @param  vertices vertices to be appended
 * @param  vcount   number of vertices to be appended
 *
 * @private
 */
  void push_back_vertices ( const void * vertices, const size_t vcount );


/**
 * Insert indices in the buffer.
 * @param  index   location before which to insert indices
 * @param  indices indices to be appended
 * @param  icount  number of indices to be appended
 *
 * @private
 */
  void insert_indices ( const size_t index, const gl::GLuint *indices, const size_t icount );


/**
 * Insert vertices in the buffer.
 * @param  index    location before which to insert vertices
 * @param  vertices vertices to be appended
 * @param  vcount   number of vertices to be appended
 *
 * @private
 */
  void insert_vertices ( const size_t index, const void *vertices, const size_t vcount );

/**
 * Erase indices in the buffer.
 * @param  first  the index of the first index to be erased
 * @param  last   the index of the last index to be erased
 *
 * @private
 */
  void erase_indices ( const size_t first, const size_t last );

/**
 * Erase vertices in the buffer.
 * @param  first  the index of the first vertex to be erased
 * @param  last   the index of the last vertex to be erased
 *
 * @private
 */
  void erase_vertices ( const size_t first, const size_t last );


private:
    /** Format of the vertex buffer. */
    char * format_;

    /** Vector of vertices. */
    vector_t * vertices_;

    /** GL identity of the Vertex Array Object */
	gl::GLuint VAO_id_;

    /** GL identity of the vertices buffer. */
	gl::GLuint vertices_id_;

    /** Vector of indices. */
	vector_t * indices_;

    /** GL identity of the indices buffer. */
	gl::GLuint indices_id_;

    /** Current size of the vertices buffer in GPU */
    size_t GPU_vsize_;

    /** Current size of the indices buffer in GPU*/
    size_t GPU_isize_;

    /** GL primitives to render. */
	gl::GLenum mode_;

    /** Whether the vertex buffer needs to be uploaded to GPU memory. */
    char state_;

    /** Individual items */
    //存放的是glm::ivec4;其四个元素含义为:
	//x--vstart
	//y--vcount
	//z--istart
	//w--icount
	vector_t * items_;
	


    /** Array of attributes. */
    VertexAttribute* attributes_[MAX_VERTEX_ATTRIBUTE];
};

#endif