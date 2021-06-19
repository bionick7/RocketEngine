#pragma once
#include "core.h"
#include "data_input/data_structure.h"
#include <typeinfo>

enum class AgentType;

#define SCRIPTAPI_ADD_VARIABLE(X) api::add_variable(&X, #X);

#define __SCRIPTAPI_START_F {WrenForeignMethodFn __internal = [](WrenVM* vm) {
#define __SCRIPTAPI_END_F(NAME, CONFIG) }; api::__add_function(api::Function(__internal, NAME, CONFIG));}
#define __SCRIPTAPI_END_M(NAME, CONFIG, CLASS) }; CLASS.add_method(api::Function(__internal, NAME, CONFIG));}
#define __SCRIPTAPI_GET_ARG(T, N) T v_##N = api::read_##T(vm, N);
#define __SCRIPTAPI_SET_RES(T) api::write_##T(vm, 0, res);

#define SCRIPTAPI_ADD_FUNCTION_0_0(FUNC, CONFIG) \
	__SCRIPTAPI_START_F\
	FUNC();\
	__SCRIPTAPI_END_F(#FUNC"()", CONFIG)

#define SCRIPTAPI_ADD_FUNCTION_1_0(FUNC, ARG0, CONFIG) \
	__SCRIPTAPI_START_F\
	__SCRIPTAPI_GET_ARG(ARG0, 1) \
	FUNC(v_1);\
	__SCRIPTAPI_END_F(#FUNC"(_)", CONFIG)

#define SCRIPTAPI_ADD_FUNCTION_2_0(FUNC, ARG0, ARG1, CONFIG) \
	__SCRIPTAPI_START_F\
	__SCRIPTAPI_GET_ARG(ARG0, 1) \
	__SCRIPTAPI_GET_ARG(ARG1, 2) \
	FUNC(v_1, v_2);\
	__SCRIPTAPI_END_F(#FUNC"(_,_)", CONFIG)

#define SCRIPTAPI_ADD_FUNCTION_0_1(FUNC, RES, CONFIG) \
	__SCRIPTAPI_START_F\
	RES res = FUNC();\
	__SCRIPTAPI_SET_RES(RES);\
	__SCRIPTAPI_END_F(#FUNC"()", CONFIG)

#define SCRIPTAPI_ADD_FUNCTION_1_1(FUNC, ARG0, RES, CONFIG) \
	__SCRIPTAPI_START_F\
	__SCRIPTAPI_GET_ARG(ARG0, 1) \
	RES res = FUNC(v_1);\
	__SCRIPTAPI_SET_RES(RES);\
	__SCRIPTAPI_END_F(#FUNC"(_)", CONFIG)

#define SCRIPTAPI_ADD_FUNCTION_2_1(FUNC, ARG0, ARG1, RES, CONFIG) \
	__SCRIPTAPI_START_F\
	__SCRIPTAPI_GET_ARG(ARG0, 1) \
	__SCRIPTAPI_GET_ARG(ARG1, 2) \
	RES res = FUNC(v_1, v_2);\
	__SCRIPTAPI_SET_RES(RES);\
	__SCRIPTAPI_END_F(#FUNC"(_,_)", CONFIG)

#define SCRIPTAPI_ADD_METHOD_0_0(FUNC, CONFIG, CLASS)\
	__SCRIPTAPI_START_F\
	FUNC();\
	__SCRIPTAPI_END_M(#FUNC"()", CONFIG, CLASS)

typedef int api_int;
typedef double api_float;
typedef bool api_bit;
typedef std::string api_string;

namespace api {
	enum class Type {
		INT32,
		FLOAT64,
		INT1,
		STRING,
		VEC3,
		NONE,
		ERR,
		FOREIGN,
	};

	struct Variable {
		void* ref;
		Type type;
		std::string name;

		// Only for foreign classes
		const char* parent_class;
		const char* parent_module;
		size_t alloc_size;

		Variable(void*, Type, const char*);
		Variable();
	};

	struct FunctionConfiguration {
		FunctionConfiguration();
		FunctionConfiguration(std::string, std::string);

		bool any_module;
		bool any_class;
		std::string module_;
		std::string class_;

		bool compare(const char*, const char*);
	};

	struct Function {
		WrenForeignMethodFn method;
		std::string name;
		bool valid;
		FunctionConfiguration config;

		Function();
		Function(WrenForeignMethodFn, const char*);
		Function(WrenForeignMethodFn, const char*, FunctionConfiguration);
	};

	struct NativeClass {
		Function constructor;
		WrenFinalizerFn destructor;
		std::unordered_map<std::string, Function> methods;

		std::string name;
		bool valid;


		NativeClass();
		NativeClass(const char*);
		void add_method(Function);
		WrenForeignClassMethods get_methods();
		WrenForeignMethodFn get_method_by_name(std::string);
	};

	struct CustomClass {
		CustomClass();
		CustomClass(std::string p_name, AgentType p_basis);
		
		std::string name;
		bool valid;
		AgentType basis;
	};


	Type get_type(const std::type_info&);

	api_int read_api_int(WrenVM* vm, int slot);
	api_float read_api_float(WrenVM* vm, int slot);
	api_bit read_api_bit(WrenVM* vm, int slot);
	api_string read_api_string(WrenVM* vm, int slot);

	void write_api_int(WrenVM* vm, int slot, api_int v);
	void write_api_float(WrenVM* vm, int slot, api_float v);
	void write_api_bit(WrenVM* vm, int slot, api_bit v);
	void write_api_string(WrenVM* vm, int slot, api_string v);
	
	void __add_variable(Variable);
	void __add_function(Function);
	void __add_class(NativeClass);
	
	template <typename T>
	void add_variable(T* var, const char* n) {
		Type type = get_type(typeid(T));
		if (type != Type::ERR) {
			Variable v = Variable((void*)var, type, n);
			__add_variable(v);
		}
	}

	class Library {
	public:
		Library();
		~Library();

		std::map<std::string, Variable> variable_map;
		std::map<std::string, Function> function_map;
		std::map<std::string, NativeClass> class_map;
		std::map<std::string, CustomClass> custom_class_map;
	};
}