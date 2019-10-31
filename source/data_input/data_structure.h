#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include "../core/debug.h"
#include "../core/math_tools.h"

namespace cfg {
	std::string to_string(std::wstring);
	std::wstring to_wstring(std::string);
	std::string to_string(LongVector);

	class DataStructure
	{
	public:
		DataStructure(std::wstring, DataStructure*, std::string);
		~DataStructure();

		int get_int(std::wstring, int = 0, bool = false);
		double get_double(std::wstring, double = 0.0, bool = false);
		bool get_bool(std::wstring, bool = false, bool = false);
		std::wstring get_string(std::wstring, std::wstring = L"", bool = false);
		LongVector get_vector(std::wstring, LongVector, bool = false);
		DataStructure* get_child(std::wstring, bool = false);
		/////////////////////////////////////////////////////
		std::vector<int> get_int_arr(std::wstring, std::vector<int> = std::vector<int>(), bool = false);
		std::vector<double> get_double_arr(std::wstring, std::vector<double> = std::vector<double>(), bool = false);
		std::vector<bool> get_bool_arr(std::wstring, std::vector<bool> = std::vector<bool>(), bool = false);
		std::vector<std::wstring> get_string_arr(std::wstring, std::vector<std::wstring> = std::vector<std::wstring>(), bool = false);
		std::vector<LongVector> get_vector_arr(std::wstring, std::vector<LongVector> = std::vector<LongVector>(), bool = false);
		std::vector<DataStructure*> get_child_arr(std::wstring, bool = false);

		void set_int(std::wstring, int);
		void set_double(std::wstring, double);
		void set_bool(std::wstring, bool);
		void set_string(std::wstring, std::wstring);
		void set_vector(std::wstring, LongVector);
		void set_child(std::wstring, DataStructure*);
		///////////////////////////////////////////////
		void set_int_arr(std::wstring, std::vector<int>);
		void set_double_arr(std::wstring, std::vector<double>);
		void set_bool_arr(std::wstring, std::vector<bool>);
		void set_string_arr(std::wstring, std::vector<std::wstring>);
		void set_vector_arr(std::wstring, std::vector<LongVector>);
		void set_child_arr(std::wstring, std::vector<DataStructure*>);

		std::vector<DataStructure*> get_all_children();

		std::string get_content_string();
		unsigned get_recursion_depht();

		std::wstring get_name();
		void set_name(std::wstring);

		DataStructure* parent = NULL;
		
		std::string get_directory();
		std::string file_path;
		std::string directory_path;

	private:
		std::map<std::wstring, int> map_int32;
		std::map<std::wstring, double> map_float64;
		std::map<std::wstring, bool> map_bools;
		std::map<std::wstring, std::wstring> map_string;
		std::map<std::wstring, LongVector> map_vectors;
		std::map<std::wstring, DataStructure*> children;

		std::map<std::wstring, std::vector<int>> map_int32_arr;
		std::map<std::wstring, std::vector<double>> map_float64_arr;
		std::map<std::wstring, std::vector<bool>> map_bools_arr;
		std::map<std::wstring, std::vector<std::wstring>> map_string_arr;
		std::map<std::wstring, std::vector<LongVector>> map_vectors_arr;
		std::map<std::wstring, std::vector<DataStructure*>> children_arr;

		std::wstring name;
	};
}
#endif // DATASTRUCTURE_H