#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;
in vec2 TexCoords;

uniform vec3 objectColor; // ���� �������
uniform vec3 lightColor;  // ���� �����
uniform vec3 viewPos;     // ������� ������
uniform float transparency; // ������������
uniform sampler2D myTexture; // Texture sampler for the first texture

void main()
{
    // Lighting calculations
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    // ����������� ������������� ���������� � ����������� �����
    vec3 diffuse = diff * lightColor; // ���������� ���������� �����
    vec3 ambient = 0.5 * lightColor; // ����� ������� ���������� ����

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor * 0.8; // ��������� ���������� ���������

    // Sample the texture color
    vec4 texColor = texture(myTexture, TexCoords);
    vec3 result = (diffuse + ambient + specular) * texColor.rgb * objectColor; // Combine lighting with texture color and object color

    // Blend with sky color based on transparency
    float gradientFactor = clamp(FragPos.z / 500.0, 0.0, 1.0); // Adjust based on scene scale
    vec3 skyColorTop = vec3(0.4, 0.7, 1.0); // Light blue
    vec3 skyColorBottom = vec3(0.7, 0.9, 1.0); // Darker blue
    vec3 skyColor = mix(skyColorBottom, skyColorTop, gradientFactor);

    // Apply transparency and blend with sky color
    vec3 finalColor = mix(skyColor, result, transparency);
    
    // Use the alpha from the texture and apply transparency
    FragColor = vec4(finalColor, texColor.a * (1.0 - (1.0 - transparency) * (1.0 - transparency)));
}