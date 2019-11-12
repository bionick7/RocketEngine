#include "textinput.h"
#ifndef CANVAS_H
#define CANVAS_H

class UICanvas
{
public:
	UICanvas(GLFWwindow*);

	TextInput* current_focus_input;

private:
	GLFWwindow* window;
};

#endif // !CANVAS_H