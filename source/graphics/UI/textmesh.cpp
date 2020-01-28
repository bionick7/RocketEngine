#include "textmesh.h"

bool is_initialized = false;

Textmesh::Textmesh(std::string _content, Font _font) {
	if (!is_initialized) {
		is_initialized = true;
	}

	content = _content;
	font = _font;
}

std::string Textmesh::get_content() {
	return content;
}

void Textmesh::set_content(std::string _content) {
	content = _content;
	request_recalculate();
}

void Textmesh::set_transform(int _x, int _y, int _sizex, int _sizey, bool wrap_around) {
	if (_sizex <= 0) {
		_sizex = INFINITY;
	}
	UIElement::set_transform(_x, _y, _sizex, _sizey, wrap_around);
	request_recalculate();
}

std::vector<float> Textmesh::get_lines() {
	std::vector<float> lines = UIElement::get_lines();
	unsigned int length = content.length();
	if (length == 0)
		return lines;

	Textpos tp = { 0, 0 };
	// Fill buffers
	for (unsigned int i = 0; i < length; i++) {
		if (content[i] >= 32) {
			unsigned char* data = assets->default_vector_font.data[content[i]];
			if ((tp.x + 1) * (1 + font_settings.spacing_x) * font_settings.font_size / font_settings.ratio <= width && (tp.y + 1) * (1 + font_settings.spacing_y) * font_settings.font_size <= height) {
				for (int j = 0; *(data + j) != 0; j += 4) {
					lines.push_back(((*(data + j + 0) - 1) / 254.0f + tp.x * (1 + font_settings.spacing_x)) * font_settings.font_size / font_settings.ratio);
					lines.push_back(((*(data + j + 1) - 1) / 254.0f - (tp.y + 1) * (1 + font_settings.spacing_y)) * font_settings.font_size + height);
					lines.push_back(((*(data + j + 2) - 1) / 254.0f + tp.x * (1 + font_settings.spacing_x)) * font_settings.font_size / font_settings.ratio);
					lines.push_back(((*(data + j + 3) - 1) / 254.0f - (tp.y + 1) * (1 + font_settings.spacing_y)) * font_settings.font_size + height);
				}
			}
		}
		else {
			// Control characters
			switch (content[i])
			{
			case '\a':
				// Ding!
				break;
			case '\f':
				// cls
				break;
			default:
				break;
			}
		}
		increment_tp(&tp, content[i]);
	}

	return lines;
}

Textmesh::Textpos Textmesh::get_text_pos(int index) {
	Textpos res = { 0, 0 };
	
	int len = index < content.length() ? index : content.length();
	for (int i = 0; i < len; i++) {
		increment_tp(&res, content[i]);
	}

	return res;
}

void increment_tp(Textmesh::Textpos* tp, char c) {
	if (c == '\n') {
		tp->y++;
		tp->x = 0;
	}
	else if (c == '\t') {
		tp->x = (tp->x / 4 + 1) * 4;
	}
	else if (c == '\v') {
		tp->y = (tp->y / 4 + 1) * 4;
	}
	else if (c == '\r') {
		tp->x = 0;
	}
	else if (c < 32);
	else {
		tp->x++;
	}
}

Textmesh::Textpos get_dimensions(std::string string) {
	Textmesh::Textpos temp = { 0, 0 };
	Textmesh::Textpos res = { 0, 0 };

	int len = string.length();
	for (int i = 0; i < len; i++) {
		increment_tp(&temp, string[i]);
		if (temp.x > res.x) res.x = temp.x;
		if (temp.y > res.y) res.y = temp.y;
	}

	return res;
}