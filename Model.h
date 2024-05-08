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

    // additional properties based on MTL file specification
    float opticalDensity;
    float transparency;
    int illumModel;

    Material() : shininess(0.0f), diffuseTexture(0),
        opticalDensity(1.0f), transparency(1.0f), illumModel(1) {}
};

class Model
{
public:

    Model(const std::filesystem::path& filename);
    Model(const std::filesystem::path& filename, const std::filesystem::path& texture);
    Model(Mesh& mesh, ShaderProgram& shader);
    Model() = default;

    void Draw(ShaderProgram& shader);
    void addMesh(const Mesh& mesh);


private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::unordered_map<std::string, GLuint> loadedTextures;

    void LoadOBJFile(const std::filesystem::path& filename, GLuint textureId); 
    void LoadMTLFile(const std::filesystem::path& filename);

    GLuint LoadTexture(const std::filesystem::path& filepath);
    std::unordered_map<std::string, Material> materials;
    

    Mesh mesh;
    ShaderProgram shader;
    std::vector<GLuint> textures;
};

