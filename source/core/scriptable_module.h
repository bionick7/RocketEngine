#pragma once
#include "debug.h"
#include "wren.h"

class Agent;
struct AgentWrenData;
class ScriptableModule;
struct RadioSignal;

class ScriptableModule {
public:
	ScriptableModule();
	~ScriptableModule();

	void setup(WrenVM* p_vm, Agent* p_parent);
	void setup_deferred();
	void step(double dt);
	void interprete_signal(RadioSignal signal);
	LongMatrix4x4 fetch_focus_transform();

	Agent* parent;

private:
	WrenVM* vm;
	AgentWrenData* handle;

	bool is_ready();

	bool is_setup = false, is_setup_deferred = false;


	//std::map<std::string, api::Variable> 
};
