#pragma once
#include "data_input/data_structure.h"

namespace io {
	int parse_binary_to_int(char* buffer, int len, bool little_endian, int offset=0);

	enum class ResourceType
	{
		NONE,	// 0
		MESH,
		FONT,
		SHADER,
		AUDIO
	};

	constexpr int RESOURCE_TYPE_COUNT = 5;

	enum class ResourceNoteSeverity
	{
		NOTE,
		WARNING,
		ERROR,
		CRITICAL_ERROR
	};

	struct ResourceLoadingNote {
		ResourceNoteSeverity severity = ResourceNoteSeverity::NOTE;
		std::string message = "";
	};

	class Resource abstract {
	public:
		Resource();
		Resource(DataStructurePtr);
		~Resource();

		bool read_from_file(std::string);
		bool store_in_file(std::string);

		bool virtual deserialize(char*, int);
		bool virtual deserialize(std::istream*, int);
		bool virtual serialize(char*, int*);
		bool virtual serialize(std::ostream*, int*);
		void virtual kill();
		ResourceType virtual get_type();

		std::string file_path;
		std::string name;

		bool is_binary = false;

		void log_note(std::string, ResourceNoteSeverity = ResourceNoteSeverity::WARNING);

		std::vector<ResourceLoadingNote> notes = std::vector<ResourceLoadingNote>();
	};

	typedef std::shared_ptr<Resource> ResourcePtr;
}