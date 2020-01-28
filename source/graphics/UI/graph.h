#pragma once
#include "ui_element.h"

class Graph:
	public UIElement {
public:
	Graph();
	~Graph();

	void set_data(std::vector<float>);
	void set_data(std::vector<float>, float, float);

protected:
	std::vector<float> get_lines() override;

private:
	std::vector<float> values;
};