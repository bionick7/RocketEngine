#pragma once

#include "debug.h"

struct RadioSignal {
	std::string identifier;
	void* content;

	RadioSignal() { identifier = "UNIDENTIFIED"; content = nullptr; }
	RadioSignal(std::string p_id) { identifier = p_id; content = nullptr; }
	RadioSignal(std::string p_id, void* p_args) { identifier = p_id; content = p_args; }
};

typedef std::function<void(void*, RadioSignal)> RSInterpreter;

class Radio
{
public:
	Radio(void* p_callback_object, RSInterpreter p_callback_function);
	~Radio();

	// Called by client
	uint32_t add_code();
	void  reserve_code(uint32_t code);
	void clear_code(uint32_t code);
	void emit(uint32_t code);
	void emit_with_message(uint32_t sign_id, void* pre_content);

	//bool contains_messages();
	//struct RadioSignal pop_signal();

	// Called by other radio
	void connect(uint32_t code, Radio* reciever, RadioSignal signal);
	void recieve(RadioSignal signal);

private:
	struct Message {
		Radio* reciever;
		RadioSignal signal;

		Message(Radio* p_reciever, RadioSignal p_signal) { reciever = p_reciever, signal = p_signal; }
	};

	// Pointer because std::vector is weird
	ConstantIndexList<std::vector<Message>*> contacts = ConstantIndexList<std::vector<Message>*>();

	//std::vector<Message> contacts[256];
	//uint64_t contact_flags[4] = { 0, 0, 0, 0 };

	void* callback_object;
	RSInterpreter callback_function;
};

class DecoratedRadio: 
	public Radio
{
public:
	DecoratedRadio(void* p_callback_object, RSInterpreter p_callback);
	~DecoratedRadio();

	void add_signal(std::string signal_name);
	void add_signal_at(std::string signal_name, uint32_t index);
	void clear_signal(std::string signal_name);
	void emit_signal(std::string signal_name);
	void emit_signal(std::string signal_name, void* content);

	void connect(std::string signal_name, Radio* reciever, RadioSignal signal);

private:
	std::unordered_map<std::string, uint32_t> signal_lookup = std::unordered_map<std::string, uint32_t>();
};

