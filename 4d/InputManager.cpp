#include "InputManager.h"

#include <cmath>
#include <iostream>

bool InputManager::keys[350];

void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	keys[key] = action;
}


double InputManager::lastX = 0;
double InputManager::lastY = 0;
double InputManager::xoffset = 0;
double InputManager::yoffset = 0;
double InputManager::xPos = 0;
double InputManager::yPos = 0;
bool InputManager::firstMouse = true;

void InputManager::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    xoffset = xpos - lastX;
    yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xPos = xpos;
    yPos = ypos;
}


bool InputManager::rightMouseDown = false;
bool InputManager::leftMouseDown = false;

void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        rightMouseDown = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        leftMouseDown = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        rightMouseDown = false;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        leftMouseDown = false;
    }
}

double InputManager::mouseScroll = 0;
void InputManager::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    mouseScroll += yoffset;
}


int InputManager::windowWidth = 1920;
int InputManager::windowHeight = 1080;
void InputManager::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

