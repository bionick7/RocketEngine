#ifndef READER_H
#define READER_H
#include "data_structure.h"

namespace cfg {
	extern std::string base_dir;

	//std::string to_string(std::wstring);

	bool initialize(char**);

	int parse_int32(std::string);
	double parse_float64(std::string);
	bool parse_bool(std::string);
	std::string parse_string(std::string);
	LongVector parse_vector(std::string);

	std::string parse_file_path(std::string, std::string);
	std::string parse_file_path(std::string, std::string);

	DataStructure* read_file(std::string, std::string, std::string, DataStructure*, bool);
	DataStructure* analyze_text(std::vector<std::string>, std::string, DataStructure*, std::string);
	DataStructure* analyze_structure(std::vector<std::string>, std::string, DataStructure*, std::string);
}
#endif // !READER_H
