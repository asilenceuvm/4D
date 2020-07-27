#version 330 core

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPSILON = 0.0001;

uniform float time;
uniform float wPos;
uniform mat4 viewToWorld;
uniform mat4 model;
uniform vec4 transform;
uniform vec3 cameraPos;

out vec4 fragColor;


float hypercubeSDF(vec4 samplePoint, float size) {
    vec4 d = abs(samplePoint) - size;
    return min(max(d.x, max(d.y, max(d.z, d.w))), 0.0) + length(max(d, 0.0));
}


float sceneSDF(vec4 samplePoint) {
    vec4 drawPoint = model * (samplePoint + transform);

    /*float c = 1.5;
	vec4 q = mod(drawPoint + 0.5 * c, c) - 0.5 * c;
    return hypercubeSDF(q, 0.2);*/

    //return hypersphereSDF(samplePoint);
    return hypercubeSDF(drawPoint, 1);
    //return max(hypercubeSDF(drawPoint, 0.8), sixteenCellSDF(drawPoint, 1I));
    //return sixteenCellSDF(samplePoint, 1);
}


float shortestDistanceToSurface(vec3 cameraPos, vec3 marchingDirection, float start, float end) {
    float depth = start;
    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = sceneSDF(vec4(cameraPos + depth * marchingDirection, wPos));
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
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(xy, -z));
}

vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec4(p.x + EPSILON, p.y, p.z, wPos)) - sceneSDF(vec4(p.x - EPSILON, p.y, p.z, wPos)),
        sceneSDF(vec4(p.x, p.y + EPSILON, p.z, wPos)) - sceneSDF(vec4(p.x, p.y - EPSILON, p.z, wPos)),
        sceneSDF(vec4(p.x, p.y, p.z  + EPSILON, wPos)) - sceneSDF(vec4(p.x, p.y, p.z - EPSILON, wPos))
    ));
}

vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 cameraPos,
                          vec3 lightPos, vec3 lightIntensity) {
    vec3 N = estimateNormal(p);
    vec3 L = normalize(lightPos - p);
    vec3 V = normalize(cameraPos - p);
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

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 cameraPos) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;
    
    vec3 light1Pos = vec3(8.0, 2.0, 4.0);
    vec3 light1Intensity = vec3(0.5, 0.5, 0.5);
    
    color += phongContribForLight(k_d, k_s, alpha, p, cameraPos,
                                  light1Pos,
                                  light1Intensity);
    
    return color;
}

mat4 viewMatrix(vec3 cameraPos, vec3 center, vec3 up) {
    vec3 f = normalize(center - cameraPos);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    return mat4(
        vec4(s, 0.0),
        vec4(u, 0.0),
        vec4(-f, 0.0),
        vec4(0.0, 0.0, 0.0, 1)
    );
}

void main() {
	vec3 viewDir = rayDirection(45.0, vec2(800, 600), gl_FragCoord.xy);
    vec3 eye = vec3(8 * sin(time), 5 , 8 * cos(time));

    mat4 worldView = viewMatrix(cameraPos, vec3(0,0,0), vec3(0,1,0));
    vec3 worldDir = (worldView  * vec4(viewDir, 0.0)).xyz;
    float dist = shortestDistanceToSurface(cameraPos, worldDir, MIN_DIST, MAX_DIST);
    
    if (dist > MAX_DIST - EPSILON) {
        fragColor = vec4(0.0, 0.0, 0.0, 0.0);
		return;
    }
    
    vec3 p = cameraPos + dist * worldDir;
    
    vec3 K_a = vec3(0.2, 0.2, 0.2);
    vec3 K_d = vec3(0.7, 0.2, 0.2);
    vec3 K_s = vec3(1.0, 1.0, 1.0);
    float shininess = 10.0;
    
    vec3 color = phongIllumination(K_a, K_d, K_s, shininess, p, cameraPos);
    
    fragColor = vec4(color, 1);
}
