#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "../data_input/reader.h"
#include "../../common/shader.hpp"
#include "../../common/texture.hpp"

struct Font {
	void kill();
	GLuint Texture_ID;
	std::string name;
};

struct VectorFont {
	void kill();
	unsigned char** data;
	std::string name;
};

unsigned char** get_vector_data(std::string);

class AssetSet {
public:
	AssetSet(cfg::DataStructure*);
	GLuint get_shader(std::string);
	Font get_font(std::string);
	VectorFont get_vector_font(std::string);

	GLuint text_shader;
	Font default_font;
	VectorFont default_vector_font;

	void kill_shader(std::string);
	void kill_font(std::string);

private:
	std::vector<Font> fonts;
	std::vector<VectorFont> vector_fonts;
	std::map<std::string, GLuint> shaders;
};

extern AssetSet* assets;

#endif // !ASSET_MANAGER_H
