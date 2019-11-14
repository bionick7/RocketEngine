#include "ui_element.h"

GLPos pixel2glpos(Screenpos pixel) {
	return GLPos(
		(float) pixel.x / settings->width * 2.0f - 1.0f,
		(float) pixel.y / settings->height * 2.0f - 1.0f
	);
}

Screenpos glpos2pixel(GLPos pos) {
	return Screenpos(
		(int) ((pos.x + 1.0f) * 0.5f * settings->width),
		(int) ((pos.y + 1.0f) * 0.5f * settings->height)
	);
}

UIElement::UIElement() {
	x = 0;
	y = 0;
	width = 0;
	height = 0;
}

void UIElement::set_transform(int _x, int _y, int _width, int _height) {
	if (_x >= 0) x = _x;
	else x = settings->width + _x;
	if (_y >= 0) y = _y;
	else y = settings->height + _y;
	width = _width;
	height = _height;
	recalculate();
}

void UIElement::update(double, GLFWwindow*) {

}

void UIElement::draw(Camera*) {

}

void UIElement::recalculate() {

}