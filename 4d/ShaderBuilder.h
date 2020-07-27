#pragma once

#include <array>
#include <vector>
#include <map>

#include "Logger.h"

namespace ShaderBuilder {
	std::string hypercubeSDF =
		"float hypercubeSDF(vec4 samplePoint, float size) {\n"
		"vec4 d = abs(samplePoint) - size;\n"
		"return min(max(d.x, max(d.y, max(d.z, d.w))), 0.0) + length(max(d, 0.0));\n"
		"}\n";

	std::string sixteenCellSDF =
		"float sixteenCellSDF(vec4 p, float s) {\n"
		"p = abs(p);\n"
		"return (p.x + p.y + p.z + p.w - s) * 0.57735027f;\n"
		"}\n";

	std::string hypersphereSDF =
		"float hypersphereSDF(vec4 p, float radius) {\n"
		"return length(p) - radius;\n"
		"}\n";

	std::array<std::string, 3> shapes = { hypercubeSDF, sixteenCellSDF, hypersphereSDF };

	std::string sceneSDFHeader =
		"float sceneSDF(vec4 samplePoint) {\n"
		"vec4 drawPoint = model * (samplePoint + transform);\n"
		"\n";

	std::string header =
		"#version 330 core\n"
		"const int MAX_MARCHING_STEPS = 255;\n"
		"const float MIN_DIST = 0.0;\n"
		"const float MAX_DIST = 100.0;\n"
		"const float EPSILON = 0.0001;\n"
		"uniform float time;\n"
		"uniform float wPos;\n"
		"uniform mat4 viewToWorld;\n"
		"uniform mat4 model;\n"
		"uniform vec4 transform;\n"
		"uniform vec3 cameraPos;\n"
		"out vec4 fragColor;\n";

	std::string footer =
		"float shortestDistanceToSurface(vec3 cameraPos, vec3 marchingDirection, float start, float end) {\n"
		"float depth = start;\n"
		"for (int i = 0; i < MAX_MARCHING_STEPS; i++) {\n"
		"float dist = sceneSDF(vec4(cameraPos + depth * marchingDirection, wPos));\n"
		"if (dist < EPSILON) {\n"
		"return depth;\n"
		"}\n"
		"depth += dist;\n"
		"if (depth >= end) {\n"
		"return end;\n"
		"}\n"
		"}\n"
		"return end;\n"
		"}\n"
		"vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {\n"
		"vec2 xy = fragCoord - size / 2.0;\n"
		"float z = size.y / tan(radians(fieldOfView) / 2.0);\n"
		"return normalize(vec3(xy, -z));\n"
		"}\n"
		"vec3 estimateNormal(vec3 p) {\n"
		"return normalize(vec3(\n"
		"sceneSDF(vec4(p.x + EPSILON, p.y, p.z, wPos)) - sceneSDF(vec4(p.x - EPSILON, p.y, p.z, wPos)),\n"
		"sceneSDF(vec4(p.x, p.y + EPSILON, p.z, wPos)) - sceneSDF(vec4(p.x, p.y - EPSILON, p.z, wPos)),\n"
		"sceneSDF(vec4(p.x, p.y, p.z  + EPSILON, wPos)) - sceneSDF(vec4(p.x, p.y, p.z - EPSILON, wPos))\n"
		"));\n"
		"}\n"
		"vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 cameraPos,\n"
		"		  vec3 lightPos, vec3 lightIntensity) {\n"
		"vec3 N = estimateNormal(p);\n"
		"vec3 L = normalize(lightPos - p);\n"
		"vec3 V = normalize(cameraPos - p);\n"
		"vec3 R = normalize(reflect(-L, N));\n"
		"float dotLN = dot(L, N);\n"
		"float dotRV = dot(R, V);\n"
		"if (dotLN < 0.0) {\n"
		"// Light not visible from this point on the surface\n"
		"return vec3(0.0, 0.0, 0.0);\n"
		"} \n"
		"if (dotRV < 0.0) {\n"
		"// Light reflection in opposite direction as viewer, apply only diffuse\n"
		"// component\n"
		"return lightIntensity * (k_d * dotLN);\n"
		"}\n"
		"return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));\n"
		"}\n"
		"vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 cameraPos) {\n"
		"const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);\n"
		"vec3 color = ambientLight * k_a;\n"
		"vec3 light1Pos = vec3(8.0, 2.0, 4.0);\n"
		"vec3 light1Intensity = vec3(0.5, 0.5, 0.5);\n"
		"color += phongContribForLight(k_d, k_s, alpha, p, cameraPos,\n"
		"				  light1Pos,\n"
		"				  light1Intensity);\n"
		"return color;\n"
		"}\n"
		"mat4 viewMatrix(vec3 cameraPos, vec3 center, vec3 up) {\n"
		"vec3 f = normalize(center - cameraPos);\n"
		"vec3 s = normalize(cross(f, up));\n"
		"vec3 u = cross(s, f);\n"
		"return mat4(\n"
		"vec4(s, 0.0),\n"
		"vec4(u, 0.0),\n"
		"vec4(-f, 0.0),\n"
		"vec4(0.0, 0.0, 0.0, 1)\n"
		");\n"
		"}\n"
		"void main() {\n"
		"vec3 viewDir = rayDirection(45.0, vec2(800, 600), gl_FragCoord.xy);\n"
		"vec3 eye = vec3(8 * sin(time), 5 , 8 * cos(time));\n"
		"mat4 worldView = viewMatrix(cameraPos, vec3(0,0,0), vec3(0,1,0));\n"
		"vec3 worldDir = (worldView  * vec4(viewDir, 0.0)).xyz;\n"
		"float dist = shortestDistanceToSurface(cameraPos, worldDir, MIN_DIST, MAX_DIST);\n"
		"if (dist > MAX_DIST - EPSILON) {\n"
		"fragColor = vec4(0.0, 0.0, 0.0, 0.0);\n"
		"return;\n"
		"}\n"
		"vec3 p = cameraPos + dist * worldDir;\n"
		"vec3 K_a = vec3(0.2, 0.2, 0.2);\n"
		"vec3 K_d = vec3(0.7, 0.2, 0.2);\n"
		"vec3 K_s = vec3(1.0, 1.0, 1.0);\n"
		"float shininess = 10.0;\n"
		"vec3 color = phongIllumination(K_a, K_d, K_s, shininess, p, cameraPos);\n"
		"fragColor = vec4(color, 1);\n"
		"}\n";

	static const int hypercube = 0;
	static const int sixteenCell = 1;
	static const int hypersphere = 2;

	inline std::string generateShader(int shapeSDF, float size) {
		std::string shader = header;

		//add shape sdfs 
		shader += shapes[shapeSDF];

		//make sceneSDF function
		shader += sceneSDFHeader;

		//TODO: add logic to intersections, unions and subtractions
		switch (shapeSDF) {
		case hypercube:
			shader += "return hypercubeSDF(drawPoint, " + std::to_string(size) + ");";
			break;
		case sixteenCell:
			shader += "return sixteenCellSDF(drawPoint, " + std::to_string(size) + ");";
			break;
		default:
			shader += "return hypersphereSDF(drawPoint, " + std::to_string(size) + ");";
		}

		shader += "\n}\n";
		
		shader += footer;

		return shader;
	}

	inline std::string getDefaultVertexShader() {
		return
			"#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"void main() {\n"
			"gl_Position = vec4(aPos, 1.0); \n"
			"}\n";
	}
};
