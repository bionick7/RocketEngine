#pragma once
#include "asset_manager.h"
#include "settings.h"
#include "ui_element.h"

#define WINDOW_RESIZED 1

class UICanvas
{
public:
	UICanvas(GLFWwindow*);
	void draw();
	void add(UIElement*);

	void character_callback(GLFWwindow*, unsigned int);
	void key_callback(GLFWwindow*, int, int, int, int);
	void mouse_callback(GLFWwindow*, int, int, int);

	UIElement* current_focus_input;

	void set_active_element(UIElement*);
	UIElement* get_active_element();

	Screenpos get_mouse_position();

private:
	GLFWwindow* window;

	std::vector<UIElement*> elements;
	UIElement* active;

	void evaluate_mouseclick(Screenpos, int);
	void push_to_top(int);

	bool dragging;
	Screenpos offset;
};