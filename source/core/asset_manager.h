#pragma once

#include "data_input/reader.h"
#include "resource.h"
#include "../../common/shader.hpp"
#include "../../common/texture.hpp"
#include "manager.h"

class Font : public io::Resource {
public:
	Font();
	Font(io::DataStructurePtr);
	~Font();

	bool deserialize(char*, int) override;
	bool deserialize(std::istream*, int) override;
	bool serialize(char*, int*) override;
	bool serialize(std::ostream*, int*) override;
	void kill() override;
	io::ResourceType get_type() override;

	std::vector<float> get_char(char c);
	float spacing_x = 0.0f;
	float spacing_y = 0.0f;
	float ratio = 1.333f;
	float font_size = 10.0f;
	
	bool all_caps;

private:
	// TODO memory block
	std::vector<float>* data = nullptr;
};

class Shader : public io::Resource {
public:
	Shader();
	Shader(io::DataStructurePtr);
	~Shader();

	void kill() override;
	io::ResourceType get_type() override;

	GLuint ID;

	operator GLuint () { return ID; }
};

class AudioResource : public io::Resource {
public:
	AudioResource();
	AudioResource(io::DataStructurePtr);
	~AudioResource();

	bool deserialize(char*, int) override;
	bool deserialize(std::istream*, int) override;
	void kill() override;
	io::ResourceType get_type() override;

	ALuint ID;
	ALuint sample_rate;
	ALenum audio_format;

	bool is_valid = false;

	operator ALuint () { return ID; }
};

class AssetSet : 
	public Manager {
public:
	AssetSet();
	~AssetSet();

	void reload(io::DataStructurePtr);

	io::ResourcePtr get(io::ResourceType, std::string, bool = false);
	io::ResourcePtr get_default(io::ResourceType);

	template<typename T>
	std::shared_ptr<T> get(io::ResourceType resource_type, std::string name, bool quiet = false) {
		return std::dynamic_pointer_cast<T>(get(resource_type, name, quiet));
	}
	template<typename T>
	std::shared_ptr<T> get_default(io::ResourceType resource_type) {
		return std::dynamic_pointer_cast<T>(get_default(resource_type));
	}

	void kill(io::ResourceType, std::string);
	std::string get_content_string();

	void interprete_signal(RadioSignal) override;

	io::DataStructurePtr root_ds;

private:
	std::map<std::string, io::ResourcePtr>* content;
};

class MeshResource;
typedef std::shared_ptr<Font> FontPtr;
typedef std::shared_ptr<Shader> ShaderPtr;
typedef std::shared_ptr<AudioResource> AudioResourcePtr;
typedef std::shared_ptr<MeshResource> MeshResourcePtr;

extern AssetSet* assets;
