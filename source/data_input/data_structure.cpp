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

	DataStructure::DataStructure(std::string _name, DataStructurePtr _parent, std::string _path)
	{
		set_name(_name);
		w_parent = _parent;
		file_path = _path;
		directory_path = get_directory();

		map_int32 = std::map<std::string, int>();
		map_float64 = std::map<std::string, double>();
		map_string = std::map<std::string, std::string>();
		children = std::map<std::string, DataStructurePtr>();

		map_int32_arr = std::map<std::string, std::vector<int>>();
		map_float64_arr = std::map<std::string, std::vector<double>>();
		map_string_arr = std::map<std::string, std::vector<std::string>>();
		children_arr = std::map<std::string, std::vector<DataStructurePtr>>();

		if (_parent != nullptr) {
			//parent->children.insert({ get_name(), this });
			for (DSEnum e : _parent->active_enums)
				active_enums.push_back(e);
		}
	}

	DataStructure::~DataStructure()
	{/*
		for (std::pair<std::string, DataStructurePtr> pair : children) {
			delete pair.second;
			pair.second = nullptr;
		}
		for (std::pair<std::string, std::vector<DataStructurePtr>> pair : children_arr) {
			for (int i = 0; i < pair.second.size(); i++) {
				if (pair.second[i] != nullptr) {
					delete pair.second[i];
					pair.second[i] = nullptr;
				}
			}
		}*/
	}

	unsigned DataStructure::get_recursion_depht() {
		DataStructurePtr parent = w_parent.lock();
		if (parent && parent != nullptr) {
			return parent->get_recursion_depht() + 1;
		}
		return 0;
	}

	DataStructurePtr DataStructure::get_child(std::string field_name, bool quiet) {
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

	void DataStructure::set_child(std::string field_name, DataStructurePtr value) {
		children.insert({ field_name, value });
	}

	std::vector<DataStructurePtr> DataStructure::get_child_arr(std::string field_name, bool quiet) {
		try {
			return children_arr.at(field_name);
		}
		catch (std::out_of_range) {
			if (!quiet) {
				std::cerr << "No such child: " << to_string(field_name) << std::endl;
			}
			return std::vector<DataStructurePtr>();
		}
	}
	void DataStructure::set_child_arr(std::string field_name, std::vector<DataStructurePtr> value) {
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
		for (std::map<std::string, DataStructurePtr>::iterator it = children.begin(); it != children.end(); it++) {
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
		for (std::map<std::string, std::vector<DataStructurePtr>>::iterator it = children_arr.begin(); it != children_arr.end(); it++) {
			str << pre_spaces << to_string(it->first) << " = (";
			for (DataStructurePtr iter : it->second) {
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
		DataStructurePtr parent = w_parent.lock();
		if (parent && parent != nullptr) {
			unsigned short i = 0;
			while (parent->children.find(value) == parent->children.end())
			{
				name = std::to_string(++i) + value;
			}
		}
	}

	DataStructurePtr DataStructure::get_parent() {
		return w_parent.lock();
	}

	std::vector<DataStructurePtr> DataStructure::get_all_children() {
		std::vector<DataStructurePtr> res = std::vector<DataStructurePtr>();
		for (std::map<std::string, DataStructurePtr>::iterator it = children.begin(); it != children.end(); it++) {
			res.push_back(it->second);
		}
		return res;
	}

	std::string DataStructure::get_directory() {
		return file_path.substr(0, file_path.find_last_of('\\'));
	}

	std::string DataStructure::get_info()
	{
		DataStructurePtr parent = w_parent.lock();
		if (parent && parent != nullptr) {
			return get_name() + " [ROOT]; " + file_path;
		}
		else {
			return get_name() + " (" + parent->get_name() + "); " + file_path;
		}
	}

	// Not sure where toput this
	// Unsafe
	DataStructurePtr get_ds_from_path(std::string path, DataStructurePtr entry) {
		if (entry == nullptr) {
			entry = get_root_ds();
		}

		if (path == "") {
			return entry;
		}

		int next_slash = path.find_first_of("/");
		std::string step = path.substr(0, next_slash);
		std::string rest = path.substr(next_slash + 1);
		if (next_slash < 0) rest = "";

		if (step == ".") {
			return io::get_ds_from_path(rest, entry);
		}
		if (step == "..") {
			return io::get_ds_from_path(rest, entry->get_parent());
		}
		DataStructurePtr child = entry->get_child(step);
		if (child == nullptr) {
			return nullptr;
		}
		return io::get_ds_from_path(rest, child);
	}
}

void print_datastructure(io::DataStructure &d) {
	debug.out << d.get_content_string();
}