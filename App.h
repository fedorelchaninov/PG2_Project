#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include "Mesh.h"

#include <unordered_map>
#include "Camera.h"


class App {
public:
    Camera camera;

    void init_assets(void);
    void update_projection_matrix(void);

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void error_callback(GLFWwindow* window, int error, const char* description);
    static void fbsize_callback(GLFWwindow* window, int width, int height);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void GLAPIENTRY MessageCallback(GLFWwindow* window, GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);


    App();

    bool init(void);
    int run(void);

    ~App();
private:
    GLFWwindow* window = nullptr;

protected:
    int width{ 0 }, height{ 0 };
    float fov = 60.0f;
    glm::mat4 projection_matrix = glm::identity<glm::mat4>();

    std::unordered_map<std::string, Model> scene;
};

