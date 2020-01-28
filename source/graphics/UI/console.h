#pragma once
#include "ui_element.h"
#include "textinput.h"

std::string last_lines(std::string, unsigned);

class Console :
	public UIElement
{
public:
	Console(Font, GLFWwindow*);

	void update(double, GLFWwindow*, Camera*) override;
	void recalculate() override;
	void set_transform(int, int, int, int, bool=false) override;

	TextInput* input;
	Textmesh* archieved;

private:
	unsigned short archieved_lines = 1;

	std::string interprete(std::string);
};

