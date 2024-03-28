#pragma once

#include "Model.h"

#include <iostream>
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
    LoadOBJFile(filename);
}

void Model::Draw(ShaderProgram& shader) {
    // call Draw() on all meshes
    for (auto& mesh : meshes) {
        mesh.Draw(shader);
    }
}

void Model::LoadOBJFile(const std::filesystem::path& filename) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<glm::vec3> tempPositions;
    std::vector<glm::vec2> tempTexCoords;
    std::vector<glm::vec3> tempNormals;

    std::ifstream fileStream(filename);
    std::string line;
    if (!fileStream.is_open()) {
        throw std::runtime_error("Failed to open the file: " + filename.string());
    }

    while (getline(fileStream, line)) {
        std::istringstream lineStream(line);
        std::string type;
        lineStream >> type;

        if (type == "v") {
            glm::vec3 position;
            lineStream >> position.x >> position.y >> position.z;
            tempPositions.push_back(position);
        }
        else if (type == "vt") {
            glm::vec2 texCoord;
            lineStream >> texCoord.x >> texCoord.y;
            tempTexCoords.push_back(texCoord);
        }
        else if (type == "vn") {
            glm::vec3 normal;
            lineStream >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        }
        else if (type == "f") {
            unsigned int vIndex[3], uvIndex[3], nIndex[3];
            char slash;

            // Измените этот шаблон в соответствии с вашим форматом OBJ файла
            lineStream >> vIndex[0] >> slash >> uvIndex[0] >> slash >> nIndex[0]
                >> vIndex[1] >> slash >> uvIndex[1] >> slash >> nIndex[1]
                >> vIndex[2] >> slash >> uvIndex[2] >> slash >> nIndex[2];

            // Добавление индексов вершин, текстурных координат и нормалей
            for (int i = 0; i < 3; i++) {
                indices.push_back(vIndex[i] - 1); // OBJ индексы начинаются с 1, в C++ с 0

                Vertex vertex;
                vertex.Position = tempPositions[vIndex[i] - 1];
                if (uvIndex[i] <= tempTexCoords.size()) { // Проверка на наличие текстурных координат
                    vertex.TexCoords = tempTexCoords[uvIndex[i] - 1];
                }
                if (nIndex[i] <= tempNormals.size()) { // Проверка на наличие нормалей
                    vertex.Normal = tempNormals[nIndex[i] - 1];
                }
                vertices.push_back(vertex);
            }
        }
    }

    // Создание меша
    GLenum primitiveType = GL_TRIANGLES; // Используемый тип примитива
    GLuint textureId = 0; // Пример: textureId должен быть загружен или определен другим способом
    Mesh mesh(primitiveType, vertices, indices, textureId);
    meshes.push_back(mesh);
}



void Model::LoadMTLFile(const std::filesystem::path& filename) {
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
        // Обработка других ключей и свойств материала
    }
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