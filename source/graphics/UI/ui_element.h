#include "../Camera.h"
#ifndef UIELEMENT_H
#define UIELEMENT_H

class UIElement abstract 
{
public:
	UIElement();
	void virtual set_transform(float, float, float, float);

	void virtual update(double, GLFWwindow*);
	void virtual draw(Camera*);

protected:
	float x, y, widt, height;

	void virtual recalculate();
};

#endif // !UIELEMENT_H