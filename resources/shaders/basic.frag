#version 430 core
out vec4 FragColor; // Цвет фрагмента, который будет выводиться на экран

uniform vec4 uColor; // Униформа для цвета модели

void main()
{
    FragColor = uColor; // Установка цвета каждого фрагмента в униформу uColor
}
