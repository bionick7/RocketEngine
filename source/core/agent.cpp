#include "agent.h"
#include "scene_manager.h"

agent_id agent_id_counter = 0;
bool Agent::is_ancestor(Agent* x) {
	if (parent == x) {
		return true;
	}
	if (parent == nullptr) {
		return false;
	}
	return parent->is_ancestor(x);
}

void Agent::add_child(Agent* child) {
	if (child == this || is_ancestor(child)) {
		return;
	}

	for (int i = 0; i < scene_manager->loose_agents.size(); i++) {
		if (scene_manager->loose_agents[i] == this) {
			scene_manager->loose_agents.erase(scene_manager->loose_agents.begin() + i);
		}
	}
	children.push_back(child);
	child->parent = this;
}

AgentWrenData::AgentWrenData(Agent* p_agent) {
	identifier = 0;
	object_handle = nullptr;
	agent = p_agent;
}

void Agent::loosen() {
	for (int i = 0; i < parent->children.size(); i++) {
		if (parent->children[i] == this) {
			parent->children.erase(parent->children.begin() + i);
		}
	}
	scene_manager->loose_agents.push_back(this);
	parent = nullptr;

	uninstantiate();
}

void Agent::kill() {
	for (int i = 0; i < parent->children.size(); i++) {
		if (parent->children[i] == this) {
			parent->children.erase(parent->children.begin() + i);
		}
	}

	uninstantiate();
	delete this;  // ?
}

Agent* Agent::find_child(std::string find_name) {
	if (capitalize(name) == capitalize(find_name)) {
		return this;
	}
	for (Agent* child : children) {
		Agent* res = child->find_child(find_name);
		if (res != nullptr) {
			return res;
		}
	}
	return nullptr;
}

api::Variable Agent::get_variable(std::string var_name) {
	auto var_res = variable_map.find(var_name);
	if (var_res == variable_map.end()) {
		return api::Variable();
	}
	return var_res->second;
}

Agent::Agent() {
	id = ++agent_id_counter;
	flags = 0;
	cycle = LifeCycle::Instantiated;
	shapes = new Shape* [MAX_SHAPES];
	transform = glm::mat4(1);
	for (int i = 0; i < MAX_SHAPES; i++) {
		shapes[i] = nullptr;
	}

	// Radio setup
	radio = new DecoratedRadio(this, &Agent::interprete_signal_callback);
	radio->add_signal_at("death", 0);
	radio->add_signal_at("at_step", 1);
	radio->add_signal_at("end_step", 2);

	scene_manager->loose_agents.push_back(this);

	wren_data = new AgentWrenData(this);

	BIND(position)
	BIND(name)
}

Agent::~Agent() {
	radio->emit_signal("death");
	for (int i = 0; i < MAX_SHAPES; i++) {
		if (shapes[i] != nullptr) {
			delete shapes[i];
		}
	}
	for (Agent* a : children) {
		delete a;
	}
	delete[] shapes;
}

void Agent::instantiate() {
	cycle = LifeCycle::Instantiated;
	for (int i = 0; i < MAX_SHAPES; i++) {
		if (shapes[i] != nullptr) {
			graphics_manager->shape_vector_array[shapes[i]->draw_order() + 127].push_back(shapes[i]);
		}
	}
}

void Agent::uninstantiate() {
	cycle = LifeCycle::UnInstantiated;
	for (int id = 0; id < MAX_SHAPES; id++) {
		if (shapes[id] != 0) {
			auto vector = graphics_manager->shape_vector_array[shapes[id]->draw_order() + 127];
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
	for (Agent* it : children) {
		it->pre_step(dt);
	}
}

void Agent::step(double dt) {
	if (AGENTFLAG_UPDATED & !flags) {
		radio->emit_signal("at_step");
		logic_step(dt);

		if (scripting_module != nullptr) {
			scripting_module->step(dt);
		}

		radio->emit_signal("end_step");
	}
	flags |= AGENTFLAG_UPDATED;

	// Recursive behaviour
	for (Agent* it : children) {
		it->step(dt);
	}
}

void Agent::post_step(double dt) {
	draw_step(dt);

	// Recursive behaviour
	for (Agent* it : children) {
		it->post_step(dt);
	}
}

void Agent::physics_step(double dt) { }
void Agent::logic_step(double dt) { }
void Agent::draw_step(double dt) { }

void Agent::loose_step(double dt) { }

LongMatrix4x4 Agent::get_focus_transformation() {
	return LongMatrix4x4();  // Identity
}

LongMatrix4x4 Agent::fetch_focus_transformation(bool direct) {
	if (direct || scripting_module == nullptr) {
		return get_focus_transformation();
	}
	return scripting_module->fetch_focus_transform();
}

int Agent::add_shape(Shape* shape) {
	shape->parent = this;

	int id = 0;
	while (shapes[id] != nullptr) id++;
	if (cycle == LifeCycle::Instantiated) {
		graphics_manager->shape_vector_array[shape->draw_order() + 127].push_back(shape);
	}
	
	shapes[id] = shape;
	shape_number++;
	return id;
}

void Agent::delete_shape(int id, bool disconnect_only=false) {
	if (!disconnect_only) {
		auto vector = graphics_manager->shape_vector_array[shapes[id]->draw_order() + 127];
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

AgentType const Agent::get_type() {
	return AgentType::NONE;
}

bool const Agent::is_instance_of(AgentType other) {
	return false;
}

void Agent::interprete_signal(RadioSignal signal) {
	if (scripting_module != nullptr) {
		return scripting_module->interprete_signal(signal);
	}
}

void Agent::interprete_signal_callback(void* obj, RadioSignal signal) {
	((Agent*)obj)->interprete_signal(signal);
}

void Agent::bind_advanced_variable(void* ptx, const char* name, const char* p_module, const char* p_class, size_t p_memory_size) {
	api::Variable v = api::Variable((void*)ptx, api::Type::FOREIGN, name);
	v.parent_module = p_module;
	v.parent_class = p_class;
	v.alloc_size = p_memory_size;
	variable_map.insert({ name, v });
};

glm::mat4 get_shape_transform(Shape* s) {
	if (s->parent == nullptr || s->independent) {
		return s->shape_transform;
	}
	return s->parent->transform * s->shape_transform;
}
