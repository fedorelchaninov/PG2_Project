#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

uniform vec3 objectColor; // Цвет объекта
uniform vec3 lightColor;  // Цвет света
uniform float transparency; // Прозрачность

void main()
{
    // Простой диффузный свет
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (diffuse + vec3(0.1)) * objectColor; // Добавляем немного окружающего света
    FragColor = vec4(result, transparency); // Устанавливаем цвет фрагмента с учетом прозрачности
}
