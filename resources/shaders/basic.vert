#version 430 core
layout (location = 0) in vec3 aPos; // ������� �������
layout (location = 1) in vec3 aNormal; // ������� ������� (�� ����� ��������������)
layout (location = 2) in vec2 aTexCoords; // ���������� ���������� ������� (�� ����� ��������������)

uniform mat4 uP_m; // ������� ��������
uniform mat4 uV_m; // ������� ����
uniform mat4 uM_m; // ������� ������

void main()
{
    gl_Position = uP_m * uV_m * uM_m * vec4(aPos, 1.0); // ������������� ������� � ������ ������ ��������, ���� � ������
}
