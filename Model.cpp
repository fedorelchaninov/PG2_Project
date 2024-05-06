#pragma once

#include "Model.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <unordered_map>

extern std::unordered_map<std::string, Material> materials;

    // load mesh (all meshes) of the model, load material of each mesh, load textures...
    // TODO: call LoadOBJFile, LoadMTLFile, process data, create mesh and set its properties 

Model::Model(const std::filesystem::path& filename) {
    // Checking if the specified file exists
    if (!std::filesystem::exists(filename)) {
        throw std::runtime_error("Model file does not exist: " + filename.string());
    }

    // Loading model
    LoadOBJFile(filename, 0);
}

Model::Model(const std::filesystem::path& filename, const std::filesystem::path& texture) {
    // Checking if the specified file exists
    if (!std::filesystem::exists(filename)) {
        throw std::runtime_error("Model file does not exist: " + filename.string());
    }
    if (!std::filesystem::exists(texture)) {
        throw std::runtime_error("Texture file does not exist: " + filename.string());
    }

    GLuint textureId = LoadTexture(texture);
    // Loading model
    LoadOBJFile(filename, textureId);
}

void Model::Draw(ShaderProgram& shader) {
    // call Draw() on all meshes
    for (auto& mesh : meshes) {
        mesh.Draw(shader);
    }
}

void Model::addMesh(const Mesh& mesh) {
    meshes.push_back(mesh);
}

void Model::LoadOBJFile(const std::filesystem::path& filename, GLuint textureId) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename.string());
    }

    std::string line;
    std::vector<glm::vec3> tempPositions;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec2> tempTexCoords;
    std::filesystem::path mtlFilePath;

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "mtllib") {
            std::string mtlFileName;
            ss >> mtlFileName;
            mtlFilePath = std::filesystem::path(directory).append("obj").append(mtlFileName);
            LoadMTLFile(mtlFilePath);  // Загрузка MTL файла
        }
        else if (prefix == "v") {
            glm::vec3 position;
            ss >> position.x >> position.y >> position.z;
            tempPositions.push_back(position);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        }
        else if (prefix == "vt") {
            glm::vec2 texCoord;
            ss >> texCoord.x >> texCoord.y;
            tempTexCoords.push_back(texCoord);
        }
        else if (prefix == "f") {
            std::vector<std::string> faceElements(3);
            ss >> faceElements[0] >> faceElements[1] >> faceElements[2];

            for (auto& elem : faceElements) {
                std::replace(elem.begin(), elem.end(), '/', ' ');
                std::istringstream elemStream(elem);
                unsigned int vIndex, uvIndex = 0, nIndex;
                elemStream >> vIndex;

                if (elemStream.peek() == ' ') {
                    elemStream.ignore();
                    if (elemStream.peek() != ' ') {
                        elemStream >> uvIndex;
                    }
                }

                elemStream.ignore();
                elemStream >> nIndex;

                Vertex vertex;
                vertex.Position = tempPositions[vIndex - 1];
                if (uvIndex > 0 && uvIndex <= tempTexCoords.size()) {
                    vertex.TexCoords = tempTexCoords[uvIndex - 1];
                }
                vertex.Normal = tempNormals[nIndex - 1];
                vertices.push_back(vertex);
                indices.push_back(vertices.size() - 1);
            }
        }
    }

    file.close();

    std::cout << "Model Loaded: " << filename << std::endl;
    std::cout << "Vertices: " << vertices.size() << std::endl;
    std::cout << "Indices: " << indices.size() << std::endl;

    Mesh mesh(GL_TRIANGLES, vertices, indices, textureId);
    this->meshes.push_back(mesh);
}








void Model::LoadMTLFile(const std::filesystem::path& filename) {
    std::cout << "Trying to open MTL file at: " << filename << std::endl;

    if (!std::filesystem::exists(filename)) {
        std::cerr << "MTL file does not exist: " << filename << std::endl;
        return;
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open MTL file: " << filename << std::endl;
        return;
    }

    std::string line, key;
    Material* currentMaterial = nullptr;

    while (std::getline(file, line)) {
        std::istringstream stream(line);
        stream >> key;
        if (key == "newmtl") {
            std::string materialName;
            stream >> materialName;
            Material material;
            material.name = materialName;
            materials[materialName] = material;
            currentMaterial = &materials[materialName];
        }
        else if (key == "Ka" && currentMaterial) {
            stream >> currentMaterial->ambient.r >> currentMaterial->ambient.g >> currentMaterial->ambient.b;
        }
        else if (key == "Kd" && currentMaterial) {
            stream >> currentMaterial->diffuse.r >> currentMaterial->diffuse.g >> currentMaterial->diffuse.b;
        }
        else if (key == "Ks" && currentMaterial) {
            stream >> currentMaterial->specular.r >> currentMaterial->specular.g >> currentMaterial->specular.b;
        }
        else if (key == "Ns" && currentMaterial) {
            stream >> currentMaterial->shininess;
        }
        else if (key == "Ni" && currentMaterial) {
            stream >> currentMaterial->opticalDensity;
        }
        else if (key == "d" && currentMaterial) {
            float transparency;
            stream >> transparency;
            currentMaterial->transparency = transparency;
        }
        else if (key == "illum" && currentMaterial) {
            int illumModel;
            stream >> illumModel;
            currentMaterial->illumModel = illumModel;
        }
        // Handle additional keys like Ke, etc., if necessary
    }

    file.close();
}

GLuint Model::LoadTexture(const std::filesystem::path& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open texture file: " << filepath << std::endl;
        return 0;
    }

    char header[54];
    file.read(header, 54);

    // Проверка, что это BMP-файл
    if (header[0] != 'B' || header[1] != 'M') {
        std::cerr << "Not a BMP file: " << filepath << std::endl;
        return 0;
    }

    // Чтение размеров изображения
    int dataOffset = *reinterpret_cast<int*>(&header[0x0A]);
    int width = *reinterpret_cast<int*>(&header[0x12]);
    int height = *reinterpret_cast<int*>(&header[0x16]);
    int imageSize = *reinterpret_cast<int*>(&header[0x22]);

    std::vector<char> imgData(imageSize);
    file.seekg(dataOffset);
    file.read(imgData.data(), imageSize);

    // Генерация текстуры в OpenGL
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, imgData.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}
