#include "asset_manager.h"

Shader::Shader() : Resource::Resource() {
	ID = 0;
}

Shader::Shader(io::DataStructure* data) {
	name = data->get_name();
	ID = LoadShaders(
		(data->get_string("vertex path")).c_str(),
		(data->get_string("fragment path")).c_str()
	);
	file_path = data->get_string("fragment path");
}

void Shader::kill() {
	glDeleteProgram(ID);
}

io::ResourceType Shader::get_type() {
	return io::ResourceType::SHADER;
}

Font::Font() : Resource::Resource() {
	data = nullptr;
}

Font::Font(io::DataStructure* data) {
	name = data->get_name();
	file_path = data->get_string("path");
	read_from_file(file_path);
}

bool Font::deserialize(char* buffer, int size) {
	int local_index = 0, total_index = 0;
	data = new unsigned char* [256];
	bool reading = true;
	for (int i = 0; i < 256; i++) {
		local_index = 0;
		if (reading) {
			while (buffer[local_index + total_index] != '\x00') {
				local_index++;
				if (local_index + total_index >= size) {
					//print_text("Error occured while reading vectorfont \"" + file_path + "\". Unexpected file ending");
					//return false;

					// Not every character is set; that's fine
					reading = false;
				}
			}
			data[i] = new unsigned char[local_index + 1];
			local_index = 0;
			while (buffer[local_index + total_index] != 0) {
				data[i][local_index] = (unsigned char)buffer[local_index++ + total_index];
			}
			total_index += local_index + 1;
		}
		else {
			data[i] = new unsigned char[1];
		}
		data[i][local_index] = '\x00';
	}
	return true;
}

bool Font::deserialize(std::istream* stream, int size) {
	stream->seekg(0);
	char* buffer = new char[size];
	stream->read(buffer, size);
	bool res = Font::deserialize(buffer, size);
	delete[] buffer;
	return res;
}

bool Font::serialize(char* buffer, int* size) {
	size = 0;
	return false;
}

bool Font::serialize(std::ostream* stream, int* size) {
	size = 0;
	return false;
}

io::ResourceType Font::get_type() {
	return io::ResourceType::FONT;
}

void Font::kill() {
	for (int i = 0; i < 256; i++) {
		delete[] data[i];
	}
	delete[] data;
}

/*
AssetSet::AssetSet(io::DataStructure* assets) {
	GLuint id;
	for (io::DataStructure* shader_child : assets->get_child("Shaders")->get_all_children()) {
		id = LoadShaders(
			(shader_child->get_string("vertex path")).c_str(),
			(shader_child->get_string("fragment path")).c_str()
		);
		shaders.insert({ shader_child->get_string("name"), id });
	}

	for (io::DataStructure* font_child : assets->get_child("Fonts")->get_all_children()) {
		std::string image_path = font_child->get_string("image path", "none", true);

		Font font = Font();
		font.name = font_child->get_string("name");
		font.read_from_file(font_child->get_string("data path"));
		vector_fonts.push_back(font);

	}

	for (io::DataStructure* resource_child : assets->get_child("Meshes")->get_all_children()) {
		//io::Resource* temp = new io::Resource();
		//resource_child->get_string("");

		//resources.insert(temp);
	}


	default_vector_font = get_vector_font("Default");
	text_shader = get_shader("*Text");
}

Font AssetSet::get_vector_font(std::string name) {
	for (Font font : vector_fonts) {
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
*/

io::Resource* get_resource_from_data(io::ResourceType r_type, io::DataStructure* data) {
	switch (r_type)
	{
	default:
	case io::ResourceType::NONE:
		return nullptr;
	case io::ResourceType::MESH:
		return new MeshResource(data);
	case io::ResourceType::FONT:
		return new Font(data);
	case io::ResourceType::SHADER:
		return new Shader(data);
	}
}

AssetSet::AssetSet(io::DataStructure* data) {
	content = new std::map<std::string, io::Resource*>[io::RESOURCE_TYPE_COUNT];
	for (int i = 0; i < data->get_all_children().size(); i++) {
		io::DataStructure* child = data->get_all_children()[i];
		std::string default_name = child->get_string("def");
		int type_identifier = child->get_int("type");
		if (type_identifier > 0 && type_identifier <= io::RESOURCE_TYPE_COUNT) {
			for (int j = 0; j < child->get_all_children().size(); j++) {
				io::DataStructure* rsc = child->get_all_children()[j];
				io::Resource* resource = get_resource_from_data((io::ResourceType) type_identifier, rsc);
				if (resource != nullptr) {
					content[type_identifier - 1].insert(std::pair<std::string, io::Resource*>(resource->name, resource));
					if (resource->name == default_name) {
						content[type_identifier - 1].insert(std::pair<std::string, io::Resource*>("__default", resource));
					}
				}
			}
		}
	}
	//std::cout << get_content_string() << std::endl;
}

io::Resource* AssetSet::get(io::ResourceType r_type, std::string r_name, bool quiet) {
	try {
		return content[(int)r_type - 1].at(r_name);
	}
	catch (std::out_of_range) {
		if (!quiet) {
			std::cerr << "No such asset: " << r_name << std::endl;\
		}
		return get_default(r_type);
	}
}

io::Resource* AssetSet::get_default(io::ResourceType r_type) {
	return get(r_type, "__default");
}

void AssetSet::kill(io::ResourceType r_type, std::string r_name) {
	content[(int)r_type - 1][r_name]->kill();
	content[(int)r_type - 1].erase(r_name);
}

std::string AssetSet::get_content_string()
{
	std::string res = "\n|Asset contents\n";
	for (int i = 0; i < io::RESOURCE_TYPE_COUNT - 1; i++)
	{
		res += "\t--- " + std::to_string(i + 1) + " ---\n";
		for (std::map<std::string, io::Resource*>::iterator it = content[i].begin(); it != content[i].end(); it++) {
			res += "\t" + it->first + " (" + it->second->name + ") \n";
		}
	}
	return res + "|End asset contents\n";
}
