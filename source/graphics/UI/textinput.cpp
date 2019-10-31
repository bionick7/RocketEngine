#include "textinput.h"

TextInput* text_input_focused = NULL;

TextInput::TextInput(Font _font, GLFWwindow* window) : Textmesh::Textmesh(L"", _font) {
	glfwSetKeyCallback(window, key_callback);
	text_input_focused = this;
}

TextInput::TextInput(std::wstring _content, Font _font, GLFWwindow* window) : Textmesh::Textmesh(_content, _font) {
	glfwSetKeyCallback(window, key_callback);
	text_input_focused = this;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS) return;
	if (text_input_focused == NULL) return;
	std::wstring cont = text_input_focused->get_content();
	wchar_t add = 0x00;
	if (0x41 < key && key < 0x5b) {
		// letter
		add = (wchar_t) key;
		if (!(mods & GLFW_MOD_SHIFT)) {
			add += 0x20;
		}
	}
	else if (key == GLFW_KEY_ESCAPE) {
		text_input_focused = NULL;
		return;
	}
	else if (key == GLFW_KEY_ENTER) {
		text_input_focused = NULL;
		return;
	}
	else if (key == GLFW_KEY_BACKSPACE) {
		cont = cont.substr(0, cont.length() - 1);
	}
	else if (key < 0x100) {
		add = key;
	}

	if (add != 0x00) {
		cont += add;
	}

	text_input_focused->set_content(cont);
}