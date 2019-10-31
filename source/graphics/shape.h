#ifndef SHAPE_H
#define SHAPE_H
#include "Camera.h"

#define DEBUG

class Shape abstract
{
public:
	Shape();
	~Shape();

	/// <summary> ID of the shader, the shape uses </summary>
	GLuint shader;
	/// <summary> ID of the vertex buffer </summary>
	GLuint vertex_buffer;

	/// <summary> method needed to draw anything on screen </summary>
	void virtual draw(Camera* camera);

protected:
	GLuint matrix_ID;
	GLuint color_ID;
};

#endif // !SHAPE_H