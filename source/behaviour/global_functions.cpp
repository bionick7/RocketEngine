#include "global_functions.h"
#include "scriptable_module.h"
#include "audio_manager.h"

#define ASSERT_VALIDITY(OBJECT, FUNC_NAME, ARG_INDEX) \
if (OBJECT == nullptr) { \
	debug.out << "Error occured at function: " << FUNC_NAME << " argument " << ARG_INDEX << " invalid."; \
	debug.submit_to(DebugOutput::WREN_ERROR); \
	wrenSetSlotNull(vm, 0); \
	return; \
} \

#define ASSERT_VALIDITY_CUSTUM(OBJECT, FUNC_NAME, ARG_INDEX, CONDITION) \
if (!(CONDITION)) { \
	debug.out << "Error occured at function: " << FUNC_NAME << " argument " << ARG_INDEX << " invalid."; \
	debug.submit_to(DebugOutput::WREN_ERROR); \
	wrenSetSlotNull(vm, 0); \
	return; \
} \

// ======================================
//			GENERAL FUNCTIONS
// ======================================

#pragma region general

void get_var(WrenVM* vm, api::Variable var) {
	switch (var.type)
	{
	case api::Type::INT32: {
		int r = *((int*)var.ref);
		wrenSetSlotDouble(vm, 0, (double)r);
		break;
	}
	case api::Type::FLOAT64: {
		double r = *((double*)var.ref);
		wrenSetSlotDouble(vm, 0, r);
		break;
	}
	case api::Type::INT1: {
		bool r = *((bool*)var.ref);
		wrenSetSlotBool(vm, 0, r);
		break;
	}
	case api::Type::STRING: {
		std::string r = *((std::string*)var.ref);
		wrenSetSlotString(vm, 0, r.c_str());
		break;
	}
	case api::Type::VEC3: {
		wrenEnsureSlots(vm, 2);
		wrenGetVariable(vm, "linear_algebra", "Vec3", 1);
		LongVector* r = (LongVector*)wrenSetSlotNewForeign(vm, 0, 1, sizeof(LongVector));
		r->x = ((LongVector*)var.ref)->x;  // Deep Copy
		r->y = ((LongVector*)var.ref)->y;
		r->z = ((LongVector*)var.ref)->z;
		break;
	}
	case api::Type::FOREIGN: {  // Very unsafe
		wrenEnsureSlots(vm, 2);
		wrenGetVariable(vm, var.parent_module, var.parent_class, 1);
		void* r = wrenSetSlotNewForeign(vm, 0, 1, var.alloc_size);
		LongMatrix4x4 lm = *((LongMatrix4x4*)var.ref);
		memcpy(r, var.ref, var.alloc_size);
		break;
	}
	case api::Type::NONE: 
	case api::Type::ERR:
	default:
		wrenSetSlotNull(vm, 0);
		break;
	}
}

void set_var(WrenVM* vm, api::Variable var) {
	switch (var.type)
	{
	case api::Type::INT32: {
		int r = (int)wrenGetSlotDouble(vm, 2);
		*((int*)var.ref) = r;
		break;
	}
	case api::Type::FLOAT64: {
		double r = wrenGetSlotDouble(vm, 2);
		*((double*)var.ref) = r;
		break;
	}
	case api::Type::INT1: {
		bool r = wrenGetSlotBool(vm, 2);
		*((bool*)var.ref) = r;
		break;
	}
	case api::Type::STRING: {
		const char* r = wrenGetSlotString(vm, 2);
		*((std::string*)var.ref) = std::string(r);
		break;
	}
	case api::Type::VEC3: {
		LongVector* r = (LongVector*)wrenGetSlotForeign(vm, 2);
		((LongVector*)var.ref)->x = r->x;  // Deep Copy
		((LongVector*)var.ref)->y = r->y;
		((LongVector*)var.ref)->z = r->z;
		break;
	}
	case api::Type::FOREIGN: {  // Very unsafe
		void* ptx = wrenGetSlotForeign(vm, 2);
		_memccpy(var.ref, ptx, 0, var.alloc_size);
		break;
	}
	case api::Type::NONE:
	case api::Type::ERR:
	default: 
		var.ref = nullptr;
		break;
	}
}

void get_global_var(WrenVM* vm) {
	std::string var_name = std::string(wrenGetSlotString(vm, 1));
	api::Variable var = scripting_manager->get_var(var_name);
	get_var(vm, var);
}

void set_global_var(WrenVM* vm) {
	std::string var_name = std::string(wrenGetSlotString(vm, 1));
	api::Variable var = scripting_manager->get_var(var_name);
	set_var(vm, var);
}

#pragma endregion

// ======================================
//			GLOBAL FUNCTIONS
// ======================================

#pragma region global
void clear_screen() {
	scripting_manager->console_flags |= CONSOLE_FLAG_CLEAR;
}

void focus_on(WrenVM* vm) {
	WrenType t = wrenGetSlotType(vm, 1);
	if (t != WrenType::WREN_TYPE_FOREIGN) return;
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 1);
	ASSERT_VALIDITY(ag, "focus_on(_)", 1);
	Agent* orbiter = ag->agent;
	ASSERT_VALIDITY(orbiter, "focus_on(_)", 1);
	
	bool direct = wrenGetSlotBool(vm, 2);


	if (direct) {
		scene_manager->main_camera->focus(orbiter);
	}
	else {
		scene_manager->main_camera->interpolate_focus(orbiter);
	}
}

std::string test(std::string in) {
	return std::string("C++ code") + in;
}

void play_sound(std::string in) {
	auto resource = assets->get<AudioResource>(io::ResourceType::AUDIO, in);
	audio_manager->get_free_player()->play_static(*resource);
}

void reload(int id) {
	return; // Not now

	switch (id)
	{
	case 0: {
		io::DataStructurePtr settings_ds = assets->root_ds->get_child("settings");
		settings->reload(settings_ds);}
		  break;
	case 1: {
		io::DataStructurePtr assets_ds = assets->root_ds->get_child("assets");
		assets->reload(assets_ds); }
		  break;
	case 2: {
		io::DataStructurePtr scripts_ds = assets->root_ds->get_child("include");
		scripting_manager->load_modules(scripts_ds); }
		  break;
	default:
		break;
	}
}

void get_root(WrenVM* vm) {
	wrenEnsureSlots(vm, 2);
	wrenGetVariable(vm, "data_structure", "DataStructure", 1);
	io::DataStructurePtr* ds_ptx = (io::DataStructurePtr*)wrenSetSlotNewForeign(vm, 0, 1, sizeof(io::DataStructurePtr));
	*ds_ptx = assets->root_ds;
}

void get_root_agent(WrenVM* vm) {
	wrenEnsureSlots(vm, 2);
	wrenGetVariable(vm, "agent_interface", "Agent", 1);
	AgentWrenData* ds_ptx = (AgentWrenData*)wrenSetSlotNewForeign(vm, 0, 1, sizeof(AgentWrenData));
	*ds_ptx = *(scene_manager->orbit_root->wren_data);
}

#pragma endregion

// ======================================
//			DATASTRUCTURE
// ======================================

#pragma region DataStructure

void ds_allocate(WrenVM* vm)
{
	const char* path = wrenGetSlotString(vm, 1);
	io::DataStructurePtr* ds_ptx = (io::DataStructurePtr*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(io::DataStructurePtr));
	*ds_ptx = io::get_ds_from_path(path, io::get_root_ds());
}

void ds_finalize(void* data)
{
	io::DataStructurePtr* ds_ptx = (io::DataStructurePtr*) data;
	*ds_ptx = nullptr;
}

void ds_get_name(WrenVM* vm)
{
	io::DataStructurePtr* ds_ptx = (io::DataStructurePtr*)wrenGetSlotForeign(vm, 0);

	if (*ds_ptx == nullptr)
	{
		wrenSetSlotString(vm, 0, "NULL");
		return;
	}

	std::string name = (*ds_ptx)->get_name();
	wrenSetSlotString(vm, 0, name.c_str());
}

void ds_get_from_path(WrenVM* vm)
{
	wrenEnsureSlots(vm, 3);
	wrenGetVariable(vm, "data_structure", "DataStructure", 2);
	io::DataStructurePtr* entry = (io::DataStructurePtr*)wrenGetSlotForeign(vm, 0);
	const char* path = wrenGetSlotString(vm, 1);
	io::DataStructurePtr* exit = (io::DataStructurePtr*)wrenSetSlotNewForeign(vm, 0, 2, sizeof(io::DataStructurePtr));
	*exit = io::get_ds_from_path(std::string(path), *entry);
}

template <typename T>
void fill_wren_list(WrenVM* vm, std::vector<T> v) {
	wrenEnsureSlots(vm, 5 + v.size());
	wrenSetSlotNewList(vm, 0);
	for (int i = 0; i < v.size(); i++) {
		wrenSetSlotDouble(vm, i + 5, v[i]);
		wrenInsertInList(vm, 0, i, i + 5);
	}
}

void ds_get_helper(WrenVM* vm, std::function<void(io::DataStructurePtr, std::string, bool)> callback) {
	io::DataStructurePtr ds = *((io::DataStructurePtr*)wrenGetSlotForeign(vm, 0));

	if (ds == nullptr)
	{
		wrenSetSlotString(vm, 0, "NULL");
		return;
	}

	std::string value_name = wrenGetSlotString(vm, 1);
	bool quiet = wrenGetSlotBool(vm, 3);

	callback(ds, value_name, quiet);
}

void ds_set_helper(WrenVM* vm, std::function<void(io::DataStructurePtr, std::string, bool)> callback) {
	io::DataStructurePtr ds = *((io::DataStructurePtr*)wrenGetSlotForeign(vm, 0));

	if (ds == nullptr)
	{
		wrenSetSlotString(vm, 0, "NULL");
		wrenAbortFiber(vm, 0);
		return;
	}

	std::string value_name = wrenGetSlotString(vm, 1);
	bool quiet = wrenGetSlotBool(vm, 3);

	callback(ds, value_name, quiet);
}

void ds_get_int(WrenVM* vm) {
	ds_get_helper(vm, [vm](io::DataStructurePtr ds, std::string value_name, bool quiet) {
		wrenSetSlotDouble(vm, 0, (double)ds->get_int(value_name, (int)wrenGetSlotDouble(vm, 2), quiet));
	});
}
void ds_get_double(WrenVM* vm) {
	ds_get_helper(vm, [vm](io::DataStructurePtr ds, std::string value_name, bool quiet) {
		wrenSetSlotDouble(vm, 0, ds->get_double(value_name, wrenGetSlotDouble(vm, 2), quiet));
	});
}
void ds_get_bool(WrenVM* vm) {
	ds_get_helper(vm, [vm](io::DataStructurePtr ds, std::string value_name, bool quiet) {
		wrenSetSlotBool(vm, 0, ds->get_bool(value_name, wrenGetSlotBool(vm, 2), quiet));
	});
}
void ds_get_string(WrenVM* vm) {
	ds_get_helper(vm, [vm](io::DataStructurePtr ds, std::string value_name, bool quiet) {
		std::string str_res = ds->get_string(value_name, wrenGetSlotString(vm, 2), quiet);
		wrenSetSlotString(vm, 0, str_res.c_str());
	});
}
void ds_get_vector(WrenVM* vm) {
	ds_get_helper(vm, [vm](io::DataStructurePtr ds, std::string value_name, bool quiet) {
		wrenEnsureSlots(vm, 8);
		LongVector vec_def = LongVector();	// ???
		LongVector vec_res = ds->get_vector(value_name, vec_def, quiet);
		wrenSetSlotDouble(vm, 5, vec_res.x);
		wrenSetSlotDouble(vm, 6, vec_res.y);
		wrenSetSlotDouble(vm, 7, vec_res.z);
		wrenSetSlotNewList(vm, 0);
		wrenInsertInList(vm, 0, 0, 5);
		wrenInsertInList(vm, 0, 1, 6);
		wrenInsertInList(vm, 0, 2, 7);
	});
}

void ds_set_int(WrenVM* vm) {
	ds_get_helper(vm, [vm](io::DataStructurePtr ds, std::string value_name, bool quiet) {
		ds->set_int(value_name, wrenGetSlotDouble(vm, 2));
	});
}
void ds_set_double(WrenVM* vm) {
	ds_get_helper(vm, [vm](io::DataStructurePtr ds, std::string value_name, bool quiet) {
		wrenSetSlotDouble(vm, 0, ds->get_double(value_name, wrenGetSlotDouble(vm, 2), quiet));
		});
}
void ds_set_bool(WrenVM* vm) {
	ds_get_helper(vm, [vm](io::DataStructurePtr ds, std::string value_name, bool quiet) {
		wrenSetSlotBool(vm, 0, ds->get_bool(value_name, wrenGetSlotBool(vm, 2), quiet));
		});
}
void ds_set_string(WrenVM* vm) {
	ds_get_helper(vm, [vm](io::DataStructurePtr ds, std::string value_name, bool quiet) {
		std::string str_res = ds->get_string(value_name, wrenGetSlotString(vm, 2), quiet);
		wrenSetSlotString(vm, 0, str_res.c_str());
		});
}
#pragma endregion

// ======================================
//			RADIO
// ======================================

#pragma region Radio

void radio_allocate(WrenVM* vm) {
	// Cannot allocate from wren
}

void radio_finalize(void* data) {
	Radio** ds_ptx = (Radio**) data;
	delete (*ds_ptx);
	//*ds_ptx = nullptr;
}

void radio_add_signal(WrenVM* vm) {
	DecoratedRadio** radio_ptx = (DecoratedRadio**)wrenGetSlotForeign(vm, 0);
	if (*radio_ptx == nullptr) return;

	std::string signal_name = std::string(wrenGetSlotString(vm, 1));

	(*radio_ptx)->add_signal(signal_name);
}

void radio_clear_signal(WrenVM* vm) {
	DecoratedRadio** radio_ptx = (DecoratedRadio**)wrenGetSlotForeign(vm, 0);
	if (*radio_ptx == nullptr) return;

	std::string signal_name = std::string(wrenGetSlotString(vm, 1));

	(*radio_ptx)->clear_signal(signal_name);
}

void radio_emit_signal(WrenVM* vm) {
	DecoratedRadio** radio_ptx = (DecoratedRadio**)wrenGetSlotForeign(vm, 0);
	if (*radio_ptx == nullptr) return;

	std::string signal_name = std::string(wrenGetSlotString(vm, 1));
	void* content = nullptr;  // Get extra through list
	
	(*radio_ptx)->emit_signal(signal_name, content);
}

void radio_connect(WrenVM* vm) {
	DecoratedRadio** radio_ptx = (DecoratedRadio**)wrenGetSlotForeign(vm, 0);
	if (*radio_ptx == nullptr) return;

	std::string signal_name = std::string(wrenGetSlotString(vm, 1));
	Radio** reciever_ptx = (Radio**)wrenGetSlotForeign(vm, 2);
	std::string callback_name = std::string(wrenGetSlotString(vm, 3));
	void* extra_content = nullptr;  // Get extra through list

	(*radio_ptx)->connect(signal_name, *reciever_ptx, RadioSignal(callback_name, extra_content));
}
#pragma endregion

// ======================================
//			AGENT
// ======================================

#pragma region Agent
void agent_allocate(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(AgentWrenData*));

	ag->alive = true;

	std::string type_name = std::string(wrenGetSlotString(vm, 1));
	ag->object_handle = wrenGetSlotHandle(vm, 2);

	// Create Module
	ScriptableModule* s_module = new ScriptableModule();

	// Create Object
	Agent* agent = spawn_custom_class(type_name);
	if (agent == nullptr) {
		errlog("Unable to spawn agent from \"" + type_name + "\"");
		wrenSetSlotNull(vm, 0);
		return;
	}

	ag->agent = agent;
	agent->scripting_module = s_module;
	agent->wren_data = ag;
	uint32_t scripting_module_id = scripting_manager->scriptable_modules.insert(s_module);
	s_module->setup(vm, agent);

	ag->identifier = scripting_module_id;
}

void agent_destruct(void* ptx) {
	//AgentWrenData* ag = (AgentWrenData*)ptx;
	//if (ag != nullptr && !ag->stable) {
	//	ag->agent->kill();
	//}
}

void agent_get_radio(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 0);
	ASSERT_VALIDITY(ag, "[Agent].get_radio()", "[INSTANCE]");
	Agent* agent = ag->agent;
	ASSERT_VALIDITY(agent, "[Agent].get_radio()", "[INSTANCE]");

	DecoratedRadio* result = agent->radio;

	wrenGetVariable(vm, "radio", "Radio", 1);
	DecoratedRadio** result_ptx = (DecoratedRadio**)wrenSetSlotNewForeign(vm, 0, 1, sizeof(DecoratedRadio*));
	*result_ptx = result;
}

void agent_get_parent(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 0);
	ASSERT_VALIDITY(ag, "[Agent].get_parent()", "[INSTANCE]");
	Agent* agent = ag->agent;
	ASSERT_VALIDITY(agent, "[Agent].get_parent()", "[INSTANCE]");

	Agent* result = agent->parent;

	if (result == nullptr) {
		wrenSetSlotNull(vm, 0);
		return;
	}

	wrenGetVariable(vm, "agent_interface", "Agent", 1);
	AgentWrenData* result_ptx = (AgentWrenData*)wrenSetSlotNewForeign(vm, 0, 1, sizeof(AgentWrenData));
	*result_ptx = *(result->wren_data);
}

void agent_kill(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 0);
	ASSERT_VALIDITY(ag, "[Agent].kill()", "[INSTANCE]");
	Agent* agent = ag->agent;
	ASSERT_VALIDITY(agent, "[Agent].kill()", "[INSTANCE]");

	agent->kill();
	ag->alive = false;
}

void agent_is_valid(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 0);
	if (ag == nullptr) {
		wrenSetSlotBool(vm, 0, false);
		return;
	}
	if (ag->agent == nullptr || !ag->alive) {
		wrenSetSlotBool(vm, 0, false);
		return;
	}
	wrenSetSlotBool(vm, 0, true);
}

void agent_get_children(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 0);
	ASSERT_VALIDITY(ag, "[Agent].get_children()", "[INSTANCE]");
	Agent* agent = ag->agent;
	ASSERT_VALIDITY(agent, "[Agent].get_children()", "[INSTANCE]");

	int child_count = agent->children.size();
	wrenEnsureSlots(vm, child_count + 2);
	wrenGetVariable(vm, "agent_interface", "Agent", child_count + 1);
	wrenSetSlotNewList(vm, 0);
	for (int i = 0; i < child_count; i++) {
		AgentWrenData* child_ptx = (AgentWrenData*)wrenSetSlotNewForeign(vm, 0, 1, sizeof(AgentWrenData));
		wrenInsertInList(vm, 0, i, i + 1);
		*child_ptx = *(agent->children[i]->wren_data);
	}
}

void agent_find_in_tree(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 0);
	ASSERT_VALIDITY(ag, "[Agent].find_in_tree()", "[INSTANCE]");
	Agent* agent = ag->agent;
	ASSERT_VALIDITY(agent, "[Agent].find_in_tree()", "[INSTANCE]");

	std::string name = std::string(wrenGetSlotString(vm, 1));

	Agent* result = agent->find_child(name);

	if (result != nullptr) {
		wrenGetVariable(vm, "agent_interface", "Agent", 2);
		AgentWrenData* result_ptx = (AgentWrenData*)wrenSetSlotNewForeign(vm, 0, 2, sizeof(AgentWrenData));
		*result_ptx = *(result->wren_data);
	} else {
		wrenSetSlotNull(vm, 0);
	}
}

void agent_spawn_on(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 0);
	ASSERT_VALIDITY(ag, "[Agent].spawn_on()", "[INSTANCE]");
	Agent* agent = ag->agent;
	ASSERT_VALIDITY(agent, "[Agent].spawn_on()", "[INSTANCE]");

	AgentWrenData* p_ag = (AgentWrenData*)wrenGetSlotForeign(vm, 1);
	ASSERT_VALIDITY(p_ag, "[Agent].spawn_on()", 1);
	Agent* parent = p_ag->agent;
	ASSERT_VALIDITY(parent, "[Agent].spawn_on()", 1);

	parent->add_child(agent);
	agent->instantiate();
}


void agent_get_type(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 0);
	ASSERT_VALIDITY(ag, "[Agent].spawn_on()", "[INSTANCE]");
	Agent* agent = ag->agent;
	ASSERT_VALIDITY(agent, "[Agent].spawn_on()", "[INSTANCE]");

	AgentType type = agent->get_type();
	const char* type_name;
	switch (type)
	{
	case AgentType::CAMERA:
		type_name = "Camera";
		break;
	case AgentType::ORBITER:
		type_name = "Orbiter";
		break;
	case AgentType::CELESTIAL:
		type_name = "Celestial";
		break;
	case AgentType::RINGSYSTEM:
		type_name = "Ringsystem";
		break;
	case AgentType::DEFAULT_SCRIPTABLE:
		type_name = "Generic";
		break;
	case AgentType::NONE:
	default:
		type_name = "Unknown";
		break;
	}
	wrenSetSlotString(vm, 0, type_name);

	agent->instantiate();
}

void agent_get_var(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 0);
	ASSERT_VALIDITY(ag, "[Agent].get_var()", "[INSTANCE]");
	Agent* agent = ag->agent;
	ASSERT_VALIDITY(agent, "[Agent].get_var()", "[INSTANCE]");

	std::string var_name = std::string(wrenGetSlotString(vm, 1));
	api::Variable var = agent->get_variable(var_name);
	get_var(vm, var);
}

void agent_set_var(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 0);
	ASSERT_VALIDITY(ag, "[Agent].set_var()", "[INSTANCE]");
	Agent* agent = ag->agent;
	ASSERT_VALIDITY(agent, "[Agent].set_var()", "[INSTANCE]");

	std::string var_name = std::string(wrenGetSlotString(vm, 1));
	api::Variable var = agent->get_variable(var_name);
	set_var(vm, var);
}

void agent_get_id(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*) wrenGetSlotForeign(vm, 0);
	ASSERT_VALIDITY(ag, "[Agent].get_id()", "[INSTANCE]");
	wrenSetSlotDouble(vm, 0, ag->identifier);
}

void agent_get_agency(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 0);
	ASSERT_VALIDITY(ag, "[Agent].get_agency()", "[INSTANCE]");
	wrenSetSlotHandle(vm, 0, ag->object_handle);
}

void agent_get_focus_transform(WrenVM* vm) {
	AgentWrenData* ag = (AgentWrenData*)wrenGetSlotForeign(vm, 0);
	ASSERT_VALIDITY(ag, "[Agent].get_agency()", "[INSTANCE]");

	wrenGetVariable(vm, "linear_algebra", "Transform", 0);
	LongMatrix4x4* res =  (LongMatrix4x4*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(LongMatrix4x4));
	*res = ag->agent->fetch_focus_transformation(true);
}
#pragma endregion

// ======================================
//			LINEAR ALGEBRA
// ======================================

#pragma region LinearAlgebra
void vector_allocate(WrenVM* vm) {
	LongVector* v = (LongVector*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(LongVector));
	v->x = wrenGetSlotDouble(vm, 1);
	v->y = wrenGetSlotDouble(vm, 2);
	v->z = wrenGetSlotDouble(vm, 3);
}

void vector_getter(WrenVM* vm) {
	LongVector* v = (LongVector*)wrenGetSlotForeign(vm, 0);
	double idx = wrenGetSlotDouble(vm, 1);
	if (std::abs(idx) < 0.5) {
		wrenSetSlotDouble(vm, 0, v->x);
	}
	else if (std::abs(idx - 1) < 0.5) {
		wrenSetSlotDouble(vm, 0, v->y);
	}
	else if (std::abs(idx - 2) < 0.5) {
		wrenSetSlotDouble(vm, 0, v->z);
	}
}

void vector_setter(WrenVM* vm) {
	LongVector* v = (LongVector*)wrenGetSlotForeign(vm, 0);
	double idx = wrenGetSlotDouble(vm, 1);
	double x = wrenGetSlotDouble(vm, 2);
	if (std::abs(idx) < 0.5) {
		v->x = x;
	}
	else if (std::abs(idx - 1) < 0.5) {
		v->y = x;
	}
	else if (std::abs(idx - 2) < 0.5) {
		v->z = x;
	}
}

void transform_allocate(WrenVM* vm) {
	LongMatrix4x4* m = (LongMatrix4x4*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(LongMatrix4x4));
	LongVector* v1 = (LongVector*)wrenGetSlotForeign(vm, 1);
	LongVector* v2 = (LongVector*)wrenGetSlotForeign(vm, 2);
	LongVector* v3 = (LongVector*)wrenGetSlotForeign(vm, 3);
	LongVector* t = (LongVector*)wrenGetSlotForeign(vm, 4);
	*m = LongMatrix4x4(
		v1->x, v2->x, v3->x, t->x,
		v1->y, v2->y, v3->y, t->y,
		v1->z, v2->z, v3->z, t->z,
		0, 0, 0, 1
	);
}

void transform_getter(WrenVM* vm) {
	LongMatrix4x4* m = (LongMatrix4x4*)wrenGetSlotForeign(vm, 0);
	int i = (int)wrenGetSlotDouble(vm, 1);
	int j = (int)wrenGetSlotDouble(vm, 2);
	wrenSetSlotDouble(vm, 0, m->get_row(i)[j]);
}

void transform_setter(WrenVM* vm) {
	LongMatrix4x4* m = (LongMatrix4x4*)wrenGetSlotForeign(vm, 0);
	int i = (int)wrenGetSlotDouble(vm, 1);
	int j = (int)wrenGetSlotDouble(vm, 2);
	double x = wrenGetSlotDouble(vm, 3);
	m->set(i, j, x);
}

void transform_multiply_point(WrenVM* vm) {
	LongMatrix4x4* m = (LongMatrix4x4*)wrenGetSlotForeign(vm, 0);
	LongVector* v = (LongVector*)wrenGetSlotForeign(vm, 1);

	wrenGetVariable(vm, "linear_algebra", "Vec3", 0);
	LongVector* res = (LongVector*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(LongVector));
	*res = m->mul(*v, 1);
}

void transform_multiply_dir(WrenVM* vm) {
	LongMatrix4x4* m = (LongMatrix4x4*)wrenGetSlotForeign(vm, 0);
	LongVector* v = (LongVector*)wrenGetSlotForeign(vm, 1);

	wrenGetVariable(vm, "linear_algebra", "Vec3", 0);
	LongVector* res = (LongVector*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(LongVector));
	*res = m->mul(*v, 0);
}

void transform_multiply_matrix(WrenVM* vm) {
	LongMatrix4x4* m = (LongMatrix4x4*)wrenGetSlotForeign(vm, 0);
	LongMatrix4x4* m2 = (LongMatrix4x4*)wrenGetSlotForeign(vm, 1);

	wrenGetVariable(vm, "linear_algebra", "Transform", 0);
	LongMatrix4x4* res = (LongMatrix4x4*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(LongMatrix4x4));
	*res = *m * *m2;
}

void transform_multiply_scalar(WrenVM* vm) {
	LongMatrix4x4* m = (LongMatrix4x4*)wrenGetSlotForeign(vm, 0);
	double s = wrenGetSlotDouble(vm, 1);

	wrenGetVariable(vm, "linear_algebra", "Transform", 0);
	LongMatrix4x4* res = (LongMatrix4x4*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(LongMatrix4x4));
	*res = *m * s;
}

void transform_add(WrenVM* vm) {
	LongMatrix4x4* m = (LongMatrix4x4*)wrenGetSlotForeign(vm, 0);
	LongMatrix4x4* m2 = (LongMatrix4x4*)wrenGetSlotForeign(vm, 1);

	wrenGetVariable(vm, "linear_algebra", "Transform", 0);
	LongMatrix4x4* res = (LongMatrix4x4*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(LongMatrix4x4));
	*res = *m + *m2;
}

#pragma endregion

void ScriptingManager::register_functions_variables() {
	
	// INDEPENDANT FUNCTIONS
	api::FunctionConfiguration any = api::FunctionConfiguration("ANY", "ANY");
	SCRIPTAPI_ADD_FUNCTION_0_0(clear_screen, any)
	SCRIPTAPI_ADD_FUNCTION_1_1(test, api_string, api_string, any)
	SCRIPTAPI_ADD_FUNCTION_1_0(reload, api_int, any)
	SCRIPTAPI_ADD_FUNCTION_1_0(play_sound, api_string, any)

	__add_function(api::Function(focus_on, "focus_on(_,_)"));
	__add_function(api::Function(get_global_var, "get_global(_)"));
	__add_function(api::Function(set_global_var, "set_global(_,_)"));
	__add_function(api::Function(get_root, "get_data_root()"));
	__add_function(api::Function(get_root_agent, "get_game_root()"));

	// DATASTRUCTURE CLASS
	api::NativeClass data_structure = api::NativeClass("DataStructure");
	data_structure.constructor = api::Function(ds_allocate, "");
	data_structure.destructor = ds_finalize;
	data_structure.add_method(api::Function(ds_get_from_path, "get_ds(_)"));
	data_structure.add_method(api::Function(ds_get_name, "get_name()"));

	data_structure.add_method(api::Function(ds_get_int, "get_int(_,_,_)"));
	data_structure.add_method(api::Function(ds_get_double, "get_double(_,_,_)"));
	data_structure.add_method(api::Function(ds_get_bool, "get_bool(_,_,_)"));
	data_structure.add_method(api::Function(ds_get_string, "get_string(_,_,_)"));
	data_structure.add_method(api::Function(ds_get_vector, "get_vec(_,_,_)"));

	data_structure.add_method(api::Function(ds_get_int, "set_int(_,_)"));
	data_structure.add_method(api::Function(ds_get_double, "set_double(_,_)"));
	data_structure.add_method(api::Function(ds_get_bool, "set_bool(_,_)"));
	data_structure.add_method(api::Function(ds_get_string, "set_string(_,_)"));
	data_structure.add_method(api::Function(ds_get_vector, "set_vec(_,_)"));

	// RADIO CLASS
	api::NativeClass radio = api::NativeClass("Radio");
	radio.constructor = api::Function(radio_allocate, "");
	radio.destructor = radio_finalize;
	radio.add_method(api::Function(radio_add_signal, "add_signal(_)"));
	radio.add_method(api::Function(radio_clear_signal, "clear_signal(_)"));
	radio.add_method(api::Function(radio_emit_signal, "emit_signal(_,_)"));
	radio.add_method(api::Function(radio_connect, "connect(_,_,_,_)"));

	// AGENT CLASS
	api::NativeClass agent = api::NativeClass("Agent");
	agent.constructor = api::Function(agent_allocate, "");
	//agent.destructor = agent_destruct;
	agent.add_method(api::Function(agent_get_id, "get_id()"));
	agent.add_method(api::Function(agent_get_agency, "get_agency()"));
	agent.add_method(api::Function(agent_find_in_tree, "find_in_tree(_)"));
	agent.add_method(api::Function(agent_spawn_on, "spawn_on(_)"));
	agent.add_method(api::Function(agent_get_children, "get_children()"));
	agent.add_method(api::Function(agent_get_radio, "get_radio()"));
	agent.add_method(api::Function(agent_get_parent, "get_parent()"));
	agent.add_method(api::Function(agent_kill, "kill()"));  // Untested
	agent.add_method(api::Function(agent_is_valid, "is_instance_valid()"));  // Untested
	agent.add_method(api::Function(agent_get_type, "get_type()"));  // Untested
	agent.add_method(api::Function(agent_get_var, "get(_)"));
	agent.add_method(api::Function(agent_set_var, "set(_,_)"));
	agent.add_method(api::Function(agent_get_focus_transform, "get_focus_transform()"));

	// VECTOR CLASS
	api::NativeClass vector = api::NativeClass("Vec3");
	vector.constructor = api::Function(vector_allocate, "");
	vector.destructor = nullptr;
	vector.add_method(api::Function(vector_getter, "[_]"));
	vector.add_method(api::Function(vector_setter, "[_]=(_)"));

	// TRANSFORM CLASS
	api::NativeClass transform = api::NativeClass("Transform");
	transform.constructor = api::Function(transform_allocate, "");
	transform.destructor = nullptr;
	transform.add_method(api::Function(transform_getter, "[_,_]"));
	transform.add_method(api::Function(transform_setter, "[_,_]=(_)"));
	transform.add_method(api::Function(transform_multiply_point, "transform_point(_)"));
	transform.add_method(api::Function(transform_multiply_dir, "transform_direction(_)"));  // Untested
	transform.add_method(api::Function(transform_multiply_matrix, "mul(_)"));
	transform.add_method(api::Function(transform_multiply_scalar, "scale(_)"));  // Untested
	transform.add_method(api::Function(transform_add, "+(_)"));  // Untested

	__add_class(data_structure);
	__add_class(radio);
	__add_class(agent);
	__add_class(vector);
	__add_class(transform);

	api::add_variable(&scene_manager->time_scale, "timescale");
}
