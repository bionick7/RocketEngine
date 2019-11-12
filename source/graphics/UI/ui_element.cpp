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

void UIElement::set_transform(float _x, float _y, float _width, float _height) {
	x = _x;
	y = _y;
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