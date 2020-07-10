#include <iostream>

#include <Gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Shader.h"


void GLAPIENTRY MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
    std::cout << (stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message) << std::endl;
}

glm::mat4 viewMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
    // Based on gluLookAt man page
    glm::vec3 f = normalize(center - eye);
    glm::vec3 s = normalize(cross(f, up));
    glm::vec3 u = cross(s, f);
    return glm::mat4(
        glm::vec4(s, 0.0),
        glm::vec4(u, 0.0),
        glm::vec4(-f, 0.0),
        glm::vec4(0.0, 0.0, 0.0, 1)
    );
}

float x = 0;

double lastX = 400, lastY = 300;
double yaw, pitch;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
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

int main() {
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glViewport(0, 0, 800, 600);
    
    glEnable(GL_DEPTH_TEST);  

    glewInit();

    glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

    glfwSetCursorPosCallback(window, mouseCallback);  
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

    float vertices[] = {
		-1, -1, -1, 0.0f,
		 1, -1, -1, 0.0f,
		 1,  1, -1, 0.0f, 
		 1,  1, -1, 0.0f, 
		-1,  1, -1, 0.0f, 
		-1, -1, -1, 0.0f, 
    };
    
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);  
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);

    Shader shader("shaders/basic.vs", "shaders/basic.fs");
    shader.use();

    glm::mat4 model = glm::mat4(1);
    model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    shader.setMat4("model", model);

    glm::mat4 view = glm::mat4(1);
	glm::vec3 cameraPos = glm::vec3(8.0f, 0.0f, 7.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
    shader.setMat4("view", view);
    std::cout << glm::to_string(viewMatrix(glm::vec3(2,0,7), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))) << std::endl;
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    std::cout << glm::to_string(view) << std::endl;
    std::cout << glm::to_string(view - viewMatrix(glm::vec3(2,0,7), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))) << std::endl;
    shader.setMat4("viewToWorld", view);
    shader.setVec3("cameraPos", cameraPos);

    glm::mat4 projection = glm::mat4(1);
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        shader.setFloat("time", glfwGetTime());

        const float cameraSpeed = 0.05f; // adjust accordingly
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPos += cameraSpeed * cameraFront;
        } 
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPos -= cameraSpeed * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
            x += 0.001;
        }
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
            x -= 0.001;
        }
        //std::cout << glm::to_string(cameraPos) << std::endl;
        //cameraPos.x += 0.01;
		//view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setFloat("wPos", x);
        //cameraPos = glm::vec3(8 * sin(glfwGetTime()), 5, 8 * cos(glfwGetTime()));
        view = glm::lookAt(cameraPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        //view = glm::translate(view, glm::vec3(0,0,cameraPos.x));
        shader.setMat4("viewToWorld", view);
        //shader.setMat4("view", view);
        shader.setVec3("cameraPos", cameraPos);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
	return 0;
}
