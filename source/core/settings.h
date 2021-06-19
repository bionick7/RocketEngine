#pragma once

#include "../data_input/reader.h"

#define SET_MASER_VOLUME	0x00
#define SET_SFX_VOLUME		0x01

#define SET_NO_LANGUAGE		0x00
#define SET_ENGLISH			0x01
#define SET_ESPERANTO		0x02
// ...

#define RENDER_FLAGS_POST_PROCESSING 0x01

const char volume_array_size = 2;
const char languages_num = 3;

struct Color {
	Color();
	Color(float, float, float);
	Color(float, float, float, float);
	Color(unsigned int);

	float r, g, b, a;
};

Color get_color(io::DataStructurePtr, std::string);

class Settings {
public:
	Settings();
	Color background;
	Color draw;
	int line_thickness;
	int width, height;

	char language;

	double mouse_sensitivity;
	double initial_fov;
	double max_vertical_angle;
	double min_vertical_angle;
	double scroll_sensitivity;

	long render_flags;

	float get_volume(int);
	void change_language_to(int);
	void reload(io::DataStructurePtr);

	std::string get_text_for(std::string);

private:
	
	//std::map<std::string, std::string> translations = std::map<std::string, std::string>();

	std::vector<io::DataStructurePtr> translation_pointers = std::vector<io::DataStructurePtr>(languages_num);
	std::vector<float> volumes = std::vector<float>(volume_array_size);
};

extern Settings* settings;
