#include "scripting_manager.h"
#include "ui_manager.h"
#include "CameraContainer.h"
#include "Celestial.h"
#include "Orbiter.h"
#include "custom_library.h"

const std::string IMPOSSIBLE_PATH = "\\.\\NONE";

Module::Module() {
	name = "INVALID";
	loading_order = 0;
	path = IMPOSSIBLE_PATH;
	line_entries = std::vector<LineEntry>();
	internal_code = "";
}

Module::Module(io::DataStructurePtr ds) {
	name = ds->get_name();
	loading_order = ds->get_int("loading order", 0);
	line_entries = std::vector<LineEntry>();
	if (ds->get_bool("internal", false, true)) {
		internal_code = ds->get_string("source", "");
		path = IMPOSSIBLE_PATH;
	} else {
		internal_code = "";
		path = ds->get_string("path", "");
	}
}

int Module::get_original_linecount(uint16_t in) {
	int res = in;
	for (LineEntry li : line_entries) {
		if (li.start < in) {
			if (li.length + li.start > in) {
				return li.length + li.start - in;
			}
			res -= li.length;
		} else {
			return res;
		}
	}
	return res;
}

Agent* spawn_custom_class(std::string type_name) {
	api::CustomClass custom_class = scripting_manager->get_custom_class(type_name);
	if (!custom_class.valid) {
		errlog("No such custom class: " + type_name);
		return nullptr;
	}
	switch (custom_class.basis)
	{
	case AgentType::CAMERA:
		return new CameraContainer();
	case AgentType::CELESTIAL:
		return new Celestial();  // Uninitialised
	case AgentType::DEFAULT_SCRIPTABLE:
		return new BlankAgent();
	case AgentType::NONE:
	default: return nullptr;
	}
}

static void writeFn(WrenVM* vm, const char* text)
{
	if (strcmp(text, "\n") != 0) {
		debug.out << text << std::endl;
		debug.submit_to(DebugOutput::WREN_INFO);
	}
}

void errorFn(WrenVM* vm, WrenErrorType errorType,
	const char* module, const int p_line,
	const char* msg)
{
	if (module == nullptr) {
		module = "---";
	}

	if (msg == nullptr) {
		msg = "---";
	}

	int line = p_line;
	for (Module m : scripting_manager->code_modules) {
		if (m.name == std::string(module)) {
			line = m.get_original_linecount(p_line);
		}
	}

	switch (errorType)
	{
	case WREN_ERROR_COMPILE:
		debug.out << "Wren compilation error: mod \"" << module << "\", L " << line << " => \"" << msg << "\"" << std::endl;
		break;
	case WREN_ERROR_STACK_TRACE:
		debug.out << "  ^^^ mod \"" << module << "\", L " << line << " @ " << msg << " ^^^" << std::endl;
		break;
	case WREN_ERROR_RUNTIME:
		debug.out << "Wren runtime error: \"" << msg << "\"" << std::endl;
		break;
	}
	debug.submit_to(DebugOutput::WREN_ERROR);
}

void notimplementedFn(WrenVM* vm) {
	debug.out << "Tried to call non implented foreign function" << std::endl;
	debug.submit_to(DebugOutput::WREN_ERROR);
}
void ScriptingManager::__add_variable(api::Variable v) {
	api_library->variable_map.insert({ v.name, v });
}

void ScriptingManager::__add_function(api::Function f) {
	api_library->function_map.insert({ f.name, f });
}


void ScriptingManager::__add_class(api::NativeClass f) {
	api_library->class_map.insert({ f.name, f });
}

WrenForeignMethodFn bindForeignMethod(
	WrenVM* vm,
	const char* module,
	const char* className,
	bool isStatic,
	const char* signature)
{
	// Only gets called on startup
	if (isStatic) {
		api::Function res = scripting_manager->get_function(signature);
		if (res.valid && res.config.compare(module, className)) {
			return res.method;
		}
		debug.out << "No such global function: " << signature << std::endl;
		debug.submit_to(DebugOutput::WREN_ERROR);
	}
	else {
		api::NativeClass nc = scripting_manager->get_class(className);
		if (nc.valid) {
			return nc.get_method_by_name(signature);
		}
		api::CustomClass cc = scripting_manager->get_custom_class(className);
		if (cc.valid) {
			return WrenForeignMethodFn();  // TBD
		}

		debug.out << "No such class: " << className << std::endl;
		debug.submit_to(DebugOutput::WREN_ERROR);
	}
	return notimplementedFn;
}

WrenForeignClassMethods bindForeignClass(
	WrenVM* vm,
	const char* module,
	const char* className) 
{
	api::NativeClass nc = scripting_manager->get_class(className);
	if (nc.valid) {
		return nc.get_methods();
	}
	return WrenForeignClassMethods();
}

void __debug_write_to_console(std::string s) {
	scripting_manager->write_to_console_directly(s);
}

ScriptingManager::ScriptingManager() {
	console_command_buffer = new std::string[CONSOLE_COMMAND_BUFFER_SIZE];
	cc_buffer_counter = 0;
	console_flags = 0;

	api_library = new api::Library();
}

ScriptingManager::~ScriptingManager() {
	free_vm();
	delete[] console_command_buffer;
	delete api_library;
	if (console_interprete_func != nullptr) wrenReleaseHandle(vm, console_interprete_func);
	if (static_console_ref != nullptr)wrenReleaseHandle(vm, static_console_ref);
}

bool ScriptingManager::call_wren_handle(WrenHandle* handle) {
	last_wren_result = wrenCall(vm, handle);
	if (check_result()) {
		return true;
	}
	return false;
}

void ScriptingManager::configure_vm() {
	WrenConfiguration config;
	wrenInitConfiguration(&config);
	config.writeFn = &writeFn;
	config.errorFn = &errorFn;
	config.bindForeignMethodFn = &bindForeignMethod;
	config.bindForeignClassFn = &bindForeignClass;
	vm = wrenNewVM(&config);
	check_result();

	// Pre3load symbols
	common_function_handles.setup_handle = wrenMakeCallHandle(vm, "setup()");
	common_function_handles.step_handle = wrenMakeCallHandle(vm, "step(_)");
	common_function_handles.radio_handle = wrenMakeCallHandle(vm, "radio_interprete(_)");
	common_function_handles.focus_transform_handle = wrenMakeCallHandle(vm, "get_focus_transform()");
}

void ScriptingManager::free_vm() {
	if (vm != nullptr) {
		wrenFreeVM(vm);
	}
}

api::Variable ScriptingManager::get_var(std::string var_name) {
	auto r = api_library->variable_map.find(var_name);
	if (r == api_library->variable_map.end())
		return api::Variable();
	return r->second;
}

void ScriptingManager::interprete_signal(RadioSignal signal) {

}

api::Function ScriptingManager::get_function(std::string key)
{
	auto r = api_library->function_map.find(key);
	if (r == api_library->function_map.end())
		return api::Function();
	return r->second;
}

api::NativeClass ScriptingManager::get_class(std::string key)
{
	auto r = api_library->class_map.find(key);
	if (r == api_library->class_map.end())
		return api::NativeClass();
	return r->second;
}

api::CustomClass ScriptingManager::get_custom_class(std::string key)
{
	auto r = api_library->custom_class_map.find(key);
	if (r == api_library->custom_class_map.end())
		return api::CustomClass();
	return r->second;
}

void ScriptingManager::load_modules(io::DataStructurePtr ds) {
	free_vm();
	configure_vm();
	register_functions_variables();

	std::string default_include = ds->get_child("general")->get_string("default include text", "");

	for (io::DataStructurePtr child : ds->get_child("custom classes")->get_all_children()) {
		std::string name = child->get_name();
		api::CustomClass cc = api::CustomClass(name, (AgentType) child->get_int("type", 0));
		api_library->custom_class_map.insert({ name, cc });
		infolog(name + " Class registered");
	}

	//std::cout << ds->get_content_string() << std::endl;
	code_modules.clear();

	for (io::DataStructurePtr child : ds->get_child("modules")->get_all_children()) {
		Module m = Module(child);
		code_modules.push_back(m);
	}

	auto cmp = [](Module a, Module b) {
		return a.loading_order < b.loading_order;
	};
	std::sort(code_modules.begin(), code_modules.end(), cmp);

	std::regex class_match("\class\\s+\\w+\\s+\\{");

	for (auto it = code_modules.begin(); it != code_modules.end(); it++) {
		infolog("Loading module " + it->name + "...");

		bool success;
		std::vector<std::string> source_code;

		if (it->path == IMPOSSIBLE_PATH) {
			source_code = split_string_to_lines(it->internal_code); // split into lines
			success = true;
		}
		else {
			int size;
			std::string path = it->path;
			source_code = io::read_lines_from_file(path, "", &success, true);
			if (!success) {
				errlog("couldn't open " + path);
			}
		}

		if (success) {
			int line_num = 0, end_line_num = 0;
			std::string new_source_code = "";
			for (std::string line : source_code) {
				new_source_code += line + "\n";
				if (std::regex_match(line, class_match)) {
					new_source_code += "\n" + default_include + "\n";

					Module::LineEntry le = Module::LineEntry();
					le.start = end_line_num;
					le.length = std::count(default_include.begin(), default_include.end(), '\n') + 2u;
					end_line_num += le.length;
					it->line_entries.push_back(le);
				}
				end_line_num++;
				line_num++;
			}

			last_wren_result = wrenInterpret(vm, it->name.c_str(), new_source_code.c_str());
			infolog("Success");
		}
		else {
			infolog("Couldn't find source");
		}
	}

	// Not modular/flexible >:(
	wrenEnsureSlots(vm, 1);
	
	wrenGetVariable(vm, "console", "Console", 0);
	static_console_ref = wrenGetSlotHandle(vm, 0);
	console_interprete_func = wrenMakeCallHandle(vm, "interprete(_)");
}


void ScriptingManager::console_interpreteation(std::string input) {
	if (cc_buffer_counter >= CONSOLE_COMMAND_BUFFER_SIZE - 1) {
		debug.out << "Can only buffer up to " << CONSOLE_COMMAND_BUFFER_SIZE << " Console commands per frame.";
		debug.submit_to(DebugOutput::ERROR);
	}
	else {
		console_command_buffer[cc_buffer_counter++] = input;
	}
}

std::string ScriptingManager::run_buffered_commands() {
	std::string res = "";
	for (int i = 0; i < cc_buffer_counter; i++) {
		res += "> " + console_command_buffer[i] + "\n";
		if (console_command_buffer[i].size() > 0) {
			res += interprete_single_command_internal(console_command_buffer[i]) + "\n";
		}
	}
	if (cc_buffer_counter > 0) {
		scripting_manager->console_flags |= CONSOLE_FLAG_UPDATE;
	}
	cc_buffer_counter = 0;
	//if (res != "") {
	//	std::cout << res << std::endl;
	//}
	return res;
}

void ScriptingManager::write_to_console_directly(std::string s) {
	console_text += s;
}

std::string ScriptingManager::get_console_text()
{
	return console_text;
}

void ScriptingManager::step(double dt) {
	Manager::step(dt);
	// Console stuff
	console_text = run_buffered_commands();
	for (ScriptableModule* sm : deferred_setup) {
		sm->setup_deferred();
	}
	deferred_setup.clear();
}

void ScriptingManager::run_on_scene_load()
{
	if (static_console_ref != nullptr) {
		wrenSetSlotHandle(vm, 0, static_console_ref);
		WrenHandle* startup_func = wrenMakeCallHandle(vm, "run_on_startup()");
		last_wren_result = wrenCall(vm, startup_func);
	}
}

std::string ScriptingManager::interprete_single_command_internal(std::string cmd)
{
	if (console_interprete_func == nullptr || static_console_ref == nullptr)
		return "---";
	wrenEnsureSlots(vm, 2);
	wrenSetSlotHandle(vm, 0, static_console_ref);
	wrenSetSlotString(vm, 1, cmd.c_str());

	last_wren_result = wrenCall(vm, console_interprete_func);
	if (check_result()) {
		return std::string(wrenGetSlotString(vm, 0));
	}
	return "An error occured";
}

bool ScriptingManager::check_result()
{
	switch (last_wren_result) {
	case WREN_RESULT_COMPILE_ERROR:
		errlog("Wren compilation error");
		return false;
	case WREN_RESULT_RUNTIME_ERROR:
		errlog("Wren runtime error");
		return false;
	case WREN_RESULT_SUCCESS:
		return true;
	}
	return false;
}