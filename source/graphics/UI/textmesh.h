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
	void set_transform(int, int, int, int) override;
	void draw(Camera*) override;

	bool vector = true;
	Font font;
	struct Textpos { unsigned x, y; };
	Textpos get_text_pos(int index);

protected:
	const float spacing_x = .4f, spacing_y = 0.7f;
	const float ratio = 1.2f;

	std::vector<float> virtual get_lines();
	void setup_lines(std::vector<float>);
	void recalculate() override;

private:
	void increment_tp(Textpos*, char);
	std::string content;

	GLuint VertexBufferID;
	GLuint UVBufferID_MatrixID;
	GLuint SamplerUniformID;
	GLuint ScreenSizeID;
	GLuint ColorID;

	unsigned vertex_buffer_size;
	std::vector<GLuint> vertexbuffers;
};

#endif // !TEXTMESH_H