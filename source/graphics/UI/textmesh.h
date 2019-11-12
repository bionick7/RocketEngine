#ifndef TEXTMESH_H
#define TEXTMESH_H

#include "vectorgraphic.h"

class Textmesh:
	public UIElement
{
public:
	Textmesh(std::string, Font);
	~Textmesh();

	std::string get_content();
	void set_content(std::string);
	void set_transform(float, float, float, float) override;

	void draw(Camera*) override;

	bool vector = true;
	Font font;

private:
	std::string content;

	GLuint VertexBufferID;
	GLuint UVBufferID_MatrixID;
	GLuint SamplerUniformID;
	GLuint ScreenSizeID;
	GLuint ColorID;

	unsigned vertex_buffer_size;
	std::vector<GLuint> vertexbuffers;

protected:
	void recalculate() override;
};

#endif // !TEXTMESH_H