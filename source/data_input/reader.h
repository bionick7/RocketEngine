#ifndef READER_H
#define READER_H
#include "data_structure.h"

namespace cfg {
	extern std::string base_dir;

	bool initialize(char**);

	int parse_int32(std::wstring);
	double parse_float64(std::wstring);
	bool parse_bool(std::wstring);
	std::wstring parse_string(std::wstring);
	LongVector parse_vector(std::wstring);

	std::string parse_file_path(std::wstring, std::string);
	std::string parse_file_path(std::string, std::string);

	DataStructure* read_file(std::string, std::string, std::wstring, DataStructure*, bool);
	DataStructure* analyze_text(std::vector<std::wstring>, std::wstring, DataStructure*, std::string);
	DataStructure* analyze_structure(std::vector<std::wstring>, std::wstring, DataStructure*, std::string);
}
#endif // !READER_H
