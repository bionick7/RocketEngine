#ifndef TEXTMESH_H
#define TEXTMESH_H

#include "vectorgraphic.h"

class Textmesh:
	public UIElement
{
public:
	struct Textpos { unsigned x, y; };
	struct FontAdjustable { float spacing_x, spacing_y, ratio = 1.2f, font_size = 10.0f; };

	Textmesh(std::string, Font);

	std::string get_content();
	void set_content(std::string);
	void set_transform(int, int, int, int, bool=false) override;

	Font font;
	FontAdjustable font_settings = {.4f, .7f, 1.2f, 10.0f};
	Textpos get_text_pos(int);

protected:

	std::vector<float> get_lines() override;

private:
	std::string content;
};

void increment_tp(Textmesh::Textpos*, char);
Textmesh::Textpos get_dimensions(std::string);

#endif // !TEXTMESH_H