#pragma once
#include "vectorgraphic.h"

#define FONT_OVERRIDE_SPACING_X 0x01
#define FONT_OVERRIDE_SPACING_Y 0x02
#define FONT_OVERRIDE_RATIO		0x04
#define FONT_OVERRIDE_FONT_SIZE 0x08

// Need to implement callbacks

struct Typography{
	float spacing_x;
	float spacing_y;
	float ratio;
	float font_size;
};

class Textmesh:
	public UIElement
{
public:
	struct Textpos { unsigned x, y; };

	Textmesh(std::string, FontPtr);

	std::string get_content();
	void set_content(std::string);
	void add_content_animated(std::string, double);
	void set_transform(int, int, int, int, bool=false) override;
	void step() override;

	void set_spacing_x (float);
	void set_spacing_y (float);
	void set_ratio (float);
	void set_font_size(float);

	float get_spacing_x();
	float get_spacing_y();
	float get_ratio();
	float get_font_size();

	unsigned int typography_override_flags;

	FontPtr font;
	Textpos get_text_pos(int);

protected:
	std::vector<float> get_lines() override;

private:
	Typography typography_override;
	std::string content;

	void reset_animation_data();

	std::string og_content = "";
	std::string display_animation_content = "";
	double display_animation_duration;
	double display_animation_t;
	bool display_animation = false;
};

void increment_tp(Textmesh::Textpos*, char);
Textmesh::Textpos get_dimensions(std::string);
