#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // ������� ��� ���������

uniform mat4 uM_m;
uniform mat4 uV_m;
uniform mat4 uP_m;
uniform vec3 lightPos; // ������� ��������� �����

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;

void main()
{
    FragPos = vec3(uM_m * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(uM_m))) * aNormal; // �������������� ��������
    LightPos = vec3(uV_m * vec4(lightPos, 1.0)); // ������� ����� � ������������ ����

    gl_Position = uP_m * uV_m * vec4(FragPos, 1.0);
}
