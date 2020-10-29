#pragma once
#include "data_input/data_structure.h"

namespace io {

	enum class ResourceType
	{
		NONE,	// 0
		MESH,
		FONT,
		SHADER
	};

	constexpr int RESOURCE_TYPE_COUNT = 4;

	class Resource abstract {
	public:
		Resource();
		Resource(DataStructure*);
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
	};
}