#include "ui_canvas.h"

UICanvas *instance;


void UICanvas::set_active_element(UIElement* new_active) {
	if (active != NULL)
		active->set_outline(false);
	active = new_active;
	if (active == NULL) {
		current_focus_input = NULL;
		return;
	}
	active->set_outline(true);
	if (TextInput* active_textinput = dynamic_cast<TextInput*>(active)) {
		current_focus_input = active_textinput;
	} else if (Console* active_console = dynamic_cast<Console*>(active)) {
		current_focus_input = active_console->input;
	} else {
		// Stick with the last
		//current_focus_input = NULL;
	}
}

UIElement* UICanvas::get_active_element() {
	return active;
}

UICanvas::UICanvas(GLFWwindow *window_ptx) {
	window = window_ptx;
	instance = this;
	GLFWcharfun char_callback_func = [](GLFWwindow* w, unsigned int _0) {
		instance->character_callback(w, _0);
	};
	GLFWkeyfun key_callback_func = [](GLFWwindow* w, int _0, int _1, int _2, int _3) {
		instance->key_callback(w, _0, _1, _2, _3);
	};
	GLFWmousebuttonfun mouse_callback_func = [](GLFWwindow* w, int _0, int _1, int _2) {
		instance->mouse_callback(w, _0, _1, _2);
	};
	glfwSetCharCallback(window, char_callback_func);
	glfwSetKeyCallback(window, key_callback_func);
	glfwSetMouseButtonCallback(window, mouse_callback_func);

	current_focus_input = NULL;
}

void UICanvas::add(UIElement *element) {
	elements.push_back(element);
}

void UICanvas::draw(double delta_t) {
	if (dragging) {
		Screenpos mouse_position = get_mouse_position();
		active->set_transform(mouse_position.x - offset.x, mouse_position.y - offset.y, active->width, active->height);
	}
	for (UIElement* element : elements) {
		element->update(delta_t, window);
	}
}

void UICanvas::character_callback(GLFWwindow* window, unsigned int codepoint) {
	if (current_focus_input == NULL)
		return;
	current_focus_input->insert(codepoint);
}

void UICanvas::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (current_focus_input == NULL)
		return;
	if (action == GLFW_PRESS) {
		std::string cont = current_focus_input->get_content();
		unsigned pos = current_focus_input->pointer;
		if (key == GLFW_KEY_BACKSPACE) {
			cont = cont.substr(0, pos - 1) + cont.substr(pos);
			current_focus_input->dec_poiter();
			current_focus_input->set_content(cont);
		}
		else if (key == GLFW_KEY_DELETE) {
			cont = cont.substr(0, pos) + cont.substr(pos + 1); // Not checked
			current_focus_input->set_content(cont);
		}
		else if (key == GLFW_KEY_ENTER) {
			current_focus_input->insert('\n');
		}
		else if (key == GLFW_KEY_RIGHT) {
			current_focus_input->inc_poiter();
		}
		else if (key == GLFW_KEY_LEFT) {
			current_focus_input->dec_poiter();
		}
	}
}

void UICanvas::mouse_callback(GLFWwindow* window, int button, int action, int modifier) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			Screenpos mouse_pos = get_mouse_position();
			evaluate_mouseclick(mouse_pos, button);
		} else if (action == GLFW_RELEASE) {
			dragging = false;
		}
	}
}

Screenpos UICanvas::get_mouse_position() {
	double x_screen_pos, y_screen_pos;
	glfwGetCursorPos(window, &x_screen_pos, &y_screen_pos);
	return { (int)x_screen_pos, settings->height - (int)y_screen_pos };
}

void UICanvas::evaluate_mouseclick(Screenpos coords, int button) {
	for (int i = elements.size() - 1; i >= 0; i--) {
		if (elements[i]->contains_pixel(coords)) {
			set_active_element(elements[i]);
			push_to_top(i);
			dragging = true;
			offset = { coords.x - active->x, coords.y - active->y };
			return;
		}
	}
	set_active_element(NULL);
}

void UICanvas::push_to_top(int index) {
	elements.push_back(elements[index]);
	elements.erase(elements.begin() + index);
}