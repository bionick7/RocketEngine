#include "api_base.h"
#include "scripting_manager.h"
#include "agent.h"

using namespace api;

Variable::Variable(void* pref, Type ptype, const char* pname) {
	ref = pref;
	type = ptype;
	name = pname;
}

Variable::Variable() {
	ref = nullptr;
	type = Type::ERR;
	name = "INVALID";
}

Function::Function(WrenForeignMethodFn pmethod, const char* pname, FunctionConfiguration pconfig) {
	method = pmethod;
	name = pname;
	valid = true;
	config = pconfig;
}

Function::Function(WrenForeignMethodFn pmethod, const char* pname) {
	method = pmethod;
	name = pname;
	valid = true;
	config = FunctionConfiguration("ANY", "ANY");
}

FunctionConfiguration::FunctionConfiguration() {
	any_module = true;
	any_class = true;
	module_ = "ANY";
	class_ = "ANY";
}

api::FunctionConfiguration::FunctionConfiguration(std::string pmodule, std::string pclass){
	module_ = pmodule;
	any_module = pmodule == "ANY";
	class_ = pclass;
	any_class = pclass == "ANY";
}

bool api::FunctionConfiguration::compare(const char* mod_name, const char* class_name) {
	std::hash<std::string> str_hash;
	if (!any_module && str_hash(std::string(mod_name)) != str_hash(module_)) return false;
	if (!any_module && str_hash(std::string(class_name)) != str_hash(class_)) return false;
	return true;
}

void placeholder(WrenVM* vm) {}
Function::Function() {
	method = placeholder;
	name = std::string("INVALID");
	valid = false;
}

// Default constructor
api::NativeClass::NativeClass() {
	constructor = Function();
	destructor = [](void* _) {};
	methods = std::unordered_map<std::string, Function>();

	name = std::string("INVALID");
	valid = false;
}

api::NativeClass::NativeClass(const char* pname) {
	constructor = Function();
	destructor = [](void* _) {};
	methods = std::unordered_map<std::string, Function>();

	name = std::string(pname);
	valid = true;
}

void api::NativeClass::add_method(Function f) {
	methods.insert({ f.name, f });
}

WrenForeignClassMethods api::NativeClass::get_methods() {
	WrenForeignClassMethods methods;
	methods.allocate = constructor.method;
	methods.finalize = destructor;
	return methods;
}

WrenForeignMethodFn api::NativeClass::get_method_by_name(std::string key) {
	auto v = methods.find(key);
	if (v != methods.end()) {
		return v->second.method;
	} else {
		return placeholder;
	}
}

api::CustomClass::CustomClass() {
	name = std::string("INVALID");
	valid = false;
	basis = AgentType::NONE;
};

api::CustomClass::CustomClass(std::string p_name, AgentType p_basis) {
	name = p_name;
	valid = true;
	basis = p_basis;
};

api::Type api::get_type(const std::type_info& info) {
	if (info.hash_code() == typeid(int).hash_code()) {
		return Type::INT32;
	}
	else if (info.hash_code() == typeid(double).hash_code()) {
		return Type::FLOAT64;
	}
	else if (info.hash_code() == typeid(bool).hash_code()) {
		return Type::INT1;
	}
	else if (info.hash_code() == typeid(std::string).hash_code()) {
		return Type::STRING;
	}
	else if (info.hash_code() == typeid(LongVector).hash_code()) {
		return Type::VEC3;
	}
	else if (info.hash_code() == typeid(nullptr_t).hash_code()) {
		return Type::NONE;
	}
	else {
		return Type::FOREIGN;
	}
}

api_int api::read_api_int(WrenVM* vm, int slot) {
	if (wrenGetSlotType(vm, slot) != WrenType::WREN_TYPE_NUM) {
		return 0;
	}
	return (api_int) wrenGetSlotDouble(vm, slot);
}

api_float api::read_api_float(WrenVM* vm, int slot) {
	if (wrenGetSlotType(vm, slot) != WrenType::WREN_TYPE_NUM) {
		return 0;
	}
	return (api_float) wrenGetSlotDouble(vm, slot);
}

api_bit api::read_api_bit(WrenVM* vm, int slot) {
	if (wrenGetSlotType(vm, slot) != WrenType::WREN_TYPE_BOOL) {
		return false;
	}
	return (api_bit) wrenGetSlotBool(vm, slot);
}

api_string api::read_api_string(WrenVM* vm, int slot) {
	if (wrenGetSlotType(vm, slot) != WrenType::WREN_TYPE_STRING) {
		return api_string("INVALID");
	}
	const char* c = wrenGetSlotString(vm, slot);
	return api_string(c);
}

void api::write_api_int(WrenVM* vm, int slot, api_int v) {
	wrenSetSlotDouble(vm, slot, (double)v);
}

void api::write_api_float(WrenVM* vm, int slot, api_float v) {
	wrenSetSlotDouble(vm, slot, (double)v);
}

void api::write_api_bit(WrenVM* vm, int slot, api_bit v) {
	wrenSetSlotBool(vm, slot, (bool)v);
}

void api::write_api_string(WrenVM* vm, int slot, api_string v) {
	const char* c = v.c_str();
	wrenSetSlotString(vm, slot, c);
}

void api::__add_variable(api::Variable v) {
	scripting_manager->__add_variable(v);
}

void api::__add_function(api::Function f) {
	scripting_manager->__add_function(f);
}

void api::__add_class(api::NativeClass f) {
	scripting_manager->__add_class(f);
}

api::Library::Library()
{
	variable_map = std::map<std::string, Variable>();
	function_map = std::map<std::string, Function>();
	class_map = std::map<std::string, NativeClass>();
	custom_class_map = std::map<std::string, CustomClass>();
}

api::Library::~Library()
{
}
