#include <iostream>

#include "Gl/glew.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Eigen/Dense>

#include "Shader.h"


void GLAPIENTRY MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
    std::cout << (stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message) << std::endl;
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

    float vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 
     0.5f, -0.5f, -0.5f, 0.0f, 
     0.5f,  0.5f, -0.5f, 0.0f, 
     0.5f,  0.5f, -0.5f, 0.0f, 
    -0.5f,  0.5f, -0.5f, 0.0f, 
    -0.5f, -0.5f, -0.5f, 0.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f, 
     0.5f, -0.5f,  0.5f,  0.0f, 
     0.5f,  0.5f,  0.5f,  0.0f, 
     0.5f,  0.5f,  0.5f,  0.0f, 
    -0.5f,  0.5f,  0.5f,  0.0f, 
    -0.5f, -0.5f,  0.5f,  0.0f, 

    -0.5f,  0.5f,  0.5f,  0.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f, 
    -0.5f, -0.5f,  0.5f,  0.0f, 
    -0.5f,  0.5f,  0.5f,  0.0f, 

     0.5f,  0.5f,  0.5f,  0.0f, 
     0.5f,  0.5f, -0.5f,  0.0f, 
     0.5f, -0.5f, -0.5f,  0.0f, 
     0.5f, -0.5f, -0.5f,  0.0f, 
     0.5f, -0.5f,  0.5f,  0.0f, 
     0.5f,  0.5f,  0.5f,  0.0f, 

    -0.5f, -0.5f, -0.5f,  0.0f, 
     0.5f, -0.5f, -0.5f,  0.0f, 
     0.5f, -0.5f,  0.5f,  0.0f, 
     0.5f, -0.5f,  0.5f,  0.0f, 
    -0.5f, -0.5f,  0.5f,  0.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f, 

    -0.5f,  0.5f, -0.5f,  0.0f, 
     0.5f,  0.5f, -0.5f,  0.0f, 
     0.5f,  0.5f,  0.5f,  0.0f, 
     0.5f,  0.5f,  0.5f,  0.0f, 
    -0.5f,  0.5f,  0.5f,  0.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f, 
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
    shader.setVec4("vertexColor", 1.0f, 0.5f, 0.5f, 1.0f);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    shader.setMat4("projection", projection);
    glm::mat4 view = glm::mat4(1.0f);

    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 
    shader.setMat4("view", view);
    shader.setMat4("viewToWorld", view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    shader.setMat4("model", model);

    shader.setFloat("wPos", 0.0);
    float x = 0;

	glm::vec4 cameraPos = glm::vec4(0.0, 0.0, 5.0, 0.0);
    shader.setVec4("cameraPos", cameraPos);

    shader.setVec4("backgroundColor", 0.2f, 0.3f, 0.3f, 1.0f);

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader.setFloat("wPos", x);
        //x += 0.0001f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPos.z -= 0.01;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPos.z += 0.01;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPos.x -= 0.01;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPos.x += 0.01;
        }
        cameraPos.w += 0.00001;
        shader.setVec4("cameraPos", cameraPos);

		model = glm::rotate(model, glm::radians(0.1f), glm::vec3(0.0, 1.0, 0.0));
		shader.setMat4("model", model);


        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
	return 0;
}