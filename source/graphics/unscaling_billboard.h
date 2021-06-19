#pragma once
#include "graphic_manager.h"

class Billboard:
	public Shape
{
public:
	Billboard(float);
	~Billboard();

	void draw(const Camera*, glm::mat4 base) override;
	signed char draw_order() override;

private:
	GLuint active_ID;
};

