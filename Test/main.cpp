
//#include <QSurfaceFormat>
//#include <QWidget>
//#include <QApplication>


#include <iostream>

#include <opencv2\opencv.hpp>
#include <opencv2\core\utility.hpp>
#include <opencv2\viz.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>
#include <glbinding/callbacks.h>
#include <glbinding/Binding.h>

#include <glbinding/gl/gl.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

using namespace gl;
using namespace glbinding;


#include <Rect2d.h>
#include "VertexBuffer.h"
#include "GLSLShader.h"

Rect2d* rect_;
glm::mat4 model, view, projection;

int w = 640, h = 480;

void error(int errnum, const char * errmsg)
{
	std::cerr << errnum << ": " << errmsg << std::endl;
}

void key_callback(GLFWwindow * window, int key, int /*scancode*/, int action, int /*mods*/)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);

	bool numCubesChanged = false;

	if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		//cubescape->setNumCubes(cubescape->numCubes() + 1);
		//numCubesChanged = true;
	}

	if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		//cubescape->setNumCubes(cubescape->numCubes() - 1);
		//numCubesChanged = true;
	}

	if (numCubesChanged)
	{
		//const int n = cubescape->numCubes();
		//std::cout << "#cubes = " << n << " * " << n << " = " << n * n << std::endl;
	}
}

void resize(GLFWwindow * /*window*/, int width, int height)
{
	if ( width == 0 || height ==0 )
	{
		return;
	}
	//rect2d_->resize(width, height);

	glViewport(0, 0, width, height);

	projection = glm::perspective(45.0f, static_cast<GLfloat>(width) / static_cast<GLfloat>(height), 0.1f, 1000.0f);

	w = width;
	h = height;
	rect_->resize(w, h);
}


void init()
{
	projection = glm::mat4(1.0);
	model = glm::mat4(1.0);
	view = glm::mat4(1.0);

	glClearColor(0.0, 0.0, 0.0, 1.0);


	const char* file = "./Resources/3.dcm";
	cv::Mat im = cv::imread(file, cv::IMREAD_GRAYSCALE);
	//cv::Mat blurred;
	//cv::GaussianBlur(im, blurred, cv::Size(), 5, 5);
	rect_->updateTexture(im);

}

void display(GLFWwindow* window)
{
	double seconds_elapsed = glfwGetTime();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model = glm::mat4(1.0);
	//glm::mat4 r1  = glm::rotate(glm::mat4(1.0), glm::radians(theta), glm::vec3(0, 0, 1));
	//glm::mat4 r2  = glm::rotate(glm::mat4(1.0), glm::radians(phi), glm::vec3(0, 1, 0));
	//glm::mat4 t1  = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -5.0) );
	//model = r1* r2;
	//model = t1;
	view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));

	glm::mat4 mvp = projection * view * model;

	rect_->draw(mvp);

	glfwSwapBuffers(window);
}

int main(int argc, char * argv[])
{
    //QApplication app(argc, argv);
    //return app.exec();


	if (!glfwInit())
	{
		return 1;
	}

	glfwSetErrorCallback(error);

	glfwDefaultWindowHints();

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	GLFWwindow * window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, resize);
	glfwSetWindowRefreshCallback(window, display);
	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	setAfterCallback([](const FunctionCall &)
	{
		gl::GLenum error = glGetError();
		if (error != GL_NO_ERROR )
			std::cout << "error: " << error << std::endl;
	});

	Binding::initialize(false); // only resolve functions that are actually used (lazy)

								// print some gl infos (query)

	std::cout << std::endl
		<< "OpenGL Version:  " << ContextInfo::version() << std::endl
		<< "OpenGL Vendor:   " << ContextInfo::vendor() << std::endl
		<< "OpenGL Renderer: " << ContextInfo::renderer() << std::endl;


	rect_ = new Rect2d();

	init();

	int width, height; 

	glfwSetWindowSize(window, 640, 480);
	glfwShowWindow(window);

	glfwSetTime(0.0);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		display(window);
	}

	delete rect_;

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;

}
