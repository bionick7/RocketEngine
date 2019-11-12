#include "reader.h"

using namespace std;

namespace cfg {

	string base_dir = "";

	//std::string to_string(std::wstring inp) {
	//
	//}

	void replace_character(string* reference_string, char char_1, char char_2) {
		string temp_str = *reference_string;
		for (unsigned i = 0; i < reference_string->size(); i++) {
			if (temp_str[i] == char_1) {
				temp_str[i] = char_2;
			}
		}
		*reference_string = temp_str;
	}

	void purge_char(string* reference_string, char char_1) {
		unsigned size = reference_string->size();
		for (unsigned i = 0; i < size; i++) {
			if ((*reference_string)[i] == char_1) {
				reference_string->erase(reference_string->begin() + i, reference_string->begin() + i + 1);
				size--;
				i--;
			}
		}
	}

	bool contains(string inp, string match) {
		return inp.find(match) != -1;
	}

	bool initialize(char** command_args) {
		char basepath[255] = "";
		std::cout << *(command_args[0]) << std::endl;
		_fullpath(basepath, command_args[0], sizeof(basepath));
		std::string base_path_string(basepath);
		base_dir = base_path_string.substr(0, base_path_string.find_last_of('\\'));

		std::cout << "Base dir is: " << base_dir << std::endl;

		return true;
	}

	string parse_file_path(string inp, string base) {

		replace_character(&inp, '/', '\\');
		while (inp[0] == '\\' && inp[1] == '<')
		{
			inp = inp.substr(2);
			base = base.substr(0, base.find_last_of('\\'));
		}

		return base + "\\" + inp;
	}

	DataStructure* read_file(string filename, string base, string ds_name, DataStructure* parent, bool full_path) {

		if (!full_path) {
			filename = parse_file_path(filename, base);
		}
		
		vector<string> all_lines = vector<string>();
		string line;

		//std::cout << "Reading: " << filename << std::endl;
		ifstream file(filename, ios::in);
		if (file.is_open())
		{
			while (std::getline(file, line))
			{
				all_lines.push_back(line);
			}
			file.close();
		}
		else {
			cerr << "couldn't open " << filename << endl;
		}

		return analyze_text(all_lines, ds_name, parent, filename);
	}

	DataStructure* analyze_text(vector<string> text_lines, string name, DataStructure* parent, string path) {
		int txt_size = text_lines.size();

		const string placeholder(1, 0xd1);

		// Eliminate all comments, spaces and Tabs
		// Merges '§'-separated lines
		vector<string> txt_lines2 = vector<string>(txt_size);
		// Set everything to empty
		for (int i = 0; i < txt_size; i++)
			txt_lines2[i] = "";
		// What was added before previously
		string base_string = "";
		bool line_string_open = false;
		for (int i = 0; i < txt_size; i++) {
			string line = text_lines[i];
			replace_character(&line, '\t', ' ');
			if (!line_string_open) {
				purge_char(&line, ' ');
			}
			if (contains(line, "//")) {
				line = line.substr(0, line.find("//"));
			}
			if (contains(line, "\"\"")) {
				line_string_open = !line_string_open;
			}
			if (line.size() > 0 && line[line.size() - 1] == '§') {
				base_string += line.substr(0, line.length() - 1);
				goto END;
			}
			if (line_string_open) {
				base_string += line + placeholder;
				goto END;
			}
			txt_lines2[i] = base_string + line;
			base_string = "";
		END:;
		}
		//Debug.Log(string.Join("\n", txt_lines2));

		//Identifiing empty lines and new commands
		for (string line : txt_lines2) {
			if (line == "") txt_size--;
		}

		//Removing empty lines
		vector<string> act_lines;
		for (unsigned i = 0; i < txt_lines2.size(); i++) {
			if (txt_lines2[i].length() != 0 || txt_lines2[i][0] == '>') {
				act_lines.push_back(txt_lines2[i]);
				//act_lines[index2] = txt_lines2[i].Replace('\t', ' ').Replace(" ", "");
			}
		}

		DataStructure *res = analyze_structure(act_lines, name, parent, path);
		return res;
	}

	DataStructure* analyze_structure(vector<string> text_lines, string name, DataStructure* parent, string path) {
		//For each command, do this:
		DataStructure* res = new DataStructure(name, parent, path);
		unsigned current_line = 0;
		bool still_running = true;
		unsigned counter = 0u;
		while (still_running) {
			int safty_counter = 0;

			if (current_line >= text_lines.size()) {
				return res;
			}

			//if (text_lines[current_line] == null) {
			//	return res;
			//}

			//For each item in the command, do this
			string line = text_lines[current_line];

			// Forward init
			string indicator_type;
			int count = 0;
			bool isarray = false;
			string written_count = "0";

			if (line[0] == '>') {
				// names are analyzed for special characters too

				string ch_name = std::to_string(counter++) + parse_string(line.substr(1));
				char recursion = 0x01;
				vector<string> _lines_ = vector<string>();
				for (current_line++; recursion > 0; current_line++) {
					// Check the size of the DataStructure
					if (current_line >= text_lines.size()) {
						cerr << "Not matching \">\" with \"<\" " << endl;
						//Throw("Not matching \">\" with \"<\" ", file_name, current_line + beginning_line);
					}
					else if (text_lines[current_line][0] == '>') {
						recursion++;
					}
					else if (text_lines[current_line][0] == '<') {
						recursion--;
					}
					_lines_.push_back(text_lines[current_line]);
				}
				analyze_structure(_lines_, ch_name, res, path);
				goto NOVALUE;
			}
			if (line[line.size() - 1] == '<') {
				still_running = false;
			}

			if (!still_running)
				goto NOVALUE;
			//Get the type:
			try {
				indicator_type = line.substr(0, 3);
			}
			catch (out_of_range) {
				indicator_type = "";
			}

			//Get the number, if array
			if (line.size() < 4)
				cerr << "index (3) out of range" << endl;
			if (line[3] == '*') {
				isarray = true;
				if (!contains(line, "-"))
					cerr << "must contain '-'" << endl;
				try {
					written_count = line.substr(4, line.find_first_of('-') - 4);
				}
				catch (out_of_range) {
					cerr << line + " is not valid" << endl;
				}
				count = parse_int32(written_count);
			}

			//If there is an array
			if (isarray) {
				string item_name = parse_string(line.substr(line.find_first_of('-') + 1, line.find_first_of('=') - line.find_first_of('-') - 1));
				vector<string> content = vector<string>(count);
				for (int j = 0; j < count; j++) {
					content[j] = text_lines[j + current_line + 1];
				}
				if (indicator_type == "int") {
					vector<int> temp_arr = vector<int>(count);
					for (int j = 0; j < count; j++) {
						temp_arr[j] = parse_int32(content[j]);
					}
					res->set_int_arr(item_name, temp_arr);
				}
				else if (indicator_type == "f64") {
					vector<double> temp_arr = vector<double>(count);
					for (int j = 0; j < count; j++) {
						temp_arr[j] = parse_float64(content[j]);
					}
					res->set_double_arr(item_name, temp_arr);
					break;
				}
				else if (indicator_type == "bit") {
					vector<bool> temp_arr = vector<bool>(count);
					for (int j = 0; j < count; j++) {
						temp_arr[j] = parse_bool(content[j]);
					}
					res->set_bool_arr(item_name, temp_arr);
					break;
				}
				else if (indicator_type == "chr") {
					vector<string> temp_arr = vector<string>(count);
					for (int j = 0; j < count; j++) {
						if (content[j][0] == '\\' && content[j][1] == '<') {
							temp_arr[j] = parse_file_path(content[j].substr(2), res->directory_path);
						} else {
							temp_arr[j] = parse_string(content[j]);
						}
					}
					res->set_string_arr(item_name, temp_arr);
				}
				else if (indicator_type == "vc3") {
					vector<LongVector> temp_arr = vector<LongVector>(count);
					for (int j = 0; j < count; j++) {
						temp_arr[j] = parse_vector(content[j]);
					}
					res->set_vector_arr(item_name, temp_arr);
					break;
				}
				else if (indicator_type == "dat") {
					vector<DataStructure*> temp_arr = vector<DataStructure*>(count);
					for (int j = 0; j < count; j++) {
						string f_path = parse_file_path(parse_string(content[j]), res->directory_path);
						temp_arr[j] = read_file(f_path, f_path, item_name, res, true);
					}
					res->set_child_arr(item_name, temp_arr);
				}
				else {
					cerr << "There is no prefix " << indicator_type << endl;
				}
				current_line += count + 1;
			}
			//If there is a single item
			else {
				string item_name;
				string content;
				if (line.find_first_of('=') == -1) {
					item_name = "";
					content = "";
				}
				else {
					item_name = parse_string(line.substr(3, line.find_first_of('=') - 3));
					content = line.substr(line.find_first_of('=') + 1);
				}
				if (indicator_type == "int") {
					res->set_int(item_name, parse_int32(content));
				}
				else if (indicator_type == "f64") {
					res->set_double(item_name, parse_float64(content));
				}
				else if (indicator_type == "bit") {
					res->set_bool(item_name, parse_bool(content));
				}
				else if (indicator_type == "chr") {
					if (content[0] == '\\' && content[1] == 'P') {
						res->set_string(item_name, parse_file_path(content.substr(2), res->directory_path));
					}
					else {
						res->set_string(item_name, parse_string(content));
					}
				}
				else if (indicator_type == "vc3") {
					res->set_vector(item_name, parse_vector(content));
				}
				else if (indicator_type == "dat") {
					string f_path = parse_file_path(parse_string(content), res->directory_path);
					res->set_child(item_name, read_file(f_path, f_path, item_name, res, true));
				}
				else {
					cerr << "There is no prefix: " << indicator_type << endl;
				}
				current_line++;
			}

		NOVALUE:
			// Safty stuff
			if (++safty_counter >= 100000) {
				cerr << "Infinite loop" << endl;
				return res;
			}
		}
		return res;
	}


	int parse_int32(std::string content) {
		int res = 0;
		bool negative = false;
		if (content.size() > 0 && content[0] == '-') {
			negative = true;
			content = content.substr(1);
		}
		if (content[0] == 'x') {
			res = 0;
			for (unsigned i = 1; i < content.size(); i++) {
				char digit = content[content.size() - i];
				int mant = 0;
				if (48 <= digit && digit <= 57) mant = digit - 48;
				else if (65 <= digit && digit <= 70) mant = digit - 55;
				else if (97 <= digit && digit <= 102) mant = digit - 87;
				for (unsigned j = 0; j < i - 1; j++)
					mant *= 16;
				res += mant;
			}
		}
		else {
			try {
				res = stoi(content);
			}
			catch (invalid_argument) {
				cerr << "\"" << content << "\" is not a valid integer" << endl;
				res = 0;
			}
		}
		return negative ? -res : res;
	}

	double parse_float64(std::string content) {
		double res = 0;
		if (content.find_first_of('e') == -1) {
			try {
				res = atof(content.c_str());
			}
			catch (invalid_argument) {
				cerr << "\"" << content << "\" is not a valid 64-bit float" << endl;
				res = 0;
			}
		}
		else {
			int place = content.find_first_of('e');
			try {
				res = atof(content.substr(0, place).c_str());
				int exponent = atoi(content.substr(place + 1).c_str());
				for (int i = 0; i < abs(exponent); i++) {
					res *= exponent > 0? 10 : 0.1;
				}
			}
			catch (invalid_argument) {
				cerr << "\"" << content << "\" is not a valid 64-bit float" << endl;
				res = 0;
			}
		}
		return res;
	}

	bool parse_bool(std::string content) {
		if (content == "1" || content == "true") {
			return true;
		}
		return false;
	}

	std::string parse_string(std::string content) {
		if (regex_match(content, regex("^\"{1,2}.*\"{1,2}$"))) {
			// Multi line staff
			content = regex_replace(content, regex("^\"{1,2}\\s*"), "");
			content = regex_replace(content, regex("\\s*\"{1,2}\\n?$"), "");
			content = regex_replace(content, regex("\\s*Ñ\\s*"), "\n");
			if (content.length() > 2) {
				content = content.substr(1, content.length() - 2);
			}
			else {
				content = content.substr(1);
			}
			return content;
		}
		for (unsigned i = 0; i < content.length(); i++) {
			if (content[i] == '_' && i > 0 && content[i - 1] != '\\') {
				content[i] = ' ';
			}
		}
		content = regex_replace(content, regex("\\\\n"), "\n");
		content = regex_replace(content, regex("\\\\_"), "_");
		content = regex_replace(content, regex("\\\\\\\\"), "\\");

		return content;
	}

	LongVector parse_vector(std::string content) {
		try {
			size_t semi1 = content.find_first_of(';');
			size_t semi2 = content.find_last_of(';');

			double x = parse_float64(content.substr(0, semi1)),
				   y = parse_float64(content.substr(semi1 + 1, semi2-semi1)),
				   z = parse_float64(content.substr(semi2 + 1));

			return LongVector(x, y, z);
		}
		catch (invalid_argument) {
			cerr << "\"" << content << "\" is not a valid integer" << endl;
			return LongVector(0.0, 0.0, 0.0);
		}
	}
}