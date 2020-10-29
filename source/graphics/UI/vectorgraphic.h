#pragma once
#include "ui_element.h"

struct Line {
	Line(float, float, float, float);

	float x_start, y_start, x_end, y_end;
};

struct Drawing {
	Drawing(float[], int);

	std::vector<Line> lines;
};

void draw_drawing(Drawing*, Screenpos, pixel, pixel);
