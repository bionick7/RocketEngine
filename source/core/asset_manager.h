#pragma once

#include "data_input/reader.h"
#include "resource.h"
#include "../../common/shader.hpp"
#include "../../common/texture.hpp"
#include "data_input/mesh_resource.h"

class Font : public io::Resource {
public:
	Font();
	Font(io::DataStructure*);

	bool deserialize(char*, int) override;
	bool deserialize(std::istream*, int) override;
	bool serialize(char*, int*) override;
	bool serialize(std::ostream*, int*) override;
	void kill() override;
	io::ResourceType get_type() override;

	unsigned char** data;
};

class Shader : public io::Resource {
public:
	Shader();
	Shader(io::DataStructure*);

	void kill() override;
	io::ResourceType get_type() override;

	GLuint ID;

	operator GLuint () { return ID; }
};

class AssetSet {
public:
	AssetSet(io::DataStructure*);

	io::Resource* get(io::ResourceType, std::string, bool = false);
	io::Resource* get_default(io::ResourceType);
	void kill(io::ResourceType, std::string);
	std::string get_content_string();

private:
	std::map<std::string, io::Resource*>* content;
};

extern AssetSet* assets;
