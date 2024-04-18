#pragma once

#include "Vertex.h"
#include "ShaderProgram.h"
#include "Mesh.h"

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>


struct Material {
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    GLuint diffuseTexture;
};

class Model
{
public:

    Model(const std::filesystem::path& filename);
    Model(Mesh& mesh, ShaderProgram& shader);
    Model() = default;

    void Draw(ShaderProgram& shader);

private:
    std::vector<Mesh> meshes;
    std::string directory; // Для хранения пути к каталогу модели
    std::unordered_map<std::string, GLuint> loadedTextures; // Кэш загруженных текстур

    void LoadOBJFile(const std::filesystem::path& filename); //TODO: modify
    void LoadMTLFile(const std::filesystem::path& filename); //TODO: implement

    GLuint LoadTexture(const std::filesystem::path& filepath);
    std::unordered_map<std::string, Material> materials;
    

    Mesh mesh;
    ShaderProgram shader;
    std::vector<GLuint> textures;
};

