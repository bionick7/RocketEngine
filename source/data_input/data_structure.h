#pragma once
#include "debug.h"

#define SETGET_ROUTINE(TYPE, SETNAME, GETNAME, MAP, DEF) \
void SETNAME(std::string field_name, TYPE value) {\
	MAP.insert({ field_name, value });\
}\
TYPE GETNAME(std::string field_name, TYPE def = DEF, bool quiet=false) {\
	auto res = MAP.find(field_name);\
	if (res != MAP.end()) return res->second; \
	if (!quiet) {\
		debug.out << "No such field: \"" << to_string(field_name) << "\" in " << get_info() << std::endl;\
	debug.submit_to(DebugOutput::ERROR);\
	}\
	return def;\
}

namespace io {
	class DataStructure;
	typedef std::shared_ptr<DataStructure> DataStructurePtr;

	std::string to_string(std::string);
	std::string to_string(LongVector);

	struct DSEnum {
		std::string name = "";
		std::vector<std::string> values = std::vector<std::string>();
	};

	class DataStructure
	{
	public:
		DataStructure(std::string, DataStructurePtr, std::string);
		~DataStructure();

		SETGET_ROUTINE(int, set_int, get_int, map_int32, 0)
		SETGET_ROUTINE(double, set_double, get_double, map_float64, 0.0)
		SETGET_ROUTINE(bool, set_bool, get_bool, map_bools, false)
		SETGET_ROUTINE(std::string, set_string, get_string, map_string, "")
		SETGET_ROUTINE(LongVector, set_vector, get_vector, map_vectors, LongVector(0.0, 0.0, 0.0))

		SETGET_ROUTINE(std::vector<int>, set_int_arr, get_int_arr, map_int32_arr, std::vector<int>())
		SETGET_ROUTINE(std::vector<double>, set_double_arr, get_double_arr, map_float64_arr, std::vector<double>())
		SETGET_ROUTINE(std::vector<bool>, set_bool_arr, get_bool_arr, map_bools_arr, std::vector<bool>())
		SETGET_ROUTINE(std::vector<std::string>, set_string_arr, get_string_arr, map_string_arr, std::vector<std::string>())
		SETGET_ROUTINE(std::vector<LongVector>, set_vector_arr, get_vector_arr, map_vectors_arr, std::vector<LongVector>())

		void set_child(std::string, DataStructurePtr);
		DataStructurePtr get_child(std::string, bool = false);

		void set_child_arr(std::string, std::vector<DataStructurePtr>);
		std::vector<DataStructurePtr> get_child_arr(std::string, bool = false);

		std::vector<DataStructurePtr> get_all_children();

		std::string get_content_string();
		unsigned get_recursion_depht();

		std::string get_name();
		void set_name(std::string);

		DataStructurePtr get_parent();
				
		std::string get_directory();
		std::string get_info();
		std::string file_path;
		std::string directory_path;

		std::vector<DSEnum> active_enums = std::vector<DSEnum>();

	private:
		std::map<std::string, int> map_int32;
		std::map<std::string, double> map_float64;
		std::map<std::string, bool> map_bools;
		std::map<std::string, std::string> map_string;
		std::map<std::string, LongVector> map_vectors;
		std::map<std::string, DataStructurePtr> children;

		std::map<std::string, std::vector<int>> map_int32_arr;
		std::map<std::string, std::vector<double>> map_float64_arr;
		std::map<std::string, std::vector<bool>> map_bools_arr;
		std::map<std::string, std::vector<std::string>> map_string_arr;
		std::map<std::string, std::vector<LongVector>> map_vectors_arr;
		std::map<std::string, std::vector<DataStructurePtr>> children_arr;

		std::weak_ptr<DataStructure> w_parent;

		std::string name;
	};

	DataStructurePtr get_ds_from_path(std::string, DataStructurePtr);
	DataStructurePtr get_root_ds();
}

void print_datastructure(io::DataStructure&);
#undef SETGET_ROUTINE