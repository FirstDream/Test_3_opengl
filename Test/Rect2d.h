
#pragma once

#include <glbinding/gl/types.h>

#include <chrono>

#include <glm/glm.hpp>
#include <glm/fwd.hpp>

#include <opencv2\opencv.hpp>

class GLSLShader;
class VertexBuffer;

class Rect2d
{
public:
	explicit Rect2d();
	~Rect2d();

	void resize(int width, int height);
	void draw( glm::mat4& MVP );

	void updateTexture(cv::Mat& data);

	void editWindowColor(glm::dvec2 delta);

protected:


	gl::GLuint texture_id_;

	VertexBuffer* vertex_buffer_;

	GLSLShader* shader_;

	int w_;
	int h_;

	int max_val_;
	int min_val_;

	//0--center;
	//1--width;
	glm::vec2 window_color_;
	glm::vec2 window_color_normal_;

};
