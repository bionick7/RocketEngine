#pragma once
#include "textmesh.h"

class TextInput :
	public Textmesh
{
public:
	unsigned int pointer = 0;

	TextInput(FontPtr, GLFWwindow*);
	TextInput(std::string, FontPtr, GLFWwindow*);

	void insert(char);
	void inc_pointer();
	void dec_poiter();

	void character_callback(GLFWwindow*, unsigned int);
	void key_callback(GLFWwindow*, int, int, int, int);

protected:
	std::vector<float> get_lines() override;
};
