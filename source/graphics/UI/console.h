#pragma once
#include "ui_element.h"
#include "textinput.h"
#include "scripting_manager.h"
#include "audio_manager.h"

std::string last_lines(std::string, int);

class Console :
	public UIElement
{
public:
	Console(FontPtr, GLFWwindow*);
	~Console();

	void step() override;
	void set_transform(int, int, int, int, bool=false) override;
	void set_transform_anchor_margin(int, int, int, int, float, float, float, float) override;

	TextInput* input;
	Textmesh* archieved;

	void character_callback(GLFWwindow*, unsigned int);
	void key_callback(GLFWwindow*, int, int, int, int);

protected:
	void interprete_signal(RadioSignal signal) override;

private:
	const int input_height = 30;
	const int border = 3;
	const int input_archive_gap = 3;

	unsigned max_lines = 5;
	unsigned line_height = 1;

	AudioResourcePtr typing_sound;
	AudioPlayer* audio_player;

	void recalculate_internal();
	void interprete(std::string);
};

