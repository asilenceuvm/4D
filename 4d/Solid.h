#pragma once

#include <Gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <memory>

#include "Shader.h"
#include "ShaderBuilder.h"
#include "MathUtil.h"
#include "InputManager.h"

#define GET_VARIABLE_NAME(Variable) (#Variable)

class Solid {
public:
    Solid(std::string sdf) {
		float vertices[] = {
			-1, -1, -1, 
			 1, -1, -1, 
			 1,  1, -1,  
			 1,  1, -1,  
			-1,  1, -1,  
			-1, -1, -1,  
		};
		
		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);
		glGenBuffers(1, &this->VBO);  
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);  
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(0);

		std::string name = "";
		for (int i = 0; i < ShaderBuilder::shapes.size(); ++i) {
			bool exists = sdf.find(ShaderBuilder::shapeNames[i]) != std::string::npos;
			if (exists) {
				name += ShaderBuilder::shapeNames[i];
				if (i != ShaderBuilder::shapes.size() - 1) {
					name += "/";
				}
			}
		}
		shader = std::make_unique<Shader>(ShaderBuilder::getDefaultVertexShader(), ShaderBuilder::generateFragmentShader(sdf), name);

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
        glBindVertexArray(this->VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void update() {
		shader->use();
        const float cameraSpeed = 0.05f;  
        if (InputManager::keys[GLFW_KEY_W] == GLFW_PRESS) { cameraPos += cameraSpeed * cameraFront;
        } 
        if (InputManager::keys[GLFW_KEY_S] == GLFW_PRESS) {
            cameraPos -= cameraSpeed * cameraFront;
        }
        if (InputManager::keys[GLFW_KEY_A] == GLFW_PRESS) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (InputManager::keys[GLFW_KEY_D] == GLFW_PRESS) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (InputManager::keys[GLFW_KEY_I] == GLFW_PRESS) {
            x += 0.001;
        }
        if (InputManager::keys[GLFW_KEY_K] == GLFW_PRESS) {
            x -= 0.001;
        }

        view = glm::lookAt(cameraPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        shader->setMat4("viewToWorld", view);

        shader->setVec3("cameraPos", cameraPos);
        shader->setMat4("model", model);
        shader->setVec4("transform", transform);
        shader->setFloat("time", glfwGetTime());
        shader->setFloat("wPos", x);
	}

	void rotate(float rot, glm::vec3 u, glm::vec3 w) {
        MathUtil::rotate4D(model, rot, u, w);
	}

	void translate(glm::vec4 delta) {
		transform = delta;
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

	glm::vec4 transform = glm::vec4(0, 0, 0, 0);
	float x = 0;

};
