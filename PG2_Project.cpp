#pragma once
// C++ 
#include <iostream>
#include <chrono>

// OpenCV 
//#include <opencv2\opencv.hpp>

// OpenGL Extension Wrangler
#include <GL/glew.h> 
#include <GL/wglew.h> //WGLEW = Windows GL Extension Wrangler (change for different platform) 

// GLFW toolkit
#include <GLFW/glfw3.h>

// OpenGL math
#include <glm/glm.hpp>

#include "App.h"

// define our application
App app;

int main()
{
    if (app.init())
        return app.run();
}