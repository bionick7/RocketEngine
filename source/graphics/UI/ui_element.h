#include "../Camera.h"
#ifndef UIELEMENT_H
#define UIELEMENT_H

GLPos pixel2glpos(Screenpos);
Screenpos glpos2pixel(GLPos);

class UIElement abstract 
{
public:
	UIElement();
	void virtual set_transform(int, int, int, int);

	void virtual update(double, GLFWwindow*);
	void virtual draw(Camera*);

protected:
	pixel x, y, width, height;

	void virtual recalculate();
};

#endif // !UIELEMENT_H