#pragma once

#include <Gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <memory>

#include "Shader.h"
#include "MathUtil.h"


class Solid {
public:
    Solid() {
		float vertices[] = {
			-1, -1, -1, 0.0f,
			 1, -1, -1, 0.0f,
			 1,  1, -1, 0.0f, 
			 1,  1, -1, 0.0f, 
			-1,  1, -1, 0.0f, 
			-1, -1, -1, 0.0f, 
		};
		
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);  
		glBindBuffer(GL_ARRAY_BUFFER, VBO);  
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(0);

		shader = std::make_unique<Shader>("shaders/basic.vs", "shaders/basic.fs");
		shader->use();

		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
		shader->setMat4("model", model);

		shader->setMat4("view", view);
		shader->setMat4("viewToWorld", view);
		shader->setVec3("cameraPos", cameraPos);

		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
		shader->setMat4("projection", projection);
    }

	void render() {
        shader->use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void update(GLFWwindow* window) {
        shader->setFloat("time", glfwGetTime());

        const float cameraSpeed = 0.05f;  
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
        shader->setFloat("wPos", x);
        view = glm::lookAt(cameraPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        shader->setMat4("viewToWorld", view);
        shader->setVec3("cameraPos", cameraPos);

        rotate4D(model, 0.1, glm::vec3(0, 0, 1), glm::vec3(sin(glfwGetTime()) * 0.005, cos(glfwGetTime()) * 0.005, sin(glfwGetTime()) * 0.005));
        shader->setMat4("model", model);


        shader->setVec4("transform", glm::vec4(0, 0, sin(glfwGetTime()), 0));

	}

private:
    unsigned int VBO, VAO;
	std::unique_ptr<Shader> shader;
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraPos = glm::vec3(8.0f, 0.0f, 7.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
	glm::mat4 model = glm::mat4(1);
	glm::mat4 view = glm::mat4(1);
	glm::mat4 projection = glm::mat4(1);

	float x = 0;

};
