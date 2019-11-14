#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H
#include "textmesh.h"

void character_callback(GLFWwindow*, unsigned int);
void key_callback(GLFWwindow*, int, int, int, int);

class TextInput :
	public Textmesh
{
public:
	unsigned int cursor_position = 0;

	TextInput(Font, GLFWwindow*);
	TextInput(std::string, Font, GLFWwindow*);

protected:
	std::vector<float> get_lines() override;
};

#endif //!TEXT_INPUT_H