#ifndef TEXTMESH_H
#define TEXTMESH_H

#include "shape.h"

#define DEBUG

class Textmesh:
	public Shape
{
public:
	Textmesh(std::wstring, Font);
	~Textmesh();

	std::wstring get_content();
	void set_content(std::wstring);
	void set_position(float, float);

	void draw(Camera*) override;

	Font font;
	int size;

private:
	float x, y;

	std::wstring content;

	GLuint Text2DVertexBufferID;
	GLuint Text2DUVBufferID;
	GLuint Text2DUniformID;
};

#endif // !TEXTMESH_H