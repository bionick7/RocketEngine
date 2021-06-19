#pragma once
#include "math_tools.h"
#include "custom_library.h"

enum class DebugOutput {
	INFO,
	WARNING,
	ERROR,
	WREN_INFO,
	WREN_WARNING,
	WREN_ERROR,
	CONSOLE,
	NOWHERE,
};

struct DebugInterface {
	const static DebugOutput DEFAULT_OUTPUT = DebugOutput::INFO;

	DebugInterface();

	std::stringstream out;
	DebugOutput current_output;

	void submit();
	void submit_to(DebugOutput);
	void target(DebugOutput);
	void end();
};

// Has to be implemented in scriptingmanager
void __debug_write_to_console(std::string);
void __print_to(std::string, DebugOutput);

void print_matrix(LongMatrix4x4);
void print_matrix(LongMatrix3x3);
void print_matrix(glm::mat4x4);
void print_matrix(glm::mat3x3);
void print_quaternion(glm::quat);
void print_quaternion(LongQuaternion);
void print_vector(glm::vec3);
void print_vector(glm::vec4);
void print_vector(LongVector);
void print_number(double);

void infolog(std::string);
void errlog(std::string);

extern DebugInterface debug;