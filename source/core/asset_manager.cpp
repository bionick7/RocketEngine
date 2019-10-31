#include "asset_manager.h"

void Font::kill() {
	glDeleteTextures(1, &Texture_ID);
}

AssetSet::AssetSet(cfg::DataStructure* assets) {
	GLuint id;
	for (cfg::DataStructure* shader_child : assets->get_child(L"Shaders")->get_all_children()) {
		id = LoadShaders(
			cfg::to_string(shader_child->get_string(L"vertex path")).c_str(),
			cfg::to_string(shader_child->get_string(L"fragment path")).c_str()
		);
		shaders.insert({ shader_child->get_string(L"name"), id });
	}

	for (cfg::DataStructure* font_child : assets->get_child(L"Fonts")->get_all_children()) {
		GLuint texture_id = loadDDS(
			cfg::to_string(font_child->get_string(L"image path")).c_str()
		);
		Font font = Font();
		font.name = font_child->get_string(L"name");
		font.Texture_ID = texture_id;
		fonts.push_back(Font(font));
	}

	default_font = get_font(L"Default");
	text_shader = get_shader(L"*Text");
}

Font AssetSet::get_font(std::wstring name) {
	for (Font font : fonts) {
		if (font.name == name) {
			return font;
		}
	}
	std::cerr << "No such font: " << cfg::to_string(name) << std::endl;
	return default_font;
}

GLuint AssetSet::get_shader(std::wstring name) {
	try {
		return shaders.at(name);
	}
	catch (std::out_of_range) {
		std::cerr << "No such shader: " << cfg::to_string(name) << std::endl;
		return 0;
	}
}

void AssetSet::kill_font(std::wstring name) {
	for (Font font : fonts) {
		if (font.name == name) {
			//fonts.erase(font);			<-- nokucken
			font.kill();
			return;
		}
	}
	std::cerr << "No such font: " << cfg::to_string(name) << std::endl;
}

void AssetSet::kill_shader(std::wstring name) {
	try {
		GLuint shader_id = shaders.at(name);
		glDeleteProgram(shader_id);
		shaders.erase(name);
	}
	catch (std::out_of_range) {
		std::cerr << "No such shader: " << cfg::to_string(name) << std::endl;
	}
}
