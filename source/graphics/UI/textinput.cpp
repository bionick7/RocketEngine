#include "textinput.h"

TextInput::TextInput(Font* _font, GLFWwindow* window) : Textmesh::Textmesh("", _font) {
	pointer = 0;
}

TextInput::TextInput(std::string _content, Font* _font, GLFWwindow* window) : Textmesh::Textmesh(_content, _font) {
	pointer = _content.length();
}

std::vector<float> TextInput::get_lines() {
	std::vector<float> result = Textmesh::get_lines();
	Textmesh::Textpos current_pos = get_text_pos(pointer);
	result.push_back(( 0.0f + current_pos.x * (1 + font_settings.spacing_x)) * font_settings.font_size / font_settings.ratio);
	result.push_back((-0.3f - (current_pos.y + 1) * (1 + font_settings.spacing_y)) * font_settings.font_size + height);
	result.push_back(( 1.0f + current_pos.x * (1 + font_settings.spacing_x)) * font_settings.font_size / font_settings.ratio);
	result.push_back((-0.3f - (current_pos.y + 1) * (1 + font_settings.spacing_y)) * font_settings.font_size + height);
	return result;
}

void TextInput::insert(char c) {
	std::string s = get_content();
	if (pointer >= s.length()) {
		s = s + c;
	}
	else {
		s = s.substr(0, pointer) + c + s.substr(pointer);
	}
	Textmesh::Textpos pos = get_dimensions(s);
	if (pos.x * font_settings.font_size * (1 + font_settings.spacing_x) / font_settings.ratio <= width) {
		pointer++;
		set_content(s);
	}
}

void TextInput::inc_poiter() {
	std::string content = get_content();
	pointer = pointer == content.length() ? content.length() : pointer + 1;
	request_recalculate();
}

void TextInput::dec_poiter() {
	pointer = pointer == 0 ? 0 : pointer - 1;
	request_recalculate();
}