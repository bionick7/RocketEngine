#include "textmesh.h"
#include "scene_manager.h"

bool is_initialized = false;

Textmesh::Textmesh(std::string _content, FontPtr _font) : UIElement::UIElement() {
	if (!is_initialized) {
		is_initialized = true;
	}

	radio->add_signal("content_set");
	radio->add_signal("animation_finished");
	radio->add_signal("animation_interrupted");
	reset_animation_data();
	set_content(_content);
	font = _font;
}

std::string Textmesh::get_content() {
	return content;
}

#define SETGETTER(SETMETHOD, GETMETHOD, ATTRIBUTE, FLAG)\
void Textmesh::SETMETHOD(float x) {\
	if (x >= 0) { \
		typography_override.ATTRIBUTE = x; \
		typography_override_flags |= FLAG; \
	}  else { \
		typography_override_flags &= !FLAG; \
	} \
} \
float Textmesh::GETMETHOD() { \
	if (typography_override_flags & FLAG) { \
		return typography_override.ATTRIBUTE; \
	} else { \
		return font->ATTRIBUTE; \
	} \
}

SETGETTER(set_spacing_x, get_spacing_x, spacing_x, FONT_OVERRIDE_SPACING_X)
SETGETTER(set_spacing_y, get_spacing_y, spacing_y, FONT_OVERRIDE_SPACING_Y)
SETGETTER(set_ratio, get_ratio, ratio, FONT_OVERRIDE_RATIO)
SETGETTER(set_font_size, get_font_size, font_size, FONT_OVERRIDE_FONT_SIZE)

#undef SETGETTER

void Textmesh::set_content(std::string _content) {
	reset_animation_data();
	content = _content;
	request_update(UI_UPDATE_FLAGS_GEOMETRY);
}

void Textmesh::add_content_animated(std::string _content, double duration) {
	if (display_animation) {
		display_animation_content += _content;
		display_animation_duration += duration;
	}
	else {
		display_animation = true;
		og_content = content;
		display_animation_content = _content;
		display_animation_duration = duration;
	}
	radio->emit_signal("content_set");
}

void Textmesh::set_transform(int _x, int _y, int _sizex, int _sizey, bool wrap_around) {
	if (_sizex <= 0) {
		_sizex = INFINITY;
	}
	UIElement::set_transform(_x, _y, _sizex, _sizey, wrap_around);
}

void Textmesh::step() {
	if (display_animation) {
		display_animation_t += scene_manager->real_time_delta;
		if (display_animation_t >= display_animation_duration) {  // Animation finished
			display_animation = false;
			set_content(og_content + display_animation_content);
			radio->emit_signal("animation_finished");
		}
		else {
			int cutoff = (int)(display_animation_content.size() * display_animation_t / display_animation_duration);

			std::string new_content = og_content + display_animation_content.substr(0, cutoff);
			if (new_content != content) {
				content = new_content;
				request_update(UI_UPDATE_FLAGS_GEOMETRY);
			}
		}
	}
	UIElement::step();
}

void Textmesh::reset_animation_data() {
	content = og_content + display_animation_content;
	if (display_animation) {
		radio->emit_signal("animation_interrupted");
	}
	display_animation = false;
	display_animation_content = "";
	display_animation_duration = 0.0;
	display_animation_t = 0.0;
}

std::vector<float> Textmesh::get_lines() {
	std::vector<float> lines = UIElement::get_lines();
	unsigned int length = content.length();
	if (length == 0)
		return lines;

	Textpos tp = { 0, 0 };
	float sx = get_font_size() / get_ratio();
	float sy = get_font_size();
	float one_plus_spacing_x = 1 + get_spacing_x() / get_font_size();
	float one_plus_spacing_y = 1 + get_spacing_y() / get_font_size();

	// Fill buffers
	for (unsigned int i = 0; i < length; i++) {
		if (content[i] >= 32) {
			//std::cout << content[i] << std::endl;
			std::vector<float> data = font->get_char(content[i]);
			
			float x_off = tp.x * one_plus_spacing_x;
			float y_off = (tp.y + 1) * one_plus_spacing_y;
			if (tp.x * sx <= width && tp.y * sy <= height && data.size() >= 4) {
				for (int j = 0; j < data.size(); j += 4) {
					lines.push_back((data[j + 0] + x_off) * sx);
					lines.push_back((data[j + 1] - y_off) * sy + height);
					lines.push_back((data[j + 2] + x_off) * sx);
					lines.push_back((data[j + 3] - y_off) * sy + height);
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