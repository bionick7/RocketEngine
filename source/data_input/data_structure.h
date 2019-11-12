#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include "../core/debug.h"
#include "../core/math_tools.h"

namespace cfg {
	std::string to_string(
	);
	std::string to_string(std::string);
	std::string to_string(LongVector);

	class DataStructure
	{
	public:
		DataStructure(std::string, DataStructure*, std::string);
		~DataStructure();

		int get_int(std::string, int = 0, bool = false);
		double get_double(std::string, double = 0.0, bool = false);
		bool get_bool(std::string, bool = false, bool = false);
		std::string get_string(std::string, std::string = "", bool = false);
		LongVector get_vector(std::string, LongVector, bool = false);
		DataStructure* get_child(std::string, bool = false);
		/////////////////////////////////////////////////////
		std::vector<int> get_int_arr(std::string, std::vector<int> = std::vector<int>(), bool = false);
		std::vector<double> get_double_arr(std::string, std::vector<double> = std::vector<double>(), bool = false);
		std::vector<bool> get_bool_arr(std::string, std::vector<bool> = std::vector<bool>(), bool = false);
		std::vector<std::string> get_string_arr(std::string, std::vector<std::string> = std::vector<std::string>(), bool = false);
		std::vector<LongVector> get_vector_arr(std::string, std::vector<LongVector> = std::vector<LongVector>(), bool = false);
		std::vector<DataStructure*> get_child_arr(std::string, bool = false);

		void set_int(std::string, int);
		void set_double(std::string, double);
		void set_bool(std::string, bool);
		void set_string(std::string, std::string);
		void set_vector(std::string, LongVector);
		void set_child(std::string, DataStructure*);
		///////////////////////////////////////////////
		void set_int_arr(std::string, std::vector<int>);
		void set_double_arr(std::string, std::vector<double>);
		void set_bool_arr(std::string, std::vector<bool>);
		void set_string_arr(std::string, std::vector<std::string>);
		void set_vector_arr(std::string, std::vector<LongVector>);
		void set_child_arr(std::string, std::vector<DataStructure*>);

		std::vector<DataStructure*> get_all_children();

		std::string get_content_string();
		unsigned get_recursion_depht();

		std::string get_name();
		void set_name(std::string);

		DataStructure* parent = NULL;
		
		std::string get_directory();
		std::string file_path;
		std::string directory_path;

	private:
		std::map<std::string, int> map_int32;
		std::map<std::string, double> map_float64;
		std::map<std::string, bool> map_bools;
		std::map<std::string, std::string> map_string;
		std::map<std::string, LongVector> map_vectors;
		std::map<std::string, DataStructure*> children;

		std::map<std::string, std::vector<int>> map_int32_arr;
		std::map<std::string, std::vector<double>> map_float64_arr;
		std::map<std::string, std::vector<bool>> map_bools_arr;
		std::map<std::string, std::vector<std::string>> map_string_arr;
		std::map<std::string, std::vector<LongVector>> map_vectors_arr;
		std::map<std::string, std::vector<DataStructure*>> children_arr;

		std::string name;
	};
}
#endif // DATASTRUCTURE_H