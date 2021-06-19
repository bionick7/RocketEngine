#include <iostream>

#include "debug.h"

using namespace std;

void __print_to(std::string content, DebugOutput outp) {
	switch (outp)
	{
	case DebugOutput::INFO:
	case DebugOutput::WARNING:
		std::cout << content;
		break;
	case DebugOutput::ERROR:
		std::cerr << "(!) " << content;
		break;
	case DebugOutput::WREN_INFO:
	case DebugOutput::WREN_WARNING:
		std::cout << "\tW " << content;
		break;
	case DebugOutput::WREN_ERROR:
		std::cerr << "\t!W " << content;
		break;
	case DebugOutput::CONSOLE:
		__debug_write_to_console(content);
		break;
	default:
		break;
	}
}

void print_matrix(LongMatrix3x3 matrix) {
	print_matrix(matrix.to_float_mat());
	debug.submit();
}

void print_matrix(LongMatrix4x4 matrix) {
	print_matrix(matrix.to_float_mat());
	debug.submit();
}

void print_matrix(glm::mat4x4 matrix) {
	for (int i = 0; i < 4; i++) {
		if (i == 0) debug.out << "/ ";
		else if (i == 3) debug.out << "\\ ";
		else debug.out << "| ";
		for (int j = 0; j < 4; j++){
			debug.out << matrix[j][i] << ", ";
		}
		if (i == 0) debug.out << " \\";
		else if (i == 3) debug.out << " /";
		else debug.out << " |";
		debug.out << endl;
	}
}

void print_matrix(glm::mat3x3 matrix) {
	for (int i = 0; i < 3; i++) {
		if (i == 0) debug.out << "/ ";
		else if (i == 2) cout << "\\ ";
		else cout << "| ";
		for (int j = 0; j < 3; j++){
			debug.out << matrix[j][i] << ", ";
		}
		if (i == 0) debug.out << " \\";
		else if (i == 2) debug.out << " /";
		else debug.out << " |";
		debug.out << endl;
	}
}

void print_quaternion(glm::quat q) {
	print_quaternion(LongQuaternion(q));
}

void print_quaternion(LongQuaternion q) {
	debug.out << q.w << " + " << q.x << "i + " << q.y << "j + " << q.z << " k" << endl;
}

void print_vector(LongVector vec) {
	debug.out << "( " << vec.x << ", " << vec.y << ", " << vec.z << " )" << endl;
}

void print_vector(glm::vec4 vec) {
	debug.out << "( " << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << " )" << endl;
}

void print_vector(glm::vec3 vec) {
	print_vector(LongVector(vec));
}

void infolog(string txt) {
	debug.current_output = DebugOutput::INFO;
	debug.out << txt << endl;
	debug.submit();
}

void errlog(string txt) {
	debug.current_output = DebugOutput::ERROR;
	debug.out << txt << endl;
	debug.submit();
}

void print_number(double inp) {
	debug.out << inp << endl;
}

DebugInterface::DebugInterface()
{
	out = std::stringstream();
	current_output = DEFAULT_OUTPUT;
}

void DebugInterface::submit()
{
	submit_to(current_output);
}

void DebugInterface::submit_to(DebugOutput outp)
{
	std::string s = out.str();
	//std::cout << "==>" << s << std::endl;
	int next = s.find_first_of('\n', 0);
	while (next != s.npos && next < s.size() - 1) {
		__print_to(s.substr(0, next+1), outp);
		//std::cout << "\"" << s << "\".substr(" << next << ") == \"" << s.substr(next) << "\"" << std::endl;
		s = s.substr(next+1);
		next = s.find_first_of('\n', 0);
	}
	__print_to(s, outp);
	out.str(std::string());
}

void DebugInterface::target(DebugOutput outp)
{
	current_output = outp;
}

void DebugInterface::end()
{
	target(DEFAULT_OUTPUT);
}

DebugInterface debug = DebugInterface();