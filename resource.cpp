#include "resource.h"

namespace io {
	int parse_binary_to_int(char* buffer, int len, bool little_endian, int offset) {
		int res = 0;
		if (little_endian) {
			for (int i = 0; i < len; i++) {
				int shift = i * 8;
				res += (buffer[i + offset] << shift) & (0xff << shift);
			}
		}
		else {
			for (int i = 0; i < len; i++) {
				int shift = (len - i - 1) * 8;
				res += (buffer[i + offset] << shift) & (0xff << shift);
			}
		}
		return res;
	}

	Resource::Resource() {
		name = "unnamed resource";
		file_path = "\\NOPATH";
	}

	Resource::Resource(DataStructurePtr data) { 
		name = data->get_name();
		file_path = data->get_string("path");
		read_from_file(file_path);
	}

	Resource::~Resource() {
		// Can only be implemented in dericved classes
	}

	bool Resource::read_from_file(std::string path) {
		std::ifstream file;
		if (is_binary) {
			file = std::ifstream(path, std::ios::in | std::ios::binary);
		} else {
			file = std::ifstream(path, std::ios::in);
		}
		if (!file.is_open()) {
			std::cerr << " ! Could not open file " << path << std::endl;
			file.close();
			return false;
		}
		file.seekg(0, std::ios::end);
		int size = file.tellg();
		if (size == 0) {
			std::cerr << " ! File " << path << " has a size of 0 bytes." << std::endl;
		}
		file.seekg(0, file.beg);
		bool res = deserialize(&file, size);
		file.close();
		if (res) {
			file_path = path;
		}
		return res;
	}

	bool Resource::store_in_file(std::string path) {
		std::ofstream file(path, std::ios::out);
		if (!file.is_open())
			return false;
		int size;
		bool res = serialize(&file, &size);
		file.close();
		return res;
	}

	bool Resource::deserialize(char* buffer, int buffer_length) {
		std::istringstream stream = std::istringstream();
		stream.read(buffer, buffer_length);
		stream.seekg(0, stream.beg);
		return deserialize(&stream, buffer_length);
	}

	bool Resource::serialize(char* buffer, int* buffer_length) {
		std::ostringstream stream = std::ostringstream();
		bool res = serialize(&stream, buffer_length);
		stream.write(buffer, *buffer_length);
		return res;
	}

	bool Resource::deserialize(std::istream* in, int buffer_length) { 
		errlog("Cannot deserialize Empty resource");
		return false; 
	}

	bool Resource::serialize(std::ostream* in, int* buffer_length) { 
		errlog("Cannot deserialize Empty resource");
		return false; 
	}

	void Resource::kill() {	}

	ResourceType Resource::get_type() {
		return ResourceType::NONE;
	}

	void Resource::log_note(std::string s, ResourceNoteSeverity severity) {
		ResourceLoadingNote note;
		note.message = s;
		note.severity = severity;

		debug.out << s << std::endl;
		switch (severity) {
		case io::ResourceNoteSeverity::NOTE:
			debug.submit_to(DebugOutput::INFO);
			break;
		case io::ResourceNoteSeverity::WARNING:
			debug.submit_to(DebugOutput::WARNING);
			break;
		case io::ResourceNoteSeverity::ERROR:
		case io::ResourceNoteSeverity::CRITICAL_ERROR:
			debug.submit_to(DebugOutput::ERROR);
			break;
		}

		notes.push_back(note);
	}
}