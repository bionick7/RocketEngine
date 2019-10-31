#include <iostream>

#include "debug.h"

using namespace std;

void print_matrix(glm::mat4x4 matrix) {
	for (int i = 0; i < 4; i++) {
		if (i == 0) cout << "/ ";
		else if (i == 3) cout << "\\ ";
		else cout << "| ";
		for (int j = 0; j < 4; j++){
			cout << to_string(matrix[j][i]) << ", ";
		}
		if (i == 0) cout << " \\";
		else if (i == 3) cout << " /";
		else cout << " |";
		cout << endl;
	}
}

void print_vector(glm::vec3 vec) {
	cout << "( " << to_string(vec.x) << ", " << to_string(vec.y) << ", " << to_string(vec.z) << " )" << endl;
}

void print_text(string txt) {
	cout << txt << endl;
}

void print_number(double inp) {
	cout << to_string(inp) << endl;
}