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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool vsyncEnabled = false;
bool fullscreen = false;
int windowPosX, windowPosY, windowWidth, windowHeight;
ShaderProgram my_shader;

float sunAngle = 0.0f; 
glm::vec3 sunPosition = glm::vec3(1200.0f, 850.0f, -1400.0f);

void App::update_projection_matrix(void)
{
    if (height < 1)
        height = 1;   // avoid division by 0

    float ratio = static_cast<float>(width) / height;

    projection_matrix = glm::perspective(
        glm::radians(fov),   // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90� (extra wide) and 30� (quite zoomed in)
        ratio,               // Aspect Ratio. Depends on the size of your window.
        0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        20000.0f             // Far clipping plane. Keep as little as possible.
    );
}

#include <vector>
#include <cmath>

std::vector<float> createCircleVertices(float centerX, float centerY, float radius, int numSegments) {
    std::vector<float> vertices;
    for (int i = 0; i <= numSegments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numSegments); // текущий угол
        float x = radius * cosf(theta); // рассчитываем x
        float y = radius * sinf(theta); // рассчитываем y
        vertices.push_back(x + centerX);
        vertices.push_back(y + centerY);
    }
    return vertices;
}


void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0.0) {
        std::cout << "tocis nahoru...\n";
    }
    else if (yoffset < 0.0) {
        std::cout << "tocis dolu...\n";
    }

    // get App instance
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    this_inst->fov += 10.0 * yoffset;
    this_inst->fov = std::clamp(this_inst->fov, 20.0f, 170.0f); // limit FOV to reasonable values...

    this_inst->update_projection_matrix();
}

void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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
        case GLFW_KEY_F11: 
            if (!fullscreen) {
                glfwGetWindowPos(window, &windowPosX, &windowPosY);
                glfwGetWindowSize(window, &windowWidth, &windowHeight);

                GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

                glfwSetWindowMonitor(window, primaryMonitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
            }
            else {
                glfwSetWindowMonitor(window, nullptr, windowPosX, windowPosY, windowWidth, windowHeight, GLFW_DONT_CARE);
            }

            fullscreen = !fullscreen;
            break;
        default:
            break;
        }
    }
}

void App::error_callback(GLFWwindow* window, int error, const char* description) {
    std::cerr << "GLFW Error: " << description << '\n';
}

void App::fbsize_callback(GLFWwindow* window, int width, int height) {
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    this_inst->width = width;
    this_inst->height = height;

    // set viewport
    glViewport(0, 0, width, height);
    //now your canvas has [0,0] in bottom left corner, and its size is [width x height] 

    this_inst->update_projection_matrix();
}

void App::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
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

    // get App instance
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    this_inst->camera.ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
}

void App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
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

void GLAPIENTRY App::MessageCallback(GLFWwindow* window, GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
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


App::App() : camera(glm::vec3(0.0f, 0.0f, 3.0f))
{
    // default constructor
    // nothing to do here (so far...)
    std::cout << "Constructed...\n";
}

void App::updateSunPosition() {
    float radius = 1500.0f; // радиус траектории солнца
    float height = 850.0f;  // высота солнца над горизонтом
    sunAngle += 0.0002f; // изменение угла для анимации движения солнца

    // Вычисляем позицию солнца по круговой траектории
    sunPosition.x = radius * cos(sunAngle);
    sunPosition.z = radius * sin(sunAngle);
    sunPosition.y = height;

    // Обновление позиции и цвета света в шейдере
    my_shader.setUniform("sunLightPos", sunPosition);
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

            glfwSetWindowUserPointer(window, this);
            glfwSetKeyCallback(window, key_callback);
            glfwSetFramebufferSizeCallback(window, fbsize_callback);			// On window resize callback.
            glfwSetMouseButtonCallback(window, mouse_button_callback);
            glfwSetCursorPosCallback(window, cursor_position_callback);
            glfwSetScrollCallback(window, scroll_callback);
            glfwSwapInterval(vsyncEnabled ? 1 : 0);

           
            init_assets();

        }
    }
    catch (std::exception const& e) {
        std::cerr << "Init failed : " << e.what() << std::endl;
        throw;
    }
    std::cout << "Initialized...\n";
    return true;
}

void App::init_assets(void) {
    my_shader = ShaderProgram("resources/shaders/basic.vert", "resources/shaders/basic.frag");


    Model my_model("obj/bunny_tri_vn.obj", "resources/textures/ground_texture.bmp");
    Model groundModel("obj/ground.obj", "resources/textures/grass.bmp");
    Model houseModel("obj/fullhouse.obj", "resources/textures/tiled-stones.bmp");
    Model mocrowaveModel("obj/microwave_not_glass.obj", "resources/textures/microwave_col.bmp");
    Model mocrowaveGlassModel("obj/glass_for_microwave.obj");
    Model greetingModel("obj/greet.obj", "resources/textures/greet.bmp");
    Model sunModel("obj/sun.obj", "resources/textures/sun.bmp");




    scene.insert({ "bunny", my_model });
    scene.insert({ "ground", groundModel });
    scene.insert({ "house", houseModel });
    scene.insert({ "microwave", mocrowaveModel });
    scene.insert({ "glass", mocrowaveGlassModel });
    scene.insert({ "greeting", greetingModel });
    scene.insert({ "sun", sunModel });
}

int App::run(void) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    update_projection_matrix();
    glViewport(0, 0, width, height);

    glClearColor(0.7f, 0.9f, 1.0f, 1.0f); // Светлый цвет неба
    

    try {
        double lastTime = glfwGetTime();
        camera.Position = glm::vec3(0, 50, 100); // Устанавливаем камеру выше земли
        int frameCount = 0;


        // render
        while (!glfwWindowShouldClose(window)) {

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            my_shader.activate();

            double currentTime = glfwGetTime();
            double delta_t = currentTime - lastTime; // Correct delta time calculation
            camera.ProcessInput(window, delta_t); // Process keyboard and mouse input
            glm::mat4 view_matrix = camera.GetViewMatrix(); // Use camera's view matrix
            float angle = static_cast<float>(currentTime); // Rotation angle depends on time
            glm::vec3 cameraFront = camera.Position + camera.Front;


            updateSunPosition();

            // Set shader uniforms
            my_shader.setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
            glm::vec3 lightPos = camera.Position + camera.Front * 10.0f;
            my_shader.setUniform("lightPos", lightPos);
            my_shader.setUniform("uP_m", projection_matrix);
            my_shader.setUniform("uV_m", view_matrix);

            
            glm::vec3 sunLightColor = glm::vec3(1.0f, 0.9f, 0.8f); // Цвет света, близкий к цвету солнечного света
            float sunLightIntensity = 0.8f; // Увеличенная интенсивность для симуляции яркости солнца
            my_shader.setUniform("sunLightColor", sunLightColor);
            my_shader.setUniform("sunLightPos", sunPosition);
            my_shader.setUniform("sunLightIntensity", sunLightIntensity);

            // Draw Bunny
            glm::mat4 modelBunny = glm::mat4(1.0f);
            modelBunny = glm::translate(modelBunny, glm::vec3(-375.0f, 13.0f, -392.0f));
            modelBunny = glm::rotate(modelBunny, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelBunny = glm::rotate(modelBunny, glm::radians(angle * 50), glm::vec3(0.0f, 0.0f, 1.0f));
            modelBunny = glm::scale(modelBunny, glm::vec3(0.5f, 0.5f, 0.5f));
            my_shader.setUniform("uM_m", modelBunny);
            my_shader.setUniform("transparency", 1.0f);  // Непрозрачный
            scene["bunny"].Draw(my_shader);


            // Draw Ground
            my_shader.setUniform("transparency", 1.0f); // Непрозрачный для земли
            glm::mat4 modelGround = glm::mat4(1.0f);
            my_shader.setUniform("uM_m", modelGround);
            scene["ground"].Draw(my_shader);


            // Draw House
            glm::mat4 modelHouse = glm::mat4(1.0f); // Start with an identity matrix
            modelHouse = glm::translate(modelHouse, glm::vec3(-330.0f, -2.0f, -400.0f));
            modelHouse = glm::rotate(modelHouse, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate 90 degrees around the Y axis
            modelHouse = glm::scale(modelHouse, glm::vec3(6.0f, 6.0f, 6.0f)); // Scale the model up by a factor of 5
            my_shader.setUniform("uM_m", modelHouse);
            scene["house"].Draw(my_shader);
            

            // Draw Microwave
            glm::mat4 modelMicrowave = glm::mat4(1.0f); // Start with an identity matrix
            modelMicrowave = glm::translate(modelMicrowave, glm::vec3(-380.0f, 5.0f, -400.0f));
            modelMicrowave = glm::rotate(modelMicrowave, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate 90 degrees around the Y axis
            // Rotate 90 degrees around the Z axis
            modelMicrowave = glm::scale(modelMicrowave, glm::vec3(20.0f, 20.0f, 20.0f));
            my_shader.setUniform("uM_m", modelMicrowave);
            scene["microwave"].Draw(my_shader);

            // Draw Glass
            my_shader.setUniform("transparency", 0.5f);
            glm::mat4 modelGlass = glm::mat4(1.0f); // Start with an identity matrix
            modelGlass = glm::translate(modelGlass, glm::vec3(-380.0f, 5.0f, -400.0f));
            modelGlass = glm::rotate(modelGlass, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate 90 degrees around the Y axis
            modelGlass = glm::scale(modelGlass, glm::vec3(20.0f, 20.0f, 20.0f));
            my_shader.setUniform("uM_m", modelGlass);
            scene["glass"].Draw(my_shader);

            // Draw Greeting
            my_shader.setUniform("transparency", 1.0f);
            glm::mat4 modelGreeting = glm::mat4(1.0f); // Start with an identity matrix
            modelGreeting = glm::scale(modelGreeting, glm::vec3(6.0f, 6.0f, 6.0f));
            modelGreeting = glm::rotate(modelGreeting, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelGreeting = glm::translate(modelGreeting, glm::vec3(-55.0f, 68.0f, 0.0f));
            my_shader.setUniform("uM_m", modelGreeting);
            scene["greeting"].Draw(my_shader);

            // Draw Sun
            my_shader.setUniform("transparency", 1.0f);
            glm::mat4 modelSun = glm::mat4(1.0f); // Start with an identity matrix
            modelSun = glm::translate(modelSun, sunPosition);
            modelSun = glm::rotate(modelSun, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate 90 degrees around the Y axis
            modelSun = glm::scale(modelSun, glm::vec3(20.0f, 20.0f, 20.0f));
            modelSun = glm::rotate(modelSun, glm::radians(angle * 10), glm::vec3(0.0f, 0.0f, 1.0f));
            my_shader.setUniform("uM_m", modelSun);
            scene["sun"].Draw(my_shader);


            
            // Draw all models in the scene
            /*for (auto& [key, model] : scene) {
                model.Draw(my_shader);
            }*/

            glfwSwapBuffers(window);
            glfwPollEvents();

            // FPS count
            frameCount++;
            if (currentTime - lastTime >= 1.0) {
                std::cout << "FPS: " << frameCount << std::endl;
                frameCount = 0;
                lastTime += 1.0;
            }
        }
    }
    catch (std::exception const& e) {
        std::cerr << "App failed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Finished OK.\n";
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
