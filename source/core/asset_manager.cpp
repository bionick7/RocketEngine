#include "asset_manager.h"

void Font::kill() {
	glDeleteTextures(1, &Texture_ID);
}

void VectorFont::kill() {
	for (int i = 0; i < 256; i++) {
		delete[] data[i];
	}
	delete[] data;
}

unsigned char** get_vector_data(std::string path) {
	std::ifstream file(path, std::ios::in | std::ios::binary);
	file.seekg(0, std::ios::end);
	int size = file.tellg();
	file.seekg(0);
	char* buffer = new char[size];
	file.read(buffer, size);
	file.close();
	unsigned char** res = new unsigned char* [256];
	int local_index = 0, total_index = 0;
	for (int i = 0; i < 256; i++) {
		local_index = 0;
		while (buffer[local_index + total_index] != 0) {
			local_index++;
		}
		res[i] = new unsigned char[local_index + 1];
		local_index = 0;
		while (buffer[local_index + total_index] != 0) {
			res[i][local_index] = (unsigned char) buffer[local_index++ + total_index];
		}
		total_index += local_index + 1;
		res[i][local_index] = 0x00;
	}
	return res;
}

AssetSet::AssetSet(cfg::DataStructure* assets) {
	GLuint id;
	for (cfg::DataStructure* shader_child : assets->get_child("Shaders")->get_all_children()) {
		id = LoadShaders(
			(shader_child->get_string("vertex path")).c_str(),
			(shader_child->get_string("fragment path")).c_str()
		);
		shaders.insert({ shader_child->get_string("name"), id });
	}

	for (cfg::DataStructure* font_child : assets->get_child("Fonts")->get_all_children()) {
		std::string image_path = font_child->get_string("image path", "none", true);
		if (image_path == "none") {
			VectorFont font = VectorFont();
			font.name = font_child->get_string("name");
			font.data = get_vector_data(font_child->get_string("data path"));
			vector_fonts.push_back(font);
		} else {
			GLuint texture_id = loadDDS(
				(image_path).c_str()
			);
			Font font = Font();
			font.name = font_child->get_string("name");
			font.Texture_ID = texture_id;
			fonts.push_back(Font(font));
		}
	}

	default_font = get_font("Default");
	default_vector_font = get_vector_font("Default");
	text_shader = get_shader("*Text");
}

Font AssetSet::get_font(std::string name) {
	for (Font font : fonts) {
		if (font.name == name) {
			return font;
		}
	}
	std::cerr << "No such font: " << name << std::endl;
	return default_font;
}

VectorFont AssetSet::get_vector_font(std::string name) {
	for (VectorFont font : vector_fonts) {
		if (font.name == name) {
			return font;
		}
	}
	std::cerr << "No such font: " << name << std::endl;
	return default_vector_font;
}

GLuint AssetSet::get_shader(std::string name) {
	try {
		return shaders.at(name);
	}
	catch (std::out_of_range) {
		std::cerr << "No such shader: " << name << std::endl;
		return 0;
	}
}

void AssetSet::kill_font(std::string name) {
	for (Font font : fonts) {
		if (font.name == name) {
			//fonts.erase(font);			<-- nokucken
			font.kill();
			return;
		}
	}
	std::cerr << "No such font: " << name << std::endl;
}

void AssetSet::kill_shader(std::string name) {
	try {
		GLuint shader_id = shaders.at(name);
		glDeleteProgram(shader_id);
		shaders.erase(name);
	}
	catch (std::out_of_range) {
		std::cerr << "No such shader: " << name << std::endl;
	}
}
