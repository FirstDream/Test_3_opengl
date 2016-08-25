
//#include <QSurfaceFormat>
//#include <QWidget>
//#include <QApplication>


#include <iostream>

//#include <opencv2\opencv.hpp>
//#include <opencv2\core\utility.hpp>
//#include <opencv2\viz.hpp>

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


using namespace gl;
using namespace glbinding;


#include <Rect2d.h>
#include "VertexBuffer.h"
#include "GLSLShader.h"

VertexBuffer* cube;
glm::mat4 model, view, projection;
GLSLShader shader;

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

	projection = glm::perspective(45.0f, static_cast<GLfloat>(width) / static_cast<GLfloat>(height), 2.0f, 10.0f);
}


void init()
{
	std::string vert_file = "./OpenGL/Shaders/cube.vert";
	std::string frag_file = "./OpenGL/Shaders/cube.frag";
	shader.LoadFromFile(GL_VERTEX_SHADER, vert_file);
	shader.LoadFromFile(GL_FRAGMENT_SHADER, frag_file);
	shader.CreateAndLinkProgram();
	//shader.Use();
	//{
	//
	//
	//}
	//shader.UnUse();

	typedef struct { float x, y, z; } xyz;
	typedef struct { float r, g, b, a; } rgba;
	typedef struct { xyz position, normal; rgba color; } vertex;
	xyz v[] = { { 1, 1, 1 },{ -1, 1, 1 },{ -1,-1, 1 },{ 1,-1, 1 },
	{ 1,-1,-1 },{ 1, 1,-1 },{ -1, 1,-1 },{ -1,-1,-1 } };
	xyz n[] = { { 0, 0, 1 },{ 1, 0, 0 },{ 0, 1, 0 } ,
	{ -1, 0, 1 },{ 0,-1, 0 },{ 0, 0,-1 } };
	rgba c[] = { { 1, 1, 1, 1 },{ 1, 1, 0, 1 },{ 1, 0, 1, 1 },{ 0, 1, 1, 1 },
	{ 1, 0, 0, 1 },{ 0, 0, 1, 1 },{ 0, 1, 0, 1 },{ 0, 0, 0, 1 } };
	vertex vertices[24] = {
		{ v[0],n[0],c[0] },{ v[1],n[0],c[1] },{ v[2],n[0],c[2] },{ v[3],n[0],c[3] },
		{ v[0],n[1],c[0] },{ v[3],n[1],c[3] },{ v[4],n[1],c[4] },{ v[5],n[1],c[5] },
		{ v[0],n[2],c[0] },{ v[5],n[2],c[5] },{ v[6],n[2],c[6] },{ v[1],n[2],c[1] },
		{ v[1],n[3],c[1] },{ v[6],n[3],c[6] },{ v[7],n[3],c[7] },{ v[2],n[3],c[2] },
		{ v[7],n[4],c[7] },{ v[4],n[4],c[4] },{ v[3],n[4],c[3] },{ v[2],n[4],c[2] },
		{ v[4],n[5],c[4] },{ v[7],n[5],c[7] },{ v[6],n[5],c[6] },{ v[5],n[5],c[5] } };
	GLuint indices[24] = { 0, 1, 2, 3,    4, 5, 6, 7,   8, 9,10,11,
		12,13,14,15,  16,17,18,19,  20,21,22,23 };

	cube =  new VertexBuffer("vertex:3f,normal:3f,color:4f");
	cube->push_back(vertices, 24, indices, 24);
	


	projection = glm::mat4(1.0);
	model = glm::mat4(1.0);
	view = glm::mat4(1.0);

	glPolygonOffset(1, 1);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);

	cube->print();
}

void display(GLFWwindow* window)
{
	static float theta = 0, phi = 0;
	static GLuint Color = 0;
	double seconds_elapsed = glfwGetTime();

	if (!Color)
	{
		Color = shader("Color");
	}
	assert(glGetError() == GL_NO_ERROR);
	theta = .5f * seconds_elapsed / 0.016f;
	phi = .5f * seconds_elapsed / 0.016f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model = glm::mat4(1);
	model = glm::rotate(model, theta, glm::vec3(0, 0, 1));
	model = glm::rotate(model, phi, glm::vec3(0, 1, 0));
	//model = glm::translate(model, glm::vec3(0.0, 0.0, -5.0) );

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);

	shader.Use();
	{
		glUniformMatrix4fv(shader("model"),		 1, 0, glm::value_ptr(model));
		glUniformMatrix4fv(shader("view"),		 1, 0, glm::value_ptr(view));
		glUniformMatrix4fv(shader("projection"), 1, 0, glm::value_ptr(projection));
	}

	glUniform4f(Color, 1, 1, 1, 1);
	cube->render(GL_QUADS);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_BLEND);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDepthMask(GL_FALSE);

	glUniform4f(Color, 0, 0, 0, .5);
	cube->render( GL_QUADS);

	shader.UnUse();

	glDepthMask(GL_TRUE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

	GLFWwindow * window = glfwCreateWindow(640, 480, "", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, resize);
	glfwSetWindowRefreshCallback(window, display);

	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1);

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

	delete cube;

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;

}
