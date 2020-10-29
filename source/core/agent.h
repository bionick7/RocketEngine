#pragma once

#include "math_tools.h"
#include "shape.h"
#include "Blackboard.h"

#define AGENTFLAG_UPDATED 0x0001
#define AGENTFLAG_DRAWN 0x0002

const int MAX_SHAPES = 128;

typedef unsigned int agent_id;

class Blackboard;

enum class LifeCycle {
	Defined,
	Instantiated,
	Dead
};

enum class Type {
	NONE,
	CAMERA,
	CELESTIAL,
	RINGSYSTEM,
};

enum class DrawMode {
	CloseUp,
	Medium,
	Far,
};

#pragma once
class Agent abstract
{
public:
	agent_id id;

	uint16_t flags;
	std::string name = "";

	std::vector<std::unique_ptr<Agent>> children = std::vector<std::unique_ptr<Agent>>();

	Agent();
	~Agent();

	void instantiate();
	void destroy();
	//void virtual draw(const Camera*);

	// Called
	void pre_step(double);
	void step(double);
	void post_step(double);

	// Invoked
	void virtual physics_step(double);
	void virtual logic_step(double);
	void virtual draw_step(double);

	glm::mat4 virtual get_focus_transformation();

	int add_shape(Shape*);
	void delete_shape(int, bool);
	Shape* get_shape(int);
	Type virtual get_type();

	LongVector position = LongVector(0.0, 0.0, 0.0);
	glm::mat4 transform;
	bool is_camera_focus = false;

private:
	LifeCycle cycle;

	Shape** shapes;
	int shape_number = 0;
	int reference_count = 0; // not used yet, but I think it gets important
};
