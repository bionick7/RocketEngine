#include "data_structure.h"


namespace io {

	std::string to_string(std::string ws) {
		const std::string s(ws.begin(), ws.end());
		return s;
	}

	std::string to_string(LongVector vec) {
		std::stringstream ss;
		ss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
		return ss.str();
	}

	DataStructure::DataStructure(std::string _name, DataStructure* _parent, std::string _path)
	{
		set_name(_name);
		parent = _parent;
		file_path = _path;
		directory_path = get_directory();

		map_int32 = std::map<std::string, int>();
		map_float64 = std::map<std::string, double>();
		map_string = std::map<std::string, std::string>();
		children = std::map<std::string, DataStructure*>();

		map_int32_arr = std::map<std::string, std::vector<int>>();
		map_float64_arr = std::map<std::string, std::vector<double>>();
		map_string_arr = std::map<std::string, std::vector<std::string>>();
		children_arr = std::map<std::string, std::vector<DataStructure*>>();

		if (parent != NULL) {
			parent->children.insert({ get_name(), this });
			for (DSEnum e : parent->active_enums)
				active_enums.push_back(e);
		}
	}

	DataStructure::~DataStructure()
	{
		for (std::pair<std::string, DataStructure*> pair : children) {
			delete pair.second;
		}
		for (std::pair<std::string, std::vector<DataStructure*>> pair : children_arr) {
			for (DataStructure* item : pair.second) {
				delete item;
			}
		}
	}

	unsigned DataStructure::get_recursion_depht() {
		if (parent == NULL)
			return 0;
		return parent->get_recursion_depht() + 1;
	}

#define SETGET_ROUTINE(TYPE, SETNAME, GETNAME, MAP) \
	void DataStructure::SETNAME(std::string field_name, TYPE value) {\
		MAP.insert({ field_name, value });\
	}\
	TYPE DataStructure::GETNAME(std::string field_name, TYPE def, bool quiet) {\
		try {\
			return MAP.at(field_name);\
		}\
		catch (std::out_of_range) {\
			if (!quiet) {\
				std::cerr << "No such field: \"" << to_string(field_name) << "\" in " << get_name() << " (" << parent->get_name() << "); " << file_path << std::endl;\
			}\
			return def;\
		}\
	}

	SETGET_ROUTINE(int, set_int, get_int, map_int32)
	SETGET_ROUTINE(double, set_double, get_double, map_float64)
	SETGET_ROUTINE(bool, set_bool, get_bool, map_bools)
	SETGET_ROUTINE(std::string, set_string, get_string, map_string)
	SETGET_ROUTINE(LongVector, set_vector, get_vector, map_vectors)

	SETGET_ROUTINE(std::vector<int>, set_int_arr, get_int_arr, map_int32_arr)
	SETGET_ROUTINE(std::vector<double>, set_double_arr, get_double_arr, map_float64_arr)
	SETGET_ROUTINE(std::vector<bool>, set_bool_arr, get_bool_arr, map_bools_arr)
	SETGET_ROUTINE(std::vector<std::string>, set_string_arr, get_string_arr, map_string_arr)
	SETGET_ROUTINE(std::vector<LongVector>, set_vector_arr, get_vector_arr, map_vectors_arr)

	DataStructure* DataStructure::get_child(std::string field_name, bool quiet) {
		try {
			return children.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such child: " << to_string(field_name) << std::endl;
			}
			return nullptr;
		}
	}

	void DataStructure::set_child(std::string field_name, DataStructure* value) {
		children.insert({ field_name, value });
	}

	std::vector<DataStructure*> DataStructure::get_child_arr(std::string field_name, bool quiet) {
		try {
			return children_arr.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such child: " << to_string(field_name) << std::endl;
			}
			return std::vector<DataStructure*>();
		}
	}
	void DataStructure::set_child_arr(std::string field_name, std::vector<DataStructure*> value) {
		children_arr.insert({ field_name, value });
	}

	std::string DataStructure::get_content_string() {
		std::stringstream str;
		str << "\n";
		std::string pre_spaces = "";
		for (unsigned i = 0; i < get_recursion_depht(); i++) { pre_spaces += "  "; }
		str << pre_spaces << "| " << to_string(get_name()) << "\n" << pre_spaces << "--------------------------------------\n";
		pre_spaces += "  ";
		for (std::map<std::string, int>::iterator it = map_int32.begin(); it != map_int32.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = " << std::to_string(it->second) << "\n";
		}
		for (std::map<std::string, double>::iterator it = map_float64.begin(); it != map_float64.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = " << std::to_string(it->second) << "\n";
		}
		for (std::map<std::string, bool>::iterator it = map_bools.begin(); it != map_bools.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = " << std::to_string(it->second) << "\n";
		}
		for (std::map<std::string, std::string>::iterator it = map_string.begin(); it != map_string.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = " << to_string(it->second) << "\n";
		}
		for (std::map<std::string, LongVector>::iterator it = map_vectors.begin(); it != map_vectors.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = " << to_string(it->second) << "\n";
		}
		for (std::map<std::string, DataStructure*>::iterator it = children.begin(); it != children.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = " << it->second->get_content_string();
		}

		for (std::map<std::string, std::vector<int>>::iterator it = map_int32_arr.begin(); it != map_int32_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (int iter : it->second) {
				str << std::to_string(iter);
			}
			str << ")\n";
		}
		for (std::map<std::string, std::vector<double>>::iterator it = map_float64_arr.begin(); it != map_float64_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (double iter : it->second) {
				str << std::to_string(iter);
			}
			str << ")\n";
		}
		for (std::map<std::string, std::vector<bool>>::iterator it = map_bools_arr.begin(); it != map_bools_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (bool iter : it->second) {
				str << std::to_string(iter);
			}
			str << ")\n";
		}
		for (std::map<std::string, std::vector<std::string>>::iterator it = map_string_arr.begin(); it != map_string_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (std::string iter : it->second) {
				str << to_string(iter);
			}
			str << ")\n";
		}
		for (std::map<std::string, std::vector<LongVector>>::iterator it = map_vectors_arr.begin(); it != map_vectors_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (LongVector iter : it->second) {
				str << to_string(iter);
			}
			str << ")\n";
		}
		for (std::map<std::string, std::vector<DataStructure*>>::iterator it = children_arr.begin(); it != children_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (DataStructure* iter : it->second) {
				str << to_string(iter->name);
			}
			str << ")\n";
		}

		return str.str();
	}

	std::string DataStructure::get_name() {
		std::string res = name;
		while (res[0] >= 48 && res[0] <= 57)
			res = res.substr(1);
		return res;
	}

	void DataStructure::set_name(std::string value) {
		name = value;
		if (parent != nullptr) {
			unsigned short i = 0;
			while (parent->children.find(value) == parent->children.end())
			{
				name = std::to_string(++i) + value;
			}
		}
	}

	std::vector<DataStructure*> DataStructure::get_all_children() {
		std::vector<DataStructure*> res = std::vector<DataStructure*>();
		for (std::map<std::string, DataStructure*>::iterator it = children.begin(); it != children.end(); it++) {
			res.push_back(it->second);
		}
		return res;
	}

	std::string DataStructure::get_directory() {
		return file_path.substr(0, file_path.find_last_of('\\'));
	}
}

void print_datastructure(io::DataStructure* d) {
	std::cout << d->get_content_string();
}