#pragma once

struct Material {
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    GLuint diffuseTexture;  
    GLuint reflectionTexture;
    float opticalDensity;
    float transparency;
    int illumModel;

    Material() :
        shininess(0.0f), diffuseTexture(0), reflectionTexture(0),
        opticalDensity(1.0f), transparency(1.0f), illumModel(1) {}
};