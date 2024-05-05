#include "Mesh.h"

// indirect (indexed) draw 



Mesh::Mesh(GLenum primitive_type, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint texture_id) :
    primitive_type(primitive_type),
    vertices(vertices),
    indices(indices),
    texture_id(texture_id)
{
    // Generate the VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the VAO
    glBindVertexArray(VAO);

    // Bind and set the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), this->vertices.data(), GL_STATIC_DRAW);

    // Bind and set the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    // Unbind to prevent unintended modifications
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void Mesh::Draw(ShaderProgram& shader) const {
    shader.activate();

    // Установка текстуры, если она есть
    if (texture_id > 0) {
        glActiveTexture(GL_TEXTURE0); // Активация текстурного юнита 0
        glBindTexture(GL_TEXTURE_2D, texture_id); // Привязка текстуры
        shader.setUniform("myTexture", 0); // Сообщаем шейдеру, что myTexture находится в текстурном юните 0
    }
    else {
        glBindTexture(GL_TEXTURE_2D, 0); // Обязательно сбрасываем привязку текстуры, если текстуры нет
    }

    // Привязка VAO и отрисовка меша
    glBindVertexArray(VAO);
    if (!indices.empty()) {
        glDrawElements(primitive_type, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(primitive_type, 0, static_cast<GLsizei>(vertices.size()));
    }
    glBindVertexArray(0);

    // Сброс привязки текстуры, если она была установлена
    if (texture_id > 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


void Mesh::clear(void) {
    vertices.clear();
    indices.clear();
    texture_id = 0;
    primitive_type = GL_POINT;

    // Deleting buffers if they were created
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }

    // Deleting VAO if it was created
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}


