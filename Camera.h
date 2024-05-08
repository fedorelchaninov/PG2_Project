#pragma once
#include <GL/glew.h>
#include <glm/fwd.hpp>
#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>

class Camera
{
public:

    // cam attr
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up; // camera local UP vector
    glm::vec3 WorldUp;

    GLfloat Yaw = -90.0f;
    GLfloat Pitch = 0.0f;;
    GLfloat Roll = 0.0f;

    // cam options
    GLfloat MovementSpeed = 0.50f;
    GLfloat MouseSensitivity = 0.5f;

    Camera(glm::vec3 position);

    glm::mat4 GetViewMatrix();

    glm::vec3 ProcessInput(GLFWwindow* window, GLfloat deltaTime);

    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE);

    bool checkCollision(const glm::vec3& newPos);

private:
    void updateCameraVectors();
};