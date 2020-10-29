#pragma once
#include "ui_element.h"
#include "console_logic.h"
#include "textinput.h"
#include "Blackboard.h"

std::string last_lines(std::string, int);

extern std::string console_text;

class Console :
	public UIElement
{
public:
	Console(Font*, GLFWwindow*);

	void update(double, GLFWwindow*) override;
	void recalculate() override;
	void set_transform(int, int, int, int, bool=false) override;

	TextInput* input;
	Textmesh* archieved;

private:
	unsigned max_lines = 5;
	unsigned line_height = 1;

	void recalculate_internal();
	void interprete(std::string);
};

