#pragma once
#include "textmesh.h"

class TextInput :
	public Textmesh
{
public:
	unsigned int pointer = 0;

	TextInput(Font*, GLFWwindow*);
	TextInput(std::string, Font*, GLFWwindow*);

	void insert(char);
	void inc_poiter();
	void dec_poiter();

protected:
	std::vector<float> get_lines() override;
};
