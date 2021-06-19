#include "scriptable_module.h"
#include "agent.h"
#include "scripting_manager.h"
#include "radio.h"

ScriptableModule::ScriptableModule() {
}

ScriptableModule::~ScriptableModule() {
	wrenReleaseHandle(vm, handle->object_handle);
}

void ScriptableModule::setup(WrenVM* p_vm, Agent* p_parent) {
	parent = p_parent;
	is_setup = true;
	handle = p_parent->wren_data;
	vm = p_vm;

	scripting_manager->deferred_setup.push_back(this);
}

void ScriptableModule::setup_deferred() {  // Used because recursive wren - C doesn't work :(
	if (is_ready()) {
		wrenEnsureSlots(vm, 1);
		wrenSetSlotHandle(vm, 0, handle->object_handle);
		scripting_manager->call_wren_handle(scripting_manager->common_function_handles.setup_handle);
	}
	is_setup_deferred = true;
}


bool ScriptableModule::is_ready() {
	return handle != nullptr && handle->object_handle != nullptr && vm != nullptr;
}

void ScriptableModule::step(double dt) {
	if (is_ready()) {
		wrenEnsureSlots(vm, 2);
		wrenSetSlotHandle(vm, 0, handle->object_handle);
		wrenSetSlotDouble(vm, 1, dt);
		//std::cout << "Arrive at " << parent->name << std::endl;
		scripting_manager->call_wren_handle(scripting_manager->common_function_handles.step_handle);
	}
}

void ScriptableModule::interprete_signal(RadioSignal signal) {
	wrenEnsureSlots(vm, 2);
	wrenSetSlotHandle(vm, 0, handle->object_handle);
	//wrenSetSlotDouble(vm, 1, signal);		TODO
	scripting_manager->call_wren_handle(scripting_manager->common_function_handles.radio_handle);  // Untested
}

LongMatrix4x4 ScriptableModule::fetch_focus_transform() {
	wrenEnsureSlots(vm, 2);
	wrenSetSlotHandle(vm, 0, handle->object_handle);
	scripting_manager->call_wren_handle(scripting_manager->common_function_handles.focus_transform_handle);
	LongMatrix4x4* res = (LongMatrix4x4*)wrenGetSlotForeign(vm, 0);
	return *res;
}
