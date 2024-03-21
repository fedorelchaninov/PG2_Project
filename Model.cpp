#include "Model.h"
#include "Vertex.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

    // load mesh (all meshes) of the model, load material of each mesh, load textures...
    // TODO: call LoadOBJFile, LoadMTLFile, process data, create mesh and set its properties 

inline Model::Model(const std::filesystem::path& filename) {
    // Checking if the specified file exists
    if (!std::filesystem::exists(filename)) {
        throw std::runtime_error("Model file does not exist: " + filename.string());
    }

    // Loading model
    LoadOBJFile(filename);
}

inline void Model::Draw(ShaderProgram& shader) {
    // call Draw() on all meshes
    for (auto& mesh : meshes) {
        mesh.Draw(shader);
    }
}

void Model::LoadOBJFile(const std::filesystem::path& filename) //TODO: modify
{
}

void Model::LoadMTLFile(const std::filesystem::path& filename) //TODO: implement
{
}
