#version 330 core

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPSILON = 0.0001;

out vec4 fragColor;
uniform mat4 model;
uniform mat4 viewToWorld;
uniform vec4 backgroundColor;
in vec2 fragCoord;
in vec4 pos;

uniform float wPos;

float sphereSDF(vec4 samplePoint) {
    return length(samplePoint) - 1.0;
}

float hypercubeSDF(vec4 samplePoint, float size) {
    vec4 d = abs(samplePoint) - size;
    return min(max(d.x, max(d.y, max(d.z, d.w))), 0.0) + length(max(d, 0.0));
}

float sceneSDF(vec4 samplePoint) {
    samplePoint.xyw = (model * vec4(samplePoint.xyz, 1)).xyz;
    //return sphereSDF(samplePoint);
    return hypercubeSDF(samplePoint, 1);
}

float shortestDistanceToSurface(vec4 eye, vec4 marchingDirection, float start, float end) {
    float depth = start;
    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = sceneSDF(eye + depth * marchingDirection);
        if (dist < EPSILON) {
			return depth;
        }
        depth += dist;
        if (depth >= end) {
            return end;
        }
    }
    return end;
}
            

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 xy = fragCoord - size / 2.0;
    float z = size.y * 0.25 / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(xy, -z));
}

vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec4(p.x + EPSILON, p.y, p.z, 0.0)) - sceneSDF(vec4(p.x - EPSILON, p.y, p.z, 0.0)),
        sceneSDF(vec4(p.x, p.y + EPSILON, p.z, 0.0)) - sceneSDF(vec4(p.x, p.y - EPSILON, p.z, 0.0)),
        sceneSDF(vec4(p.x, p.y, p.z  + EPSILON, 0.0)) - sceneSDF(vec4(p.x, p.y, p.z - EPSILON, 0.0))
    ));
}

vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity) {
    vec3 N = estimateNormal(p);
    vec3 L = normalize(lightPos - p);
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));
    
    float dotLN = dot(L, N);
    float dotRV = dot(R, V);
    
    if (dotLN < 0.0) {
        // Light not visible from this point on the surface
        return vec3(0.0, 0.0, 0.0);
    } 
    
    if (dotRV < 0.0) {
        // Light reflection in opposite direction as viewer, apply only diffuse
        // component
        return lightIntensity * (k_d * dotLN);
    }
    return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;
    
    vec3 light1Pos = vec3(4.0 * sin(0),
                          2.0,
                          4.0 * cos(0));
    vec3 light1Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light1Pos,
                                  light1Intensity);
    
    vec3 light2Pos = vec3(2.0 * sin(0.37 * 0),
                          2.0 * cos(0.37 * 0),
                          2.0);
    vec3 light2Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light2Pos,
                                  light2Intensity);    
    return color;
}

void main() {
	vec3 dir = rayDirection(45.0, vec2(800, 600), gl_FragCoord.xy);
    
    vec4 eye = pos;
    vec4 worldDir = (viewToWorld * vec4(dir, 0.0));
    float dist = shortestDistanceToSurface(eye, vec4(worldDir.xyz, wPos), MIN_DIST, MAX_DIST);
    
    //float dist = shortestDistanceToSurface(eye, vec4(dir, wPos), MIN_DIST, MAX_DIST);
    
    if (dist > MAX_DIST - EPSILON) {
        // Didn't hit anything
        fragColor = backgroundColor;
		return;
    }

    vec3 p = eye.xyz + dist * dir;
    
    vec3 K_a = vec3(0.2, 0.2, 0.2);
    vec3 K_d = vec3(0.7, 0.2, 0.2);
    vec3 K_s = vec3(1.0, 1.0, 1.0);
    float shininess = 10.0;
    
    vec3 color = phongIllumination(K_a, K_d, K_s, shininess, p.xyz, eye.xyz);
    
    fragColor = vec4(color, 1.0);
    
    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}