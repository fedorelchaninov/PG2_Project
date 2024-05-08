#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float transparency;
uniform sampler2D myTexture;

uniform vec3 sunLightColor;
uniform vec3 sunLightPos;
uniform float sunLightIntensity;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(viewPos - FragPos);
    vec3 sunDir = normalize(sunLightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 0.5 * diff * lightColor;

    float sunDiff = max(dot(norm, sunDir), 0.0);
    vec3 sunDiffuse = sunLightIntensity * sunDiff * sunLightColor; // calc of sunlight without fading

    vec3 ambient = 0.4 * lightColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.2 * spec * lightColor;

    vec4 texColor = texture(myTexture, TexCoords);
    vec3 result = (diffuse + ambient + specular + sunDiffuse) * texColor.rgb;

    float gradientFactor = clamp(FragPos.z / 500.0, 0.0, 1.0);
    vec3 skyColorTop = vec3(0.4, 0.7, 1.0);
    vec3 skyColorBottom = vec3(0.7, 0.9, 1.0);
    vec3 skyColor = mix(skyColorBottom, skyColorTop, gradientFactor);

    vec3 finalColor = mix(skyColor, result, transparency);

    FragColor = vec4(finalColor, texColor.a * transparency);
}