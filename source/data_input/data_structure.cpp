#include "data_structure.h"


namespace cfg {

	std::string to_string(std::wstring ws) {
		const std::string s(ws.begin(), ws.end());
		return s;
	}

	std::wstring to_wstring(std::string s) {
		const std::wstring ws(s.begin(), s.end());
		return ws;
	}

	std::string to_string(LongVector vec) {
		std::stringstream ss;
		ss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
		return ss.str();
	}

	DataStructure::DataStructure(std::wstring _name, DataStructure* _parent, std::string _path)
	{
		set_name(_name);
		parent = _parent;
		file_path = _path;
		directory_path = get_directory();

		map_int32 = std::map<std::wstring, int>();
		map_float64 = std::map<std::wstring, double>();
		map_string = std::map<std::wstring, std::wstring>();
		children = std::map<std::wstring, DataStructure*>();

		map_int32_arr = std::map<std::wstring, std::vector<int>>();
		map_float64_arr = std::map<std::wstring, std::vector<double>>();
		map_string_arr = std::map<std::wstring, std::vector<std::wstring>>();
		children_arr = std::map<std::wstring, std::vector<DataStructure*>>();

		if (parent != NULL) {
			parent->children.insert({ get_name(), this });
		}
	}

	DataStructure::~DataStructure()
	{
	}

	unsigned DataStructure::get_recursion_depht() {
		if (parent == NULL)
			return 0;
		return parent->get_recursion_depht() + 1;
	}

#pragma region get_set_content

	int DataStructure::get_int(std::wstring field_name, int def, bool quiet) {
		try {
			return map_int32.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such field: " << to_string(field_name) << std::endl;
			}
			return def;
		}
	}

	double DataStructure::get_double(std::wstring field_name, double def, bool quiet) {
		try {
			return map_float64.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such field: " << to_string(field_name) << std::endl;
			}
			return def;
		}
	}

	bool DataStructure::get_bool(std::wstring field_name, bool def, bool quiet) {
		try {
			return map_bools.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such field: " << to_string(field_name) << std::endl;
			}
			return def;
		}
	}

	std::wstring DataStructure::get_string(std::wstring field_name, std::wstring def, bool quiet) {
		try {
			return map_string.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such field: " << to_string(field_name) << std::endl;
			}
			return def;
		}
	}

	LongVector DataStructure::get_vector(std::wstring field_name, LongVector def, bool quiet) {
		try {
			return map_vectors.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such field: " << to_string(field_name) << std::endl;
			}
			return def;
		}
	}

	DataStructure* DataStructure::get_child(std::wstring field_name, bool quiet) {
		try {
			return children.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such child: " << to_string(field_name) << std::endl;
			}
			return NULL;
		}
	}

	std::vector<int> DataStructure::get_int_arr(std::wstring field_name, std::vector<int> def, bool quiet) {
		try {
			return map_int32_arr.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such field: " << to_string(field_name) << std::endl;
			}
			return def;
		}
	}

	std::vector<double> DataStructure::get_double_arr(std::wstring field_name, std::vector<double> def, bool quiet) {
		try {
			return map_float64_arr.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such field: " << to_string(field_name) << std::endl;
			}
			return def;
		}
	}

	std::vector<bool> DataStructure::get_bool_arr(std::wstring field_name, std::vector<bool> def, bool quiet) {
		try {
			return map_bools_arr.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such field: " << to_string(field_name) << std::endl;
			}
			return def;
		}
	}

	std::vector<std::wstring> DataStructure::get_string_arr(std::wstring field_name, std::vector<std::wstring> def, bool quiet) {
		try {
			return map_string_arr.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such field: " << to_string(field_name) << std::endl;
			}
			return def;
		}
	}

	std::vector<LongVector> DataStructure::get_vector_arr(std::wstring field_name, std::vector<LongVector> def, bool quiet) {
		try {
			return map_vectors_arr.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such field: " << to_string(field_name) << std::endl;
			}
			return def;
		}
	}

	std::vector<DataStructure*> DataStructure::get_child_arr(std::wstring field_name, bool quiet) {
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

	void DataStructure::set_int(std::wstring field_name, int value) {
		map_int32.insert({ field_name, value });
	}

	void DataStructure::set_double(std::wstring field_name, double value) {
		map_float64.insert({ field_name, value });
	}

	void DataStructure::set_bool(std::wstring field_name, bool value) {
		map_bools.insert({ field_name, value });
	}

	void DataStructure::set_string(std::wstring field_name, std::wstring value) {
		map_string.insert({ field_name, value });
	}

	void DataStructure::set_vector(std::wstring field_name, LongVector value) {
		map_vectors.insert({ field_name, value });
	}

	void DataStructure::set_child(std::wstring field_name, DataStructure* value) {
		children.insert({ field_name, value });
	}

	void DataStructure::set_int_arr(std::wstring field_name, std::vector<int> value) {
		map_int32_arr.insert({ field_name, value });
	}

	void DataStructure::set_double_arr(std::wstring field_name, std::vector<double> value) {
		map_float64_arr.insert({ field_name, value });
	}

	void DataStructure::set_bool_arr(std::wstring field_name, std::vector<bool> value) {
		map_bools_arr.insert({ field_name, value });
	}

	void DataStructure::set_string_arr(std::wstring field_name, std::vector<std::wstring> value) {
		map_string_arr.insert({ field_name, value });
	}

	void DataStructure::set_vector_arr(std::wstring field_name, std::vector<LongVector> value) {
		map_vectors_arr.insert({ field_name, value });
	}

	void DataStructure::set_child_arr(std::wstring field_name, std::vector<DataStructure*> value) {
		children_arr.insert({ field_name, value });
	}

#pragma endregion

	std::string DataStructure::get_content_string() {
		std::stringstream str;
		str << "\n";
		std::string pre_spaces = "";
		for (unsigned i = 0; i < get_recursion_depht(); i++) { pre_spaces += "  "; }
		str << pre_spaces << "| " << to_string(get_name()) << "\n" << pre_spaces << "--------------------------------------\n";
		pre_spaces += "  ";
		for (std::map<std::wstring, int>::iterator it = map_int32.begin(); it != map_int32.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = " << std::to_string(it->second) << "\n";
		}
		for (std::map<std::wstring, double>::iterator it = map_float64.begin(); it != map_float64.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = " << std::to_string(it->second) << "\n";
		}
		for (std::map<std::wstring, bool>::iterator it = map_bools.begin(); it != map_bools.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = " << std::to_string(it->second) << "\n";
		}
		for (std::map<std::wstring, std::wstring>::iterator it = map_string.begin(); it != map_string.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = " << to_string(it->second) << "\n";
		}
		for (std::map<std::wstring, LongVector>::iterator it = map_vectors.begin(); it != map_vectors.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = " << to_string(it->second) << "\n";
		}
		for (std::map<std::wstring, DataStructure*>::iterator it = children.begin(); it != children.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = " << it->second->get_content_string();
		}

		for (std::map<std::wstring, std::vector<int>>::iterator it = map_int32_arr.begin(); it != map_int32_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (int iter : it->second) {
				str << std::to_string(iter);
			}
			str << ")\n";
		}
		for (std::map<std::wstring, std::vector<double>>::iterator it = map_float64_arr.begin(); it != map_float64_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (double iter : it->second) {
				str << std::to_string(iter);
			}
			str << ")\n";
		}
		for (std::map<std::wstring, std::vector<bool>>::iterator it = map_bools_arr.begin(); it != map_bools_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (bool iter : it->second) {
				str << std::to_string(iter);
			}
			str << ")\n";
		}
		for (std::map<std::wstring, std::vector<std::wstring>>::iterator it = map_string_arr.begin(); it != map_string_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (std::wstring iter : it->second) {
				str << to_string(iter);
			}
			str << ")\n";
		}
		for (std::map<std::wstring, std::vector<LongVector>>::iterator it = map_vectors_arr.begin(); it != map_vectors_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (LongVector iter : it->second) {
				str << to_string(iter);
			}
			str << ")\n";
		}
		for (std::map<std::wstring, std::vector<DataStructure*>>::iterator it = children_arr.begin(); it != children_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (DataStructure* iter : it->second) {
				str << to_string(iter->name);
			}
			str << ")\n";
		}

		return str.str();
	}

	std::wstring DataStructure::get_name() {
		std::wstring res = name;
		while (res[0] >= 48 && res[0] <= 57)
			res = res.substr(1);
		return res;
	}

	void DataStructure::set_name(std::wstring value) {
		name = value;
		if (parent != NULL) {
			unsigned short i = 0;
			while (parent->children.find(value) == parent->children.end())
			{
				name = std::to_wstring(++i) + value;
			}
		}
	}

	std::vector<DataStructure*> DataStructure::get_all_children() {
		std::vector<DataStructure*> res = std::vector<DataStructure*>();
		for (std::map<std::wstring, DataStructure*>::iterator it = children.begin(); it != children.end(); it++) {
			res.push_back(it->second);
		}
		return res;
	}

	std::string DataStructure::get_directory() {
		return file_path.substr(0, file_path.find_last_of('\\'));
	}
}