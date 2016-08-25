
#pragma once

#include <glbinding/gl/types.h>

#include <chrono>

#include <glm/glm.hpp>
#include <glm/fwd.hpp>

#include <opencv2\opencv.hpp>


class GLSLShader;

class Rect2d
{
public:
	Rect2d();
	~Rect2d();

	void resize(int width, int height);
	void draw();

	void updateTexture(cv::Mat& data);

protected:
	gl::GLint a_vertex;

	gl::GLuint m_vao;
	gl::GLuint m_indices;
	gl::GLuint m_vertices;
	gl::GLuint m_texture;

	GLSLShader* m_shader;

	glm::mat4 m_view;
	glm::mat4 m_projection;


	int w_;
	int h_;

};
