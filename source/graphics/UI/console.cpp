#include "console.h"

std::string console_text = "";

std::string last_lines(std::string s, int line_num) {
	// Pretty inefficient probably, but meh
	std::vector<std::string> lines = std::vector<std::string>();
	std::string line = std::string("");
	for (unsigned i = 0; i < s.length(); i++) {
		if (s[i] == '\n') { 
			lines.push_back(line); 
			line.clear();
		}
		else line += s[i];
	}

	std::string res = std::string("");
	for (int i = std::max(0, (int) lines.size() - line_num); i < lines.size(); i++) {
		res += lines[i] + "\n";
	}

	return res;
}

Console::Console(Font* f,GLFWwindow* window) {
	input = new TextInput(f, window);
	archieved = new Textmesh("", f);

	line_height = archieved->font_settings.font_size * (1 + archieved->font_settings.spacing_y);

	global_blackboard->console_flags |= CONSOLE_FLAG_CLEAR;
}

void Console::set_transform(int _x, int _y, int _sizex, int _sizey, bool wrap_around) {
	UIElement::set_transform(_x, _y, _sizex, _sizey, wrap_around);
	recalculate_internal();
}

void Console::update(double dt, GLFWwindow* win) {
	if (CONSOLE_FLAG_CLEAR & global_blackboard->console_flags) {
		archieved->set_content("");
		console_text = "";
		recalc_flag = true;
		global_blackboard->console_flags &= !CONSOLE_FLAG_CLEAR;
		recalculate_internal();
	}
	recalc_flag |= input->recalc_flag || (CONSOLE_FLAG_UPDATE & global_blackboard->console_flags);
	UIElement::update(dt, win);
	input->update(dt, win);
	archieved->update(dt, win);
	if (CONSOLE_FLAG_UPDATE & global_blackboard->console_flags) {
		recalculate_internal();
	}
	global_blackboard->console_flags &= !CONSOLE_FLAG_UPDATE;
}

void Console::recalculate() {
	UIElement::recalculate();
	//std::cout << "a";
	if (input->get_content().length() > 0) {
		char input_last = input->get_content()[input->get_content().length() - 1];
		if (input_last == '\n') { // Hit enter
			interprete(input->get_content());
			input->set_content("");
			input->pointer = 0;
		}
	}
	archieved->set_content(console_text);
}

void Console::recalculate_internal(){
	// Should invoke on next frame
	unsigned archieved_lines = 0;
	std::string archieved_content = archieved->get_content();
	for (int i = 0; i < archieved_content.size(); i++) {
		archieved_lines += archieved_content[i] == '\n';
	}
	unsigned archieved_height = std::min(height - 2 * line_height, line_height * archieved_lines + 10);
	archieved->set_transform(x + 4, y + height - 4 - archieved_height, width - 8, archieved_height);
	input->set_transform(x + 4, y + 4, width - 8, height - archieved_height - 8);
}

void Console::interprete(std::string inp) {
	console_interpreteation(inp.substr(0, inp.size()-1));
}