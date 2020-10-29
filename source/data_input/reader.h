#pragma once
#include "data_structure.h"

namespace io {
	extern std::string base_dir;
	extern DataStructure* error_ds;

	//std::string to_string(std::wstring);

	bool initialize(int, char**);

	int parse_int32(std::string, std::vector<DSEnum> = std::vector<DSEnum>());
	double parse_float64(std::string);
	bool parse_bool(std::string);
	std::string parse_string(std::string);
	LongVector parse_vector(std::string, char);

	std::string parse_file_path(std::string, std::string);
	std::string parse_file_path(std::string, std::string);

	std::vector<std::string> read_lines_from_file(std::string, std::string, bool*, bool);
	DataStructure* read_file(std::string, std::string, std::string, DataStructure*, bool);
	DataStructure* analyze_text(std::vector<std::string>, std::string, DataStructure*, std::string);
	DataStructure* analyze_structure(std::vector<std::string>, std::string, DataStructure*, std::string);
}
