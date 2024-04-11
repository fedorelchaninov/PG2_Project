#version 430 core
layout (location = 0) in vec3 aPos; // Позиция вершины
layout (location = 1) in vec3 aNormal; // Нормаль вершины (не будет использоваться)
layout (location = 2) in vec2 aTexCoords; // Текстурные координаты вершины (не будет использоваться)

uniform mat4 uP_m; // Матрица проекции
uniform mat4 uV_m; // Матрица вида
uniform mat4 uM_m; // Матрица модели

void main()
{
    gl_Position = uP_m * uV_m * uM_m * vec4(aPos, 1.0); // Трансформация вершины с учетом матриц проекции, вида и модели
}
