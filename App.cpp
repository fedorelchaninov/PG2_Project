#include <iostream>
#include <chrono>
#include <stack>
#include <random>

// OpenGL Extension Wrangler: allow all multiplatform GL functions
#include <GL/glew.h> 
// WGLEW = Windows GL Extension Wrangler (change for different platform) 
// platform specific functions (in this case Windows)
#include <GL/wglew.h> 

// GLFW toolkit
// Uses GL calls to open GL context, i.e. GLEW must be first.
#include <GLFW/glfw3.h>

// OpenGL math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "App.h"
#include "ShaderProgram.h"

extern App app;

bool vsyncEnabled = false;

void App::scroll_callback(double xoffset, double yoffset) {
    if (yoffset > 0.0) {
        std::cout << "tocis nahoru...\n";
    }
    else if (yoffset < 0.0) {
        std::cout << "tocis dolu...\n";
    }

}

void App::key_callback(int key, int scancode, int action, int mods) {
    if ((action == GLFW_PRESS) || (action == GLFW_REPEAT))
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_V:
            vsyncEnabled = !vsyncEnabled;
            glfwSwapInterval(vsyncEnabled ? 1 : 0);
            if(vsyncEnabled) std::cout << "V-Sync Enabled"<< std::endl;
            else std::cout << "V-Sync Disabled"<<std::endl;
            break;
        default:
            break;
        }
    }
}

void App::error_callback(int error, const char* description) {
    std::cerr << "GLFW Error: " << description << '\n';
}

void App::fbsize_callback(int width, int height) {
    glViewport(0, 0, width, height);


}

void App::cursor_position_callback(double xpos, double ypos) {
    static bool firstMouse = true;
    static double lastX, lastY;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;
}

void App::mouse_button_callback(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "Left mouse button pressed" << std::endl;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        std::cout << "Left mouse button released" << std::endl;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        std::cout << "Right mouse button pressed" << std::endl;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        std::cout << "Right mouse button released" << std::endl;
    }
}

void GLAPIENTRY App::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    auto const src_str = [source]() {
        switch (source)
        {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER: return "OTHER";
        }
        }();

        auto const type_str = [type]() {
            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR: return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER: return "MARKER";
            case GL_DEBUG_TYPE_OTHER: return "OTHER";
            }
            }();

            auto const severity_str = [severity]() {
                switch (severity) {
                case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
                case GL_DEBUG_SEVERITY_LOW: return "LOW";
                case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
                case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
                }
                }();

                std::cout << "[GL CALLBACK]: " <<
                    "source = " << src_str <<
                    ", type = " << type_str <<
                    ", severity = " << severity_str <<
                    ", ID = '" << id << '\'' <<
                    ", message = '" << message << '\'' << std::endl;
}

void key_callback_tr(GLFWwindow* window, int key, int scancode, int action, int mods) {
    app.key_callback(key, scancode, action, mods); // Correctly delegates to an instance method
}

void scroll_callback_tr(GLFWwindow* window, double xoffset, double yoffset) {
    app.scroll_callback(xoffset, yoffset);
}

void mouse_button_callback_tr(GLFWwindow* window, int button, int action, int mods) {
    app.mouse_button_callback(button, action, mods);
}

void cursor_position_callback_tr(GLFWwindow* window, double xpos, double ypos) {
    app.cursor_position_callback(xpos, ypos);
}

void fbsize_callback_tr(GLFWwindow* window, int width, int height) {
    app.fbsize_callback(width, height);
};

void error_callback_tr(int error, const char* description) {
    app.error_callback(error, description);
}

void GLAPIENTRY MessageCallback_tr(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    app.MessageCallback(source, type, id, severity, length, message, userParam);
}

App::App()
{
    // default constructor
    // nothing to do here (so far...)
    std::cout << "Constructed...\n";
}

bool App::init()
{
    try {
        // all initialization code


        // some init
        // if (not_success)
        //  throw std::exception("something went bad");
        {
            // init glfw
            // https://www.glfw.org/documentation.html
            glfwInit();

            // open window (GL canvas) with no special properties
            // https://www.glfw.org/docs/latest/quick.html#quick_create_window
            window = glfwCreateWindow(800, 600, "OpenGL context", NULL, NULL);
            glfwMakeContextCurrent(window);

            // init glew
            // http://glew.sourceforge.net/basic.html
            glewInit();
            wglewInit();



            GLint major, minor;
            glGetIntegerv(GL_MAJOR_VERSION, &major);
            glGetIntegerv(GL_MINOR_VERSION, &minor);
            std::cout << "ver " << major << '.' << minor << '\n';

            const char* vendor = (const char*)glGetString(GL_VENDOR);
            std::cout << "Vendor is: " << vendor << '\n';
            const char* renderer = (const char*)glGetString(GL_RENDERER);
            std::cout << "Renderer is: " << renderer << '\n';
            const char* version = (const char*)glGetString(GL_VERSION);
            std::cout << "VERSION is: " << version << '\n';
            const char* lan_version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
            std::cout << "LANGUAGE_VERSION is: " << lan_version << '\n';

            glfwSetKeyCallback(window, key_callback_tr);
            glfwSetFramebufferSizeCallback(window, fbsize_callback_tr);			// On window resize callback.
            glfwSetMouseButtonCallback(window, mouse_button_callback_tr);
            glfwSetCursorPosCallback(window, cursor_position_callback_tr);
            glfwSetScrollCallback(window, scroll_callback_tr);
            glfwSwapInterval(vsyncEnabled ? 1 : 0);

            
            ShaderProgram my_shader = ShaderProgram("resources/shaders/basic.vert", "resources/shaders/basic.frag");
        }
    }
    catch (std::exception const& e) {
        std::cerr << "Init failed : " << e.what() << std::endl;
        throw;
    }
    std::cout << "Initialized...\n";
    return true;
}

int App::run(void)
{
    try {
        // app code
        //...
        while (!glfwWindowShouldClose(window))
        {
            auto start = std::chrono::steady_clock::now();

            // Creating variables for FPS calculation
            auto startTime = std::chrono::steady_clock::now();
            std::chrono::steady_clock::time_point lastTime = startTime;
            int frameCount = 0;

            // clear canvas
            while (!glfwWindowShouldClose(window)) {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



                // poll events, call callbacks, flip back<->front buffer
                glfwPollEvents();
                glfwSwapBuffers(window);
                // FPS calculation
                frameCount++;
                std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
                std::chrono::duration<double> elapsedTime = currentTime - lastTime;
                if (elapsedTime.count() >= 1.0) {
                    double fps = static_cast<double>(frameCount) / elapsedTime.count();
                    std::cout << "FPS: " << fps << std::endl;
                    frameCount = 0;
                    lastTime = currentTime;

                }
            }
            auto end = std::chrono::steady_clock::now();

            std::chrono::duration<double> elapsed_seconds = end - start;
            std::cout << "elapsed time: " << elapsed_seconds.count() << "sec" << std::endl;
            

            // ... do_something();
            // 
            // if (condition)
            //   glfwSetWindowShouldClose(window, GLFW_TRUE);
            // 

            // Clear OpenGL canvas, both color buffer and Z-buffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Swap front and back buffers
            glfwSwapBuffers(window);

            // Poll for and process events
            glfwPollEvents();
        }
    }
    catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Finished OK...\n";
    return EXIT_SUCCESS;
}

App::~App()
{
    // clean-up
    std::cout << "Bye...\n";
    if (window)
        glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
