#version 430 core
out vec4 FragColor; // ���� ���������, ������� ����� ���������� �� �����

uniform vec4 uColor; // �������� ��� ����� ������

void main()
{
    FragColor = uColor; // ��������� ����� ������� ��������� � �������� uColor
}
