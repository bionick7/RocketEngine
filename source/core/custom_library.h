#pragma once
#include "core.h"

void replace_character(std::string* reference_string, char char_1, char char_2);
void purge_char(std::string* reference_string, char char_1);
bool contains(std::string inp, std::string match);
std::vector<std::string> split_string_to_lines(std::string s);  // Keeps \n at the end
std::string capitalize(std::string s);

template<typename T>
class ConstantIndexList
{
public:
	ConstantIndexList() { }
	~ConstantIndexList() { }

	size_t insert(T x) {
		if (free.empty()) {
			data.push_back(x);
			return data.size() - 1;
		}
		else {
			size_t index = *(free.begin());
			free.erase(index);
			data[index] = x;
			return index;
		}
	}

	void overwrite(T x, size_t index, T filler) {
		if (index >= data.size()) {
			for (int i = data.size(); i < index; i++) {
				data.push_back(filler);
				free.insert(i);
			}
			data.push_back(x);
		}
		else {
			if (!is_value_at(index)) {
				free.erase(index);
			}
			data[index] = x;
		}
	}

	void erase(size_t index) {
		free.insert(index);
	}

	bool is_value_at(size_t index) {
		return free.find(index) == free.end();
	}

	size_t allocated_size() {
		return data.size();
	}

	T operator[](size_t index) {
		if (is_value_at(index)) {
			return data[index];
		}
		else {
			throw std::out_of_range("index out of range");
		}
	}

private:
	std::vector<T> data = std::vector<T>();
	std::set<size_t> free = std::set<size_t>();
};
