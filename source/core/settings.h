#ifndef SETTINGS_H
#define SETTINGS_H

#include "../data_input/reader.h"

#define SET_MASER_VOLUME	0x00
#define SET_SFX_VOLUME		0x01

#define SET_NO_LANGUAGE		0x00
#define SET_ENGLISH			0x01
#define SET_ESPERANTO		0x02
// ...

const char volume_array_size = 2;
const char languages_num = 3;

const double deg2rad = 3.1415923565 / 180.0;

struct Color {
	Color();
	Color(float, float, float);
	Color(float, float, float, float);
	Color(unsigned int);

	float r, g, b, a;
};

Color get_color(cfg::DataStructure*, std::string);

class Settings {
public:
	Settings(cfg::DataStructure*);
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

	float get_volume(char);
	void change_language_to(char);

	std::string get_text_for(std::string);

private:
	
	//std::map<std::string, std::string> translations = std::map<std::string, std::string>();

	cfg::DataStructure* translation_pointers[languages_num];
	float volumes[volume_array_size];
};

extern Settings* settings;

#endif // !SETTINGS_H
