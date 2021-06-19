#pragma once

#include "math_tools.h"
#include "graphic_manager.h"
#include "radio.h"
#include "scriptable_module.h"
#include "api_base.h"

#define AGENTFLAG_UPDATED 0x0001
#define AGENTFLAG_DRAWN 0x0002

const int MAX_SHAPES = 128;

typedef unsigned int agent_id;

class Blackboard;

enum class LifeCycle {
	Defined,
	Instantiated,
	UnInstantiated
};

enum class AgentType {
	NONE,
	CAMERA,
	ORBITER,
	CELESTIAL,
	RINGSYSTEM,
	DEFAULT_SCRIPTABLE,
};

enum class DrawMode {
	CloseUp,
	Medium,
	Far,
};

struct AgentWrenData {
	AgentWrenData(Agent*);

	uint32_t identifier;
	WrenHandle* object_handle = nullptr;  // For the NOT foreign class
	Agent* agent;

	bool alive = true;
};

#define BIND(X) bind_basic_variable(&X, #X);

class Agent abstract
{
public:

	Agent();
	~Agent();

	void instantiate();
	void uninstantiate();
	//void virtual draw(const Camera*);

	// Called
	void pre_step(double);
	void step(double);
	void post_step(double);

	// Invoked
	void virtual physics_step(double);
	void virtual logic_step(double);
	void virtual draw_step(double);

	// Called when not in hierarchy
	void virtual loose_step(double);

	// reorients camera pitch and roll
	// rotation only
	LongMatrix4x4 virtual get_focus_transformation();
	LongMatrix4x4 fetch_focus_transformation(bool);

	int add_shape(Shape*);
	void delete_shape(int, bool);
	Shape* get_shape(int);
	AgentType const virtual get_type();
	bool const virtual is_instance_of(AgentType);

	agent_id id;
	uint16_t flags;
	std::string name = "";
	LongVector position = LongVector(0.0, 0.0, 0.0);
	glm::mat4 transform;
	bool is_camera_focus = false;

	void virtual interprete_signal(RadioSignal);
	static void interprete_signal_callback(void*, RadioSignal);

	Agent* parent = nullptr;
	std::vector<Agent*> children = std::vector<Agent*>();
	bool is_ancestor(Agent*);
	void add_child(Agent*);
	void loosen();
	void kill();
	Agent* find_child(std::string);

	DecoratedRadio* radio;
	ScriptableModule* scripting_module;
	api::Variable get_variable(std::string);

	AgentWrenData* wren_data;

protected:
	template <typename T>
	void bind_basic_variable(T* ptx, const char* name) {
		api::Type type = api::get_type(typeid(T));
		api::Variable v = api::Variable((void*) ptx, type, name);
		variable_map.insert({ name, v });
	};

	void bind_advanced_variable(void* ptx, const char* name, const char* p_module, const char* p_class, size_t p_memory_size);

private:
	LifeCycle cycle;

	Shape** shapes;
	int shape_number = 0;
	int reference_count = 0;  // not used yet, but I think it gets important

	std::map<std::string, api::Variable> variable_map = std::map<std::string, api::Variable>();
};

class BlankAgent :
	public Agent {
public:
	BlankAgent() : Agent() {};
	~BlankAgent() { Agent::~Agent(); };
};
