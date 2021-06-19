#include "textinput.h"

TextInput::TextInput(FontPtr _font, GLFWwindow* window) : Textmesh::Textmesh("", _font) {
	pointer = 0;
}

TextInput::TextInput(std::string _content, FontPtr _font, GLFWwindow* window) : Textmesh::Textmesh(_content, _font) {
	pointer = _content.length();
}

std::vector<float> TextInput::get_lines() {
	std::vector<float> result = Textmesh::get_lines();
	Textmesh::Textpos current_pos = get_text_pos(pointer);

	float sx = get_font_size() / get_ratio();
	float sy = get_font_size();
	float one_plus_spacing_x = 1 + get_spacing_x() / get_font_size();
	float one_plus_spacing_y = get_spacing_y() / get_font_size();

	result.push_back(( 0.0f + current_pos.x * one_plus_spacing_x) * sx);
	result.push_back((-0.3f - (current_pos.y) * one_plus_spacing_y) * sy);
	result.push_back(( 1.0f + current_pos.x * one_plus_spacing_x) * sx);
	result.push_back((-0.3f - (current_pos.y) * one_plus_spacing_y) * sy);
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
	if (pos.x * font->font_size * (1 + font->spacing_x) / font->ratio <= width) {
		pointer++;
		set_content(s);
	}
}

void TextInput::inc_pointer() {
	std::string content = get_content();
	pointer = pointer == content.length() ? content.length() : pointer + 1;
	request_update(UI_UPDATE_FLAGS_GEOMETRY);
}

void TextInput::dec_poiter() {
	pointer = pointer == 0 ? 0 : pointer - 1;
	request_update(UI_UPDATE_FLAGS_GEOMETRY);
}

void TextInput::character_callback(GLFWwindow* window, unsigned int codepoint) {
	set_content(get_content() + (char)codepoint);
	inc_pointer();
}

void TextInput::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		std::string cont = get_content();
		unsigned pos = pointer;
		if (key == GLFW_KEY_BACKSPACE) {
			if (pos > 0) {
				cont = cont.substr(0, pos - 1) + cont.substr(pos);
				dec_poiter();
				set_content(cont);
			}
		}
		else if (key == GLFW_KEY_DELETE) {
			cont = cont.substr(0, pos) + cont.substr(pos + 1); // Not checked
			set_content(cont);
		}
		else if (key == GLFW_KEY_ENTER) {
			insert('\n');
		}
		else if (key == GLFW_KEY_RIGHT) {
			inc_pointer();
		}
		else if (key == GLFW_KEY_LEFT) {
			dec_poiter();
		}
	}
}