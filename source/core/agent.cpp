#include "agent.h"

agent_id agent_id_counter = 0;

Agent::Agent() {
	id = ++agent_id_counter;
	flags = 0;
	cycle = LifeCycle::Instantiated;
	shapes = new Shape* [MAX_SHAPES];
	transform = glm::mat4(1);
	for (int i = 0; i < MAX_SHAPES; i++) {
		shapes[i] = nullptr;
	}
}

Agent::~Agent() {
	for (int i = 0; i < MAX_SHAPES; i++) {
		if (shapes[i] != nullptr) {
			delete shapes[i];
		}
	}
	delete[] shapes;
}

void Agent::instantiate() {
	cycle = LifeCycle::Instantiated;
	for (int i = 0; i < MAX_SHAPES; i++) {
		if (shapes[i] != nullptr) {
			graphics_server->shape_vector_array[shapes[i]->draw_order()].push_back(shapes[i]);
		}
	}
}

void Agent::destroy() {
	cycle = LifeCycle::Dead;
	for (int id = 0; id < MAX_SHAPES; id++) {
		if (shapes[id] != 0) {
			auto vector = graphics_server->shape_vector_array[shapes[id]->draw_order()];
			for (auto i = vector.cbegin(); i != vector.cend(); ++i) {
				if (*i == shapes[id])
					vector.erase(i);
			}
			delete shapes[id];
		}
	}
}

void Agent::pre_step(double dt) {
	physics_step(dt);

	flags = flags & !AGENTFLAG_UPDATED;

	// Recursive behaviour
	for (auto it = children.begin(); it != children.end(); ++it) {
		it->get()->pre_step(dt);
	}
}

void Agent::step(double dt) {
	if (AGENTFLAG_UPDATED & !flags) {
		logic_step(dt);
	}
	flags |= AGENTFLAG_UPDATED;

	// Recursive behaviour
	for (auto it = children.begin(); it != children.end(); ++it) {
		it->get()->step(dt);
	}
}

void Agent::post_step(double dt) {
	draw_step(dt);

	// Recursive behaviour
	for (auto it = children.begin(); it != children.end(); ++it) {
		it->get()->post_step(dt);
	}
}

void Agent::physics_step(double dt){ }
void Agent::logic_step(double dt) { }
void Agent::draw_step(double dt) { }

glm::mat4 Agent::get_focus_transformation() {
	return glm::mat4(1); // identity
}

int Agent::add_shape(Shape* shape) {
	shape->parent = this;

	int id = 0;
	while (shapes[id] != nullptr) id++;
	if (cycle == LifeCycle::Instantiated) {
		graphics_server->shape_vector_array[shape->draw_order()].push_back(shape);
	}
	
	shapes[id] = shape;
	shape_number++;
	return id;
}

void Agent::delete_shape(int id, bool disconnect_only=false) {
	if (!disconnect_only) {
		auto vector = graphics_server->shape_vector_array[shapes[id]->draw_order()];
		for (auto i = vector.cbegin(); i != vector.cend(); ++i) {
			if (*i == shapes[id + 128])
				vector.erase(i);
		}
		delete shapes[id];
	}
	shapes[id] = nullptr;
	shape_number--;
}

Shape* Agent::get_shape(int id) {
	return shapes[id];
}

Type Agent::get_type() {
	return Type::NONE;
}

glm::mat4 get_shape_transform(Shape* s) {
	if (s->parent == nullptr || s->independent) {
		return s->shape_transform;
	}
	return s->parent->transform * s->shape_transform;
}
