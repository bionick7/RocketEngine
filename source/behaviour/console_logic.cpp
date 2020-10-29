#include "console_logic.h"
#include "Celestial.h"
#include <iostream>

using namespace std;

std::string* console_command_buffer = new std::string[CONSOLE_COMMAND_BUFFER_SIZE];
int cc_buffer_counter;

// Thx random internet dude !
// Very cool
constexpr unsigned int str2int(const char* str, int h = 0) {
	return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

string capitalize(string s) {
	for (int i = 0; i < s.size(); i++) {
		s[i] = toupper(s[i]);
	}
	return s;
}

int count_character_in_string(string s, char c) {
	int res = 0;
	for (int i = 0; i < s.size(); i++) {
		res += (s[i] == c);
	}
	return res;
}

string cutoff_after(string s, char c) {
	string res = "";
	bool writing = false;
	for (int i = 0; i < s.length(); i++) {
		if (writing) {
			res += s[i];
		}
		if (s[i] == c) writing = true;
	}
	return res;
}

Orbiter* find_celestial_of_name(Orbiter* entry_point, string name) {
	if (capitalize(entry_point->name) == name) {
		return entry_point;
	}
	if (entry_point->get_type() == Type::CELESTIAL) {
		for (unsigned i = 0; i < ((Celestial*)entry_point)->sattelites_num; i++) {
			Orbiter* res = find_celestial_of_name(((Celestial*)entry_point)->get_sattelite(i), name);
			if (res != nullptr) {
				return res;
			}
		}
	}
	return nullptr;
}

void console_interpreteation(string input) {
	if (cc_buffer_counter >= CONSOLE_COMMAND_BUFFER_SIZE - 1) {
		std::cerr << "Can only buffer up to " << CONSOLE_COMMAND_BUFFER_SIZE << " Console commands per frame." << std::endl;
	}
	else {
		console_command_buffer[cc_buffer_counter++] = input;
	}
}

string interprete_single_command(string input) {
	string command = input.substr(0, input.find_first_of(' '));
	int arg_count = count_character_in_string(input, ' ');
	string rest = cutoff_after(input, ' ');
	string args[5];
	for (int i = 0; i < 5; i++) {
		if (i >= arg_count) {
			args[i] = "";
		}
		else {
			args[i] = rest.substr(0, rest.find_first_of(i == arg_count - 1 ? '\n' : ' '));
			if (i != arg_count - 1) {
				// std::cout << rest << " -> ";
				rest = cutoff_after(rest, ' ');
				// std::cout << rest << std::endl;
			}
		}
	}
	// std::cout << "cmd: \"" << command << "\"; arg1: \"" << args[0] << "\"; arg2: \"" << args[1] << "\"" << std::endl;
	switch (str2int(capitalize(command).c_str()))
	{
	case str2int("CLS"):
		global_blackboard->console_flags |= CONSOLE_FLAG_CLEAR;
		return "";
	case str2int("FOCUS"): {
		args[0] = capitalize(args[0]);
		Orbiter* orbiter = find_celestial_of_name(global_blackboard->hierarchy_entrypoint, capitalize(args[0]));
		if (orbiter == nullptr) {
			return "No such body: " + args[0];
		}
		if (args[1] == "*D") {
			global_blackboard->main_camera->focus(orbiter);
		}
		else {
			global_blackboard->main_camera->interpolate_focus(orbiter);
		}
		return "Focusing on " + orbiter->name;}
	case str2int("ACC"):
		time_scale *= 5;
		return "k = " + std::to_string(time_scale);
	case str2int("DEC"):
		time_scale /= 5;
		return "k = " + std::to_string(time_scale);
	default:
		return "Command not known: " + command;
	}
	return "";
}

string run_buffered_commands() {
	string res = "";
	for (int i = 0; i < cc_buffer_counter; i++) {
		res += "> " + console_command_buffer[i] + "\n";
		if (console_command_buffer[i].size() > 0)
			res += interprete_single_command(console_command_buffer[i]) + "\n";
	}
	if (cc_buffer_counter > 0) {
		global_blackboard->console_flags |= CONSOLE_FLAG_UPDATE;
	}
	cc_buffer_counter = 0;
	if (res != "")
		std::cout << res << std::endl;
	return res;
}