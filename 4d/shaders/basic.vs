#version 330 core
layout (location = 0) in vec4 aPos; // the position variable has attribute position 0

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 cameraPos;

out vec2 fragCoord;
out vec4 pos;

void main() {
    gl_Position = projection * view * vec4(aPos.xyz, 1.0); 
    //gl_Position = vec4(aPos.xyz, 1.0); 
    fragCoord = aPos.xy;
    pos = cameraPos;
}