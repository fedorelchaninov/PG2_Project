#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;
in vec2 TexCoords;

uniform vec3 lightColor;  // Цвет света
uniform vec3 viewPos;     // Позиция камеры
uniform float transparency; // Прозрачность
uniform sampler2D myTexture; // Texture sampler for the first texture

void main()
{
    // Normalization and lighting calculations
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    // Регулировка интенсивности диффузного света
    vec3 diffuse = 0.5 * diff * lightColor; // Понижаем интенсивность диффузного света

    // Регулировка окружающего света
    vec3 ambient = 0.4 * lightColor; // Снижение интенсивности окружающего света для более темного фона

    // Specular lighting adjustment
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.2 * spec * lightColor; // Уменьшение интенсивности зеркального света

    // Texture color sampling
    vec4 texColor = texture(myTexture, TexCoords);
    vec3 result = (diffuse + ambient + specular) * texColor.rgb; // Объединение освещения с цветом текстуры

    // Blending with sky color based on transparency
    float gradientFactor = clamp(FragPos.z / 500.0, 0.0, 1.0); // Adjust based on scene scale
    vec3 skyColorTop = vec3(0.4, 0.7, 1.0); // Light blue
    vec3 skyColorBottom = vec3(0.7, 0.9, 1.0); // Darker blue
    vec3 skyColor = mix(skyColorBottom, skyColorTop, gradientFactor);

    // Applying transparency and blending with sky color
    vec3 finalColor = mix(skyColor, result, transparency);

    // Use the alpha from the texture and apply transparency
    FragColor = vec4(finalColor, texColor.a * (1.0 - (1.0 - transparency) * (1.0 - transparency)));
}