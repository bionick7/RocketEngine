#include "textinput.h"

TextInput* text_input_focused = NULL;

TextInput::TextInput(Font _font, GLFWwindow* window) : Textmesh::Textmesh("", _font) {
	glfwSetCharCallback(window, character_callback);
	glfwSetKeyCallback(window, key_callback);
	text_input_focused = this;
	cursor_position = 0;
}

TextInput::TextInput(std::string _content, Font _font, GLFWwindow* window) : Textmesh::Textmesh(_content, _font) {
	glfwSetCharCallback(window, character_callback);
	glfwSetKeyCallback(window, key_callback);
	text_input_focused = this;
	cursor_position = _content.length();
}

std::vector<float> TextInput::get_lines() {
	std::vector<float> result = Textmesh::get_lines();
	Textmesh::Textpos current_pos = get_text_pos(3);
	result.push_back(0.0f + current_pos.x * (1 + spacing_x));
	result.push_back(0.0f - current_pos.y * (1 + spacing_y));
	result.push_back(1.0f + current_pos.x * (1 + spacing_x));
	result.push_back(0.0f - current_pos.y * (1 + spacing_y));
	return result;
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
	std::string cont = text_input_focused->get_content();
	cont += codepoint;
	text_input_focused->set_content(cont);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		std::string cont = text_input_focused->get_content();
		if (key == GLFW_KEY_BACKSPACE) {
			cont = cont.substr(0, cont.length() - 1);
		}
		else if (key == GLFW_KEY_ENTER) {
			cont += '\n';
		}
		text_input_focused->set_content(cont);
	}
}