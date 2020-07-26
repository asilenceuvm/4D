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

double lastX = 400, lastY = 300;
double yaw, pitch;
GLFWwindow* window;
Solid* solid;

void GLAPIENTRY MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
	const GLchar* log = (stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
	if (type == GL_DEBUG_TYPE_ERROR) {
		Logger::logError("GL_ERROR", log);
	}
	else {
		Logger::logWarning("GL_WARNING", log);
	}
}



void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    //cameraFront = glm::normalize(direction);
}

void update() {
	solid->update(window);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	glfwPollEvents();    
}

void render() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		Logger::logError("Starting opengl context");
        glfwTerminate();
        return -1;
	}
	else {
		Logger::logSuccess("Starting opengl context");
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

	const GLubyte* log = (stderr, "Graphics context: %s\n", (glGetString(GL_RENDERER)));
	Logger::logMessage("OPENGL", log);

    glfwSetCursorPosCallback(window, mouseCallback);  
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

	solid = new Solid();
	run(1.0/120.0);
	delete solid;

    glfwTerminate();
	return 0;
}

