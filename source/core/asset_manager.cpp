#include "asset_manager.h"
#include "data_input/mesh_resource.h"

Shader::Shader() : Resource::Resource() {
	ID = 0;
}

Shader::~Shader(){
	kill();
}

Shader::Shader(io::DataStructurePtr data) {
	name = data->get_name();
	std::string vertex_path = data->get_string("vertex path");
	std::string fragment_path = data->get_string("fragment path");

	ID = load_shaders(vertex_path.c_str(), fragment_path.c_str());

	//delete[] vertex_path;
	//delete[] fragment_path;

	file_path = data->get_string("fragment path");
}

void Shader::kill() {
	glDeleteProgram(ID);
}

io::ResourceType Shader::get_type() {
	return io::ResourceType::SHADER;
}

Font::Font() : Resource::Resource() {
	is_binary = true;
}

Font::Font(io::DataStructurePtr dsdata) {
	is_binary = true;
	name = dsdata->get_name();
	file_path = dsdata->get_string("path");
	read_from_file(file_path);
	spacing_x = dsdata->get_double("x spacing", 0.0);
	spacing_y = dsdata->get_double("y spacing", 0.0);
	ratio = dsdata->get_double("ratio", 1.0);
	font_size = dsdata->get_double("size", 12.0);
}

Font::~Font(){
	kill();
}

//bool Font::deserialize(char* buffer, int size) {
//	int local_index = 0, total_index = 0;
//	data = new float* [256];
//	bool reading = true;  // if there is still space left
//	for (int i = 0; i < 256; i++) {
//		local_index = 0;
//		if (reading) {
//			// Mainly just check, if the file length isn't exceded
//			while (buffer[local_index + total_index] != '\x00') {
//				local_index++;
//				if (local_index + total_index >= size) {
//					//logerr("Error occured while reading vectorfont \"" + file_path + "\". Unexpected file ending");
//					//return false;
//
//					// Not every character is set; that's fine
//					reading = false;
//				}
//			}
//			data[i] = new float[local_index + 1];
//			local_index = 0;
//			while (buffer[local_index + total_index] != 0) {
//				int read_in_0 = (int)((unsigned char)(buffer[local_index + total_index + 0])) - 32,
//					read_in_1 = (int)((unsigned char)(buffer[local_index + total_index + 1])) - 32,
//					read_in_2 = (int)((unsigned char)(buffer[local_index + total_index + 2])) - 32,
//					read_in_3 = (int)((unsigned char)(buffer[local_index + total_index + 3])) - 32;
//
//
//				if (read_in_1 > read_in_3) {
//					data[i][local_index + 0] = (float) read_in_2 / 223.0f;
//					data[i][local_index + 1] = (float) read_in_3 / 223.0f;
//					data[i][local_index + 2] = (float) read_in_0 / 223.0f;
//					data[i][local_index + 3] = (float) read_in_1 / 223.0f;
//				} else {
//					data[i][local_index + 0] = (float) read_in_0 / 223.0f;
//					data[i][local_index + 1] = (float) read_in_1 / 223.0f;
//					data[i][local_index + 2] = (float) read_in_2 / 223.0f;
//					data[i][local_index + 3] = (float) read_in_3 / 223.0f;
//				}
//				
//				//std::cout << data[i][local_index] << ", ";
//				local_index += 4;
//			}
//			//std::cout << " >> " << (i >= 32 ? (char)(i) : '-') << std::endl;
//			total_index += local_index + 1;
//		}
//		else {
//			data[i] = new float[1];
//			data[i][0] = 0.0;
//		}
//		data[i][local_index] = -1;
//	}
//	return true;
//}

bool Font::deserialize(char* buffer, int size) {
	int local_index = 0, total_index = 0;
	data = new std::vector<float>[256];
	bool reading = true;  // if there is still space left
	for (int i = 0; i < 256; i++) {
		local_index = 0;
		if (reading) {
			// Mainly just check, if the file length isn't exceded
			while (buffer[local_index + total_index] != '\x00') {
				local_index++;
				if (local_index + total_index >= size) {
					//logerr("Error occured while reading vectorfont \"" + file_path + "\". Unexpected file ending");
					//return false;

					// Not every character is set; that's fine
					reading = false;
				}
			}
			data[i] = std::vector<float>();
			local_index = 0;
			while (buffer[local_index + total_index] != 0) {
				int read_in_0 = (int)((unsigned char)(buffer[local_index + total_index + 0])) - 32,
					read_in_1 = (int)((unsigned char)(buffer[local_index + total_index + 1])) - 32,
					read_in_2 = (int)((unsigned char)(buffer[local_index + total_index + 2])) - 32,
					read_in_3 = (int)((unsigned char)(buffer[local_index + total_index + 3])) - 32;


				if (read_in_1 > read_in_3) {
					data[i].push_back((float) read_in_2 / 223.0f);
					data[i].push_back((float) read_in_3 / 223.0f);
					data[i].push_back((float) read_in_0 / 223.0f);
					data[i].push_back((float) read_in_1 / 223.0f);
				} else {
					data[i].push_back((float) read_in_0 / 223.0f);
					data[i].push_back((float) read_in_1 / 223.0f);
					data[i].push_back((float) read_in_2 / 223.0f);
					data[i].push_back((float) read_in_3 / 223.0f);
				}
				
				//std::cout << data[i][local_index] << ", ";
				local_index += 4;
			}
			//std::cout << " >> " << (i >= 32 ? (char)(i) : '-') << std::endl;
			total_index += local_index + 1;
		}
		else {
			data[i] = std::vector<float>();
		}
	}
	return true;
}

bool Font::deserialize(std::istream* stream, int size) {
	stream->seekg(0);
	char* buffer = new char[size];
	stream->read(buffer, size);
	/*char chr = 0;
	for (int i = 0; i < size; i++) {
		if (buffer[i] == '\x00') {
			//std::cout << chr++ << std::endl;
		} else {
			//std::cout << (int) buffer[i] << ", ";
		}
	}*/

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

std::vector<float> Font::get_char(char c) {
	//return data[33];
	return data[(int)c];
}

io::ResourceType Font::get_type() {
	return io::ResourceType::FONT;
}

void Font::kill() {
	if (data != nullptr) {
		delete[] data;
	}
}

/*
AssetSet::AssetSet(io::DataStructurePtr assets) {
	GLuint id;
	for (io::DataStructurePtr shader_child : assets->get_child("Shaders")->get_all_children()) {
		id = LoadShaders(
			(shader_child->get_string("vertex path")).c_str(),
			(shader_child->get_string("fragment path")).c_str()
		);
		shaders.insert({ shader_child->get_string("name"), id });
	}

	for (io::DataStructurePtr font_child : assets->get_child("Fonts")->get_all_children()) {
		std::string image_path = font_child->get_string("image path", "none", true);

		Font font = Font();
		font.name = font_child->get_string("name");
		font.read_from_file(font_child->get_string("data path"));
		vector_fonts.push_back(font);

	}

	for (io::DataStructurePtr resource_child : assets->get_child("Meshes")->get_all_children()) {
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

AudioResource::AudioResource() {

}

AudioResource::AudioResource(io::DataStructurePtr data) {
	name = data->get_name();
	file_path = data->get_string("path");
	is_binary = true;
	read_from_file(file_path);
}

AudioResource::~AudioResource() {
	kill();
}


bool AudioResource::deserialize(char* text_data, int size) {

	// Check if it's riff data
	
	if (strncmp(text_data, "RIFF", 4) != 0) {
		errlog("Not a RIFF file");
		return false;
	}

}

bool AudioResource::deserialize(std::istream* stream, int size) {
	is_valid = true;

	char* buffer = new char[4];
	stream->seekg(0, stream->beg);
	stream->read(buffer, 4); // RIFF
	if (strncmp(buffer, "RIFF", 4) != 0) {
		log_note("Not a RIFF file", io::ResourceNoteSeverity::CRITICAL_ERROR);
		is_valid = false;
		return false;
	}

	stream->read(buffer, 4); // size
	int full_size = io::parse_binary_to_int(buffer, 4, true);
	if (size != full_size + 36) {
		log_note("Read size does not match full size");
	}
	stream->read(buffer, 4); // WAVE
	if (strncmp(buffer, "WAVE", 4) != 0) {
		log_note("Not a WAVE file");
		is_valid = false;
	}

	// Subchunk 1
	stream->read(buffer, 4); // subchunk1ID
	if (strncmp(buffer, "fmt", 3) != 0) {
		log_note("chunk not named fmt");
	}
	stream->read(buffer, 4); // subchunk1Size
	int chunk1_size = io::parse_binary_to_int(buffer, 4, true);
	stream->read(buffer, 4); // Audioformat + NumChannel
	int format = io::parse_binary_to_int(buffer, 2, true);
	int channel_count = io::parse_binary_to_int(buffer, 2, true, 2);

	if (format != 1) {
		log_note("Can't read compressed WAVs jet");
		is_valid = false;
	}

	stream->read(buffer, 4); // samplerate
	sample_rate = io::parse_binary_to_int(buffer, 4, true);
	stream->read(buffer, 4); // byterate
	int byterate = io::parse_binary_to_int(buffer, 4, true);  // Bytes per second. Redundant, but usefull for timelength calcs
	stream->read(buffer, 4); // blockalign + bitspsample
	int blockalign = io::parse_binary_to_int(buffer, 2, true);
	int bitsample = io::parse_binary_to_int(buffer, 2, true, 2);

	if (bitsample == 8 && channel_count == 1) {
		audio_format = AL_FORMAT_MONO8;
	} else if (bitsample == 8 && channel_count == 2) {
		audio_format = AL_FORMAT_STEREO8;
	} else if (bitsample == 16 && channel_count == 1) {
		audio_format = AL_FORMAT_MONO16;
	} else if (bitsample == 16 && channel_count == 2) {
		audio_format = AL_FORMAT_STEREO16;
	} else {
		log_note("No AL configuration: " + std::to_string(bitsample) + " bits on " + std::to_string(channel_count) + " Channels");
		is_valid = false;
	}

	stream->seekg(chunk1_size + 20);  // Skip any possible additional parameters
	// Subchunk 2 (Data)
	stream->read(buffer, 4); // subchunk1ID
	if (strncmp(buffer, "data", 4) != 0) {
		log_note("chunk not named data");
	}
	stream->read(buffer, 4); // subchunk1Size
	int data_size = std::min(io::parse_binary_to_int(buffer, 4, true), size - chunk1_size - 28);

	char* sound_data = new char[data_size];
	stream->read(sound_data, data_size);
	
	alGenBuffers(1, &ID);
	if (alGetError() != AL_NO_ERROR) {
		log_note("AL error while creating data buffer");
	}
	alBufferData(ID, audio_format, (void*) sound_data, data_size, sample_rate);
	if (alGetError() != AL_NO_ERROR) {
		log_note("AL error while filling data buffer");
	}
	delete[] buffer;
	delete[] sound_data;
}

void AudioResource::kill() {
	alDeleteBuffers(1, &ID);
}

io::ResourceType AudioResource::get_type() {
	return io::ResourceType::AUDIO;
}

io::ResourcePtr get_resource_from_data(io::ResourceType r_type, io::DataStructurePtr data) {
	switch (r_type)
	{
	default:
	case io::ResourceType::NONE:
		return nullptr;
	case io::ResourceType::MESH:
		return std::make_shared<MeshResource>(data);
	case io::ResourceType::FONT:
		return std::make_shared<Font>(data);
	case io::ResourceType::SHADER:
		return std::make_shared<Shader>(data);
	case io::ResourceType::AUDIO:
		return std::make_shared<AudioResource>(data);
	}
}

AssetSet::AssetSet() {
	content = new std::map<std::string, io::ResourcePtr>[io::RESOURCE_TYPE_COUNT];
}

AssetSet::~AssetSet() {
	delete[] content;
}

void AssetSet::interprete_signal(RadioSignal) {
}

void AssetSet::reload(io::DataStructurePtr data) {
	for (io::DataStructurePtr child : data->get_all_children()) {
		std::string default_name = child->get_string("def");
		int type_identifier = child->get_int("type");
		if (type_identifier > 0 && type_identifier <= io::RESOURCE_TYPE_COUNT) {
			for (int j = 0; j < child->get_all_children().size(); j++) {
				io::DataStructurePtr rsc = child->get_all_children()[j];
				io::ResourcePtr resource = get_resource_from_data((io::ResourceType) type_identifier, rsc);
				if (resource != nullptr) {
					content[type_identifier - 1].insert(std::pair<std::string, io::ResourcePtr>(resource->name, resource));
					if (resource->name == default_name) {
						content[type_identifier - 1].insert(std::pair<std::string, io::ResourcePtr>("__default", resource));
					}
				}
			}
		}
	}
	//std::cout << get_content_string() << std::endl;
}

io::ResourcePtr AssetSet::get(io::ResourceType r_type, std::string r_name, bool quiet) {
	if ((int) r_type > io::RESOURCE_TYPE_COUNT) {
		if (!quiet) {
			errlog("Invalid resource type: (" + std::to_string((int) r_type) + ")");
		}
		return nullptr;
	}
	try {
		return content[(int)r_type - 1].at(r_name);
	}
	catch (std::out_of_range) {
		if (!quiet) {
			errlog("No such asset: " + r_name);
		}
		return get_default(r_type);
	}
}

io::ResourcePtr AssetSet::get_default(io::ResourceType r_type) {
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
		for (std::map<std::string, io::ResourcePtr>::iterator it = content[i].begin(); it != content[i].end(); it++) {
			res += "\t" + it->first + " (" + it->second->name + ") \n";
		}
	}
	return res + "|End asset contents\n";
}

io::DataStructurePtr io::get_root_ds() {
	return assets->root_ds;
}