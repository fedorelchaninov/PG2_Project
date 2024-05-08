#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;  // normals for lighting
layout (location = 2) in vec2 aTexCoords; // tex coords

uniform mat4 uM_m;
uniform mat4 uV_m;
uniform mat4 uP_m;
uniform vec3 lightPos; // light source pos

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;
out vec2 TexCoords; // tex coords to frag shader

void main()
{
    FragPos = vec3(uM_m * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(uM_m))) * aNormal;  // normals transpose
    LightPos = vec3(uV_m * vec4(lightPos, 1.0)); // light pos
    TexCoords = aTexCoords;

    gl_Position = uP_m * uV_m * vec4(FragPos, 1.0);
}
