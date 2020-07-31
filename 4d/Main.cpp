#include <iostream>

#include <Gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.h"
#include "MathUtil.h"
#include "Solid.h"
#include "Logger.h"
#include "InputManager.h"
#include "ShaderBuilder.h"

GLFWwindow* window;
Solid* solid;
Solid* solid2;

void GLAPIENTRY MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
	const GLchar* log = (stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
	if (type == GL_DEBUG_TYPE_ERROR) {
		Logger::logError("GL_ERROR", log);
	}
	else {
		Logger::logWarning("OPENGL", log, false);
	}
}

void update() {
	solid->rotate(0.1, glm::vec3(0, 0, 1), glm::vec3(0.001, 0, 0));
	solid->update();
	solid2->update();

	if (InputManager::keys[GLFW_KEY_ESCAPE] == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	glfwPollEvents();    
}

void render() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	solid2->render();
	solid->render();

	glfwSwapBuffers(window);
}

void run(double delta) {
	double lastTime = glfwGetTime(), timer = lastTime;
	double deltaTime = 0, nowTime = 0;
	int frames = 0, updates = 0;

	//enter loop
	while (!glfwWindowShouldClose(window)) {

		//get time
		nowTime = glfwGetTime();
		deltaTime += (nowTime - lastTime) / delta;
		lastTime = nowTime;

		//update at delta
		while (deltaTime >= 1.0) {
			update();
			updates++;
			deltaTime--;
		}

		render();
		frames++;

		//reset and output fps
		if (glfwGetTime() - timer > 1.0) {
			timer++;
			std::cout << "FPS: " << frames << " Updates:" << updates << std::endl;
			updates = 0;
			frames = 0;
		}

	}
}

int main() {
	Logger::logMessage("OPENGL", "Starting opengl context");
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
		Logger::logError("OPENGL", "Starting opengl context");
        glfwTerminate();
        return -1;
	}
	else {
		Logger::logSuccess("OPENGL", "Starting opengl context");
	}

    glfwMakeContextCurrent(window);
	//glfwSwapInterval(0);
    glViewport(0, 0, 800, 600);

    glewInit();
    glEnable(GL_DEPTH_TEST);  
    glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	glEnable(GL_BLEND);  
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthMask(false);
	//glEnable(GL_DITHER);

	std::string log = "Graphics context: ";
	log += (char*)(glGetString(GL_RENDERER));
	Logger::logInfo("OPENGL", log.c_str());

	log = "Opengl version: ";
	log += (char*)(glGetString(GL_VERSION));
	Logger::logInfo("OPENGL", log.c_str());

	glfwSetKeyCallback(window, InputManager::keyCallback);
	glfwSetCursorPosCallback(window, InputManager::mouseCallback);
	glfwSetMouseButtonCallback(window, InputManager::mouseButtonCallback);
	glfwSetScrollCallback(window, InputManager::scrollCallback);
	glfwSetFramebufferSizeCallback(window, InputManager::framebufferSizeCallback);
	
	solid = new Solid(ShaderBuilder::subtractionSDFString(ShaderBuilder::hypercubeSDFString(1),ShaderBuilder::hypersphereSDFString(1.1)));
	solid2 = new Solid(ShaderBuilder::hypercubeSDFString(0.5));
	run(1.0/120.0);
	delete solid;
	delete solid2;

    glfwTerminate();
	return 0;
}

