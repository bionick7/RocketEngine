#include "custom_library.h"

void replace_character(std::string* reference_string, char char_1, char char_2) {
	std::string temp_str = *reference_string;
	for (unsigned i = 0; i < reference_string->size(); i++) {
		if (temp_str[i] == char_1) {
			temp_str[i] = char_2;
		}
	}
	*reference_string = temp_str;
}

void purge_char(std::string* reference_string, char char_1) {
	unsigned size = reference_string->size();
	for (unsigned i = 0; i < size; i++) {
		if ((*reference_string)[i] == char_1) {
			reference_string->erase(reference_string->begin() + i, reference_string->begin() + i + 1);
			size--;
			i--;
		}
	}
}

bool contains(std::string inp, std::string match) {
	return inp.find(match) != -1;
}

std::vector<std::string> split_string_to_lines(std::string s) {  // Omits \n at the end
	std::vector<std::string> res = std::vector<std::string>();
	std::string line = "";
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == '\n') {
			res.push_back(line);
			line = "";
		}
		else {
			line += s[i];
		}
	}
	if (line != "") {
		res.push_back(line);
	}
	return res;
}

std::string capitalize(std::string s) {
	for (int i = 0; i < s.size(); i++) {
		s[i] = toupper(s[i]);
	}
	return s;
}
