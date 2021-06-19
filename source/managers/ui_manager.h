#pragma once
#include "ui_canvas.h"
#include "manager.h"

#define MAX_CANVAS_COUNT 128

class UIManager:
	public Manager
{
public:
	UIManager();
	~UIManager();

	void setup(GLFWwindow*);
	void step(double) override;

	UICanvas* get_canvas();
	GLFWwindow* get_window_ptx();
	double* get_scroll_ptx();

	void interprete_signal(RadioSignal) override;

private:
	UICanvas* canvas;

	GLFWwindow* window_ptx;
	double* scroll_value_ptx;

};

extern UIManager* ui_manager;