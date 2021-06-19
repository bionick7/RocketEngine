#pragma once
#include "core.h"
#include "data_input/reader.h"
#include "api_base.h"
#include "manager.h"
#include "scriptable_module.h"

constexpr uint64_t CONSOLE_FLAG_UPDATE = 0x01;
constexpr uint64_t CONSOLE_FLAG_CLEAR = 0x02;

constexpr int CONSOLE_COMMAND_BUFFER_SIZE = 128;

std::string interprete_single_command(std::string input);
Agent* spawn_custom_class(std::string type_name);

struct Module {
	std::string name;
	int loading_order;
	struct LineEntry { uint16_t start, length; }; // Mesured with respect to the final result
	std::vector<LineEntry> line_entries;
	std::string path;
	std::string internal_code;

	Module();
	Module(io::DataStructurePtr);

	int get_original_linecount(uint16_t);
};

class ScriptingManager:
	public Manager
{
public:
	ScriptingManager();
	~ScriptingManager();

	void load_modules(io::DataStructurePtr);

	uint64_t console_flags;

	std::string* console_command_buffer;
	int cc_buffer_counter;

	void console_interpreteation(std::string);
	std::string run_buffered_commands();

	void write_to_console_directly(std::string);
	std::string get_console_text();
	void run_on_scene_load();
	void step(double) override;

	void register_functions_variables();

	void __add_variable(api::Variable);
	void __add_function(api::Function);
	void __add_class(api::NativeClass);

	api::Function get_function(std::string);
	api::NativeClass get_class(std::string);
	api::CustomClass get_custom_class(std::string);
	
	std::string current_var_name;
	api::Variable get_var(std::string);

	void interprete_signal(RadioSignal) override;

	std::vector<Module> code_modules = std::vector<Module>();
	ConstantIndexList<ScriptableModule*> scriptable_modules = ConstantIndexList<ScriptableModule*>();

	std::vector<ScriptableModule*> deferred_setup = std::vector<ScriptableModule*>();

	bool call_wren_handle(WrenHandle*);

	struct {
		WrenHandle* setup_handle;
		WrenHandle* step_handle;
		WrenHandle* radio_handle;
		WrenHandle* focus_transform_handle;
	} common_function_handles;

private:
	void configure_vm();
	void free_vm();

	std::string console_text;

	WrenVM* vm;
	WrenInterpretResult last_wren_result;

	std::string interprete_single_command_internal(std::string);

	WrenHandle* static_console_ref = nullptr;
	WrenHandle* console_interprete_func = nullptr;

	api::Library* api_library;

	bool check_result();
};

extern ScriptingManager* scripting_manager;
