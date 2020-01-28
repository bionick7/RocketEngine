#include "graphics/UI/ui_canvas.h"
#pragma once

const std::string init_path = "init.cfgt";

UICanvas* setup_canvas(GLFWwindow*, Camera*);
void setup_opengl();
GLFWwindow* setup_window();
cfg::DataStructure* setup_data(char**);
