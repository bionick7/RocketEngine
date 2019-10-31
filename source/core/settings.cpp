#include "settings.h"

Color::Color() {
	r = 0.0f;
	g = 0.0f;
	b = 0.0f;
	a = 0.0f;
}

Color::Color(float _r, float _g, float _b) {
	r = _r;
	g = _g;
	b = _b;
	a = 1.0f;
}

Color::Color(float _r, float _g, float _b, float _a) {
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

Color::Color(unsigned int hex) {
	r = (hex & 0xff000000 >> 24) / 256.0f;
	g = (hex & 0x00ff0000 >> 16) / 256.0f;
	b = (hex & 0x0000ff00 >>  8) / 256.0f;
	a = (hex & 0x000000ff      ) / 256.0f;
}

Color get_color(cfg::DataStructure* ds, std::wstring name) {
	LongVector lv = ds->get_vector(name, LongVector(2.0, 0.0, 0.0), true);
	if (lv.x == 2.0) {
		std::wcerr << L"No such vector: " << name << std::endl;
	}
	else {
		return Color(lv.x, lv.y, lv.z);
	}
}

Settings::Settings(cfg::DataStructure* data) {
	cfg::DataStructure* graphics_structure = data->get_child(L"Graphics");
	background = get_color(graphics_structure, L"Background color");
	draw = get_color(graphics_structure, L"Draw color");
	line_thickness = graphics_structure->get_int(L"Line thickness", 3);
	width = graphics_structure->get_int(L"Screen width", 800);
	height = graphics_structure->get_int(L"Screen height", 600);

	cfg::DataStructure* audio_structure = data->get_child(L"Audio");
	volumes[SET_MASER_VOLUME] = audio_structure->get_double(L"Master volume");
	volumes[SET_SFX_VOLUME] = audio_structure->get_double(L"SFX volume", 0.0, true);

	cfg::DataStructure* ui_structure = data->get_child(L"UI");
	std::wstring language_str = ui_structure->get_string(L"Language");
	if (language_str == L"English") {
		language = SET_ENGLISH;
	} else if (language_str == L"Esperanto") {
		language = SET_ESPERANTO;
	} else {
		language = SET_NO_LANGUAGE;
	}
	std::vector<cfg::DataStructure*> translations = ui_structure->get_child_arr(L"translations");
	for (int i = 0; i < languages_num; i++) {
		translation_pointers[i] = translations[i];
	}

	cfg::DataStructure* controls_structure = data->get_child(L"Controls");
	mouse_sensitivity = controls_structure->get_double(L"Mouse sensitivity") / 2.0;
	initial_fov = controls_structure->get_double(L"Initial fov") * deg2rad / 2.0;
	max_vertical_angle = controls_structure->get_double(L"Max vertical angle") * deg2rad;
	min_vertical_angle = controls_structure->get_double(L"Min vertical angle") * deg2rad;
	scroll_sensitivity = controls_structure->get_double(L"Scroll sensitivity") * 0.087;
}

void Settings::change_language_to(char identifyer) {
	if (identifyer >= languages_num) {
		std::cout << "ID is " << identifyer << ", but cannot be more than " << languages_num << std::endl;
	} else {
		language = identifyer;
	}
}

std::wstring Settings::get_text_for(std::wstring identifier) {
	return translation_pointers[language]->get_string(identifier, identifier, true);
}

float Settings::get_volume(char id) {
	if (id >= volume_array_size) {
		std::cout << "ID is " << id << ", but cannot be more than " << volume_array_size << std::endl;
	}
	float res = volumes[(std::size_t)id];
	if (id != SET_MASER_VOLUME) res *= volumes[SET_MASER_VOLUME];
	return res;
}