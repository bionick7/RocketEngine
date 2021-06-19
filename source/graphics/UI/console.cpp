#include "console.h"

// Truncates a string, so that only the last n lines are left
std::string last_lines(std::string s, int line_num) {
	int line_count = 0;
	for (unsigned i = 0; i < s.length(); i++) {
		if (s[i] == '\n') {
			line_count++;
		}
	}
	if (line_count <= line_num)
		return s;

	// Pretty inefficient probably, but eh ..
	std::vector<std::string> lines = std::vector<std::string>();
	std::string line = std::string("");
	for (unsigned i = 0; i < s.length(); i++) {
		if (s[i] == '\n') { 
			lines.push_back(line); 
			line.clear();
		}
		else line += s[i];
	}

	std::string res = std::string("");
	for (int i = std::max(0, (int) lines.size() - line_num); i < lines.size(); i++) {
		res += lines[i] + "\n";
	}

	return res;
}

Console::Console(FontPtr f,GLFWwindow* window) {
	input = new TextInput(f, window);
	archieved = new Textmesh("ARCHIEVE", f);
	archieved->_debug_label = "CONSOLE_ARCHIEVE";
	archieved->radio->connect("animation_finished", radio, RadioSignal("update_lines_culling"));
	archieved->radio->connect("animation_interrupted", radio, RadioSignal("stop_sound"));
	archieved->radio->connect("animation_finished", radio, RadioSignal("stop_sound"));
	input->_debug_label = "CONSOLE_INPUT";

	dependency_count = 2;
	dependencies = new UIElement*[dependency_count];
	dependencies[0] = input;
	dependencies[1] = archieved;

	input->parent = this;
	archieved->parent = this;

	//std::cout << archieved->get_font_size() << "; " << (1 + archieved->get_spacing_y()) << std::endl;
	line_height = archieved->get_font_size() + archieved->get_spacing_y();

	scripting_manager->console_flags |= CONSOLE_FLAG_CLEAR;

	audio_player = new AudioPlayer();
	audio_player->set_loop(true);

	typing_sound = assets->get<AudioResource>(io::ResourceType::AUDIO, "KeyStroke");
}

Console::~Console() {
	delete audio_player;
}

void Console::set_transform_anchor_margin(int margin_left_, int margin_right_, int margin_up_, int margin_down_,
	float anchor_left_, float anchor_right_, float anchor_up_, float anchor_down_) {
	UIElement::set_transform_anchor_margin(margin_left_, margin_right_, margin_up_, margin_down_, anchor_left_, anchor_right_, anchor_up_, anchor_down_);
	
	archieved->set_transform_anchor_margin(
		margin_left_ + border, margin_right_ - border, margin_up_ - border, margin_down_ + border * 2 + input_height + input_archive_gap,
		anchor_left_, anchor_right_, anchor_up_, anchor_down_
	);

	input->set_transform_anchor_margin(
		margin_left_ + border, margin_right_ - border, margin_down_ + input_archive_gap + input_height + border * 2, margin_down_ + border,
		anchor_left_, anchor_right_, anchor_down_, anchor_down_
	);

	max_lines = (unsigned int)(std::max((float) archieved->height, 0.0f) / line_height);
	max_lines = std::max(max_lines - 1, 0u);

	//archieved->set_transform(x + border, y + border + input_height + input_archive_gap, width - border * 2, archive_height);
	//input->set_transform(x + border, y + border, width - border * 2, input_height - border * 2);
}

void Console::set_transform(int _x, int _y, int _sizex, int _sizey, bool wrap_around) {
	UIElement::set_transform(_x, _y, _sizex, _sizey, wrap_around);

	float archive_height = _sizey - border * 2 - input_height - input_archive_gap;
	max_lines = (unsigned int) (archive_height / line_height);

	archieved->set_transform(_x + border, _y + border + input_height + input_archive_gap, _sizex - border*2, archive_height);
	input->set_transform(_x + border, _y + border, _sizex - border*2, input_height - border*2);
}

void Console::step() {
	update_flags |= (CONSOLE_FLAG_UPDATE & scripting_manager->console_flags) * UI_UPDATE_FLAGS_GEOMETRY;
	if (update_flags & UI_UPDATE_FLAGS_GEOMETRY || 
		input->update_flags & UI_UPDATE_FLAGS_GEOMETRY || 
		CONSOLE_FLAG_UPDATE & scripting_manager->console_flags) {
		recalculate_internal();
	}
	// clear
	if (CONSOLE_FLAG_CLEAR & scripting_manager->console_flags) {
		archieved->set_content("");
		update_flags |= UI_UPDATE_FLAGS_GEOMETRY;
		scripting_manager->console_flags &= !CONSOLE_FLAG_CLEAR;
	}
	scripting_manager->console_flags &= !CONSOLE_FLAG_UPDATE;

	UIElement::step();
}

void Console::recalculate_internal(){
	if (input->get_content().length() > 0) {
		char input_last = input->get_content()[input->get_content().length() - 1];
		if (input_last == '\n') { // Hit enter
			interprete(input->get_content());
			input->set_content("");
			input->pointer = 0;
		}
	}

	// Update console text
	std::string txt = scripting_manager->get_console_text();
	if (txt != "") {
		archieved->add_content_animated(txt, txt.size() * 0.016);
		audio_player->play_static(*typing_sound);
	}

	//std::cout << "((" << scripting_manager->get_console_text() << "||" << last_lines(archieved->get_content() + scripting_manager->get_console_text(), max_lines) << "))" << std::endl;
}

void Console::interprete(std::string inp) {
	UIElement::interprete_signal(inp);
	scripting_manager->console_interpreteation(inp.substr(0, inp.size()-1));
}

void Console::character_callback(GLFWwindow* window, unsigned int codepoint) {
	input->character_callback(window, codepoint);
}

void Console::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	input->key_callback(window, key, scancode, action, mods);
}

void Console::interprete_signal(RadioSignal signal) {
	if (signal.identifier == "update_lines_culling") {
		std::string new_content = last_lines(archieved->get_content(), max_lines);
		archieved->set_content(new_content);
	}
	if (signal.identifier == "stop_sound") {
		audio_player->stop();
	}
}
