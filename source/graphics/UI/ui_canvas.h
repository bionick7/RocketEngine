#include "console.h"
#include "graph.h"
#ifndef CANVAS_H
#define CANVAS_H

class UICanvas
{
public:
	UICanvas(GLFWwindow*, Camera*);
	void draw(double);
	void add(UIElement*);

	void character_callback(GLFWwindow*, unsigned int);
	void key_callback(GLFWwindow*, int, int, int, int);
	void mouse_callback(GLFWwindow*, int, int, int);

	TextInput* current_focus_input;


	void set_active_element(UIElement*);
	UIElement* get_active_element();

	Screenpos get_mouse_position();

private:
	GLFWwindow* window;
	Camera* camera;

	std::vector<UIElement*> elements;
	UIElement* active;

	void evaluate_mouseclick(Screenpos, int);
	void push_to_top(int);

	bool dragging;
	Screenpos offset;
};

#endif // !CANVAS_H