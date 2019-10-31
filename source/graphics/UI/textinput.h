#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H
#include "textmesh.h"

void key_callback(GLFWwindow*, int, int, int, int);

class TextInput :
	public Textmesh
{
public:
	TextInput(Font, GLFWwindow*);
	TextInput(std::wstring, Font, GLFWwindow*);
};

#endif //!TEXT_INPUT_H