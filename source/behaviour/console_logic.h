#pragma once
#include <string>
#include "Blackboard.h"
#include "CameraContainer.h"

constexpr int CONSOLE_COMMAND_BUFFER_SIZE = 128;

extern std::string* console_command_buffer;

void console_interpreteation(std::string);
std::string run_buffered_commands();
