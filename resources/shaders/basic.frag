#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;
in vec2 TexCoords; // Incoming texture coordinates from vertex shader

uniform vec3 objectColor; // Цвет объекта
uniform vec3 lightColor;  // Цвет света
uniform float transparency; // Прозрачность
uniform sampler2D texture1; // Texture sampler for the first texture

void main()
{
    // Lighting calculations
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Sample the texture color
    vec4 texColor = texture(texture1, TexCoords);
    vec3 result = (diffuse + vec3(0.1)) * texColor.rgb * objectColor; // Combine diffuse light with texture color and object color

    // Blend with sky color based on transparency (you may adjust or remove this part based on your needs)
    float gradientFactor = clamp(FragPos.z / 500.0, 0.0, 1.0); // Adjust based on scene scale
    vec3 skyColorTop = vec3(0.4, 0.7, 1.0); // Light blue
    vec3 skyColorBottom = vec3(0.7, 0.9, 1.0); // Darker blue
    vec3 skyColor = mix(skyColorBottom, skyColorTop, gradientFactor);

    // Apply transparency and blend with sky color
    vec3 finalColor = mix(skyColor, result, transparency);
    
    // Use the alpha from the texture and apply transparency
    FragColor = vec4(finalColor, texColor.a * (1.0 - (1.0 - transparency) * (1.0 - transparency)));
}