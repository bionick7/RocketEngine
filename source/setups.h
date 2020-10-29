#include "graphics/UI/ui_canvas.h"
#pragma once

const std::string init_path = "init.cfgt";

UICanvas* setup_canvas(GLFWwindow*);
void setup_opengl();
GLFWwindow* setup_window();
io::DataStructure* setup_data(int, char**);
