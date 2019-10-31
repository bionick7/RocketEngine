#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "../data_input/reader.h"
#include "../../common/shader.hpp"
#include "../../common/texture.hpp"

struct Font {
	void kill();

	GLuint Texture_ID;
	std::wstring name;
};

class AssetSet {
public:
	AssetSet(cfg::DataStructure*);
	GLuint get_shader(std::wstring);
	Font get_font(std::wstring);

	GLuint text_shader;
	Font default_font;

	void kill_shader(std::wstring);
	void kill_font(std::wstring);

private:
	std::vector<Font> fonts;
	std::map<std::wstring, GLuint> shaders;
};

extern AssetSet* assets;

#endif // !ASSET_MANAGER_H
