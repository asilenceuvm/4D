#pragma once

#include <GLFW/glfw3.h>

class InputManager {
public:
	static bool keys[350];
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static double lastX, lastY;
	static double xoffset, yoffset;
	static double xPos, yPos;
	static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

	static bool rightMouseDown, leftMouseDown;
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	static double mouseScroll;
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	static int windowWidth, windowHeight;
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
private:
	static bool firstMouse;
};
