#include "ui_element.h"

UIElement::UIElement() {
	x = 0;
	y = 0;
	widt = 0;
	height = 0;
}

void UIElement::set_transform(float _x, float _y, float _width, float _height) {
	x = _x;
	y = _y;
	widt = _width;
	height = _height;
	recalculate();
}

void UIElement::update(double, GLFWwindow*) {

}

void UIElement::draw(Camera*) {

}

void UIElement::recalculate() {

}