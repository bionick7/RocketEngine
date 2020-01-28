#include "console.h"

std::string last_lines(std::string s, unsigned line_num) {
	std::vector<std::string> lines = std::vector<std::string>();
	char* line = new char[1000];
	for (unsigned i = 0; i < s.length(); i++) {
		if (s[i] == '\n') lines.push_back(std::string(line));
		else *(line + i) = s[i];
	}
	delete[] line;

	return s;
}

Console::Console(Font f,GLFWwindow* window) {
	input = new TextInput(f, window);
	archieved = new Textmesh("", f);
}

void Console::set_transform(int _x, int _y, int _sizex, int _sizey, bool wrap_around) {
	UIElement::set_transform(_x, _y, _sizex, _sizey, wrap_around);
	input->set_transform(x + 4, y + height - 4, width - 8, 0, wrap_around);
	archieved->set_transform(x + 4, y + 4, width - 8, height - 8, wrap_around);
}

void Console::update(double dt, GLFWwindow* win, Camera* cam) {
	recalc_flag |= input->recalc_flag;
	UIElement::update(dt, win, cam);
	input->update(dt, win, cam);
	archieved->update(dt, win, cam);
}

void Console::recalculate() {
	UIElement::recalculate();
	if (input->get_content().length() > 0) {
		char input_last = input->get_content()[input->get_content().length() - 1];
		if (input_last == '\n') {
			std::string answer = interprete(input->get_content());
			archieved->set_content(archieved->get_content() + input->get_content() + answer);
			input->set_content("");
			input->pointer = 0;
			// Should invoke on next frame
			unsigned line_height = archieved->font_settings.font_size * (1 + archieved->font_settings.spacing_y);
			unsigned archieved_height = std::min(height - 2 * line_height, line_height * ++++archieved_lines);
			archieved->set_transform(x + 4, y + height - 4 - archieved_height, width - 8, archieved_height);
			input->set_transform(x + 4, y + 4 , width - 8, height - archieved_height - 8);
		}
	}
}

std::string Console::interprete(std::string inp) {
	return "-> " + inp;
}