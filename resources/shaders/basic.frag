#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

uniform vec3 objectColor; // ���� �������
uniform vec3 lightColor;  // ���� �����
uniform float transparency; // ������������

void main()
{
    // ������� ��������� ����
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (diffuse + vec3(0.1)) * objectColor; // ��������� ������� ����������� �����
    FragColor = vec4(result, transparency); // ������������� ���� ��������� � ������ ������������
}
