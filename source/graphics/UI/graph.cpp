#include "graph.h"

Graph::Graph() {

}

Graph::~Graph() {

}

void Graph::set_data(std::vector<float> data) {
	float max_value = -INFINITY, min_value = INFINITY;
	for (float v : data) {
		if (v > max_value) max_value = v;
		if (v < min_value) min_value = v;
	}
	set_data(data, min_value, max_value);
}

void Graph::set_data(std::vector<float> data, float min_y, float max_y) {
	for (float v : data) {
		values.push_back((v - min_y) / (max_y - min_y));
	}
}

std::vector<float> Graph::get_lines() {
	std::vector<float> res = UIElement::get_lines();
	size_t values_size = values.size();
	res.push_back(0);
	res.push_back(values[0] * height);
	for (size_t i = 1; i < values_size - 1; i++) {
		res.push_back((float) i / (values_size - 1) * width);
		res.push_back(values[i] * height);
		res.push_back((float)i / (values_size - 1) * width);
		res.push_back(values[i] * height);
	}
	res.push_back(width);
	res.push_back(values[values_size - 1] * height);
	return res;
}