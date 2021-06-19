#pragma once
#include "ui_manager.h"
#include "scene_manager.h"
#include "scripting_manager.h"

#include "console.h"

const std::string init_path = "init.cfgt";

void setup_canvas();
void setup_opengl();
GLFWwindow* setup_window(int, int);
io::DataStructurePtr setup_data(int, char**);
void interprete_cmds(int, char**);
