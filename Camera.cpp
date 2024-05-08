#include "Camera.h"
#include <iostream>

Camera::Camera(glm::vec3 position) : Position(position), WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)) {
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

glm::vec3 Camera::ProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
    glm::vec3 direction{ 0 };

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        direction += Front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        direction -= Front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        direction -= Right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        direction += Right;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        direction += WorldUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        direction -= WorldUp;

    if (glm::length(direction) != 0) {
        glm::vec3 newPosition = Position + glm::normalize(direction) * MovementSpeed * deltaTime;
        if (!checkCollision(newPosition)) {
            Position = newPosition;
        }
        else {
            std::cout << "Collision detected, movement blocked." << std::endl;
        }
    }
    return direction;
}

bool Camera::checkCollision(const glm::vec3& newPos) {
    glm::vec3 teapotPosition = glm::vec3(0.0f, -8.0f, 0.0f); 
    float teapotRadius = 10.5f;

    float distance = glm::length(newPos - teapotPosition);
    bool collision = distance < (teapotRadius + MovementSpeed);

    /*std::cout << "Checking collision: newPos (" << newPos.x << ", " << newPos.y << ", " << newPos.z
        << ") teapotPos (" << teapotPosition.x << ", " << teapotPosition.y << ", " << teapotPosition.z
        << ") distance: " << distance << " collision: " << (collision ? "yes" : "no") << std::endl;*/

    return collision;
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch)
{
    xoffset *= this->MouseSensitivity;
    yoffset *= this->MouseSensitivity;

    this->Yaw += xoffset;
    this->Pitch += yoffset;

    if (constraintPitch)
    {
        if (this->Pitch > 89.0f)
            this->Pitch = 89.0f;
        if (this->Pitch < -89.0f)
            this->Pitch = -89.0f;
    }

    this->updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    front.y = sin(glm::radians(this->Pitch));
    front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

    this->Front = glm::normalize(front);
    this->Right = glm::normalize(glm::cross(this->Front, glm::vec3(0.0f, 1.0f, 0.0f)));
    this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}
