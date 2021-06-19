#include "radio.h"
// #include <bitset>

Radio::Radio(void* p_callback_object, RSInterpreter p_callback_function) {
	callback_function = p_callback_function;
	callback_object = p_callback_object;
}

Radio::~Radio() {
	for (int i = 0; i < contacts.allocated_size(); i++) {
		clear_code(i);
	}
}

uint32_t Radio::add_code() {
	return contacts.insert(new std::vector<Message>());
	//uint8_t code = 0;
	//uint64_t mask = 1;
	//for (int i = 0; i < 4; i++) {
	//	mask = 1;
	//	for (int j = 0; j < 64; j++) {
	//		if (~contact_flags[i] & mask) {
	//			contact_flags[i] |= mask;
	//			// std::cout << std::bitset<64>(contact_flags[i]) << std::endl;
	//			return code;
	//		}
	//		code++;
	//		mask <<= 1;
	//	}
	//}
}

void Radio::reserve_code(uint32_t code){
	contacts.overwrite(new std::vector<Message>(), code, nullptr);
	//contact_flags[code / 64] |= (uint64_t(1) << (code % 64));
}

void Radio::clear_code(uint32_t code) {
	delete contacts[code];
	contacts.erase(code);
	//contact_flags[code / 64] &= ~(1 << (code % 64));
}

void Radio::emit(uint32_t code) {
	for (Message m : *contacts[code]) {
		m.reciever->recieve(m.signal);
	}
}

void Radio::emit_with_message(uint32_t code, void* content) {
	for (Message m : *contacts[code]) {
		RadioSignal signal = m.signal;
		signal.content = content;
		m.reciever->recieve(signal);
	}
}

void Radio::connect(uint32_t code, Radio* reciever, RadioSignal signal) {
	Message m = Message(reciever, signal);
	contacts[code]->push_back(m);
}

void Radio::recieve(RadioSignal signal) {
	// std::cout << "Recieved: " << signal.identifier << std::endl;
	callback_function(callback_object, signal);
}

DecoratedRadio::DecoratedRadio(void* p_callback_object, RSInterpreter p_callback_function) : 
	Radio::Radio(p_callback_object, p_callback_function) { }

DecoratedRadio::~DecoratedRadio(){

}

void DecoratedRadio::add_signal(std::string signal_name) {
	signal_lookup.insert({ signal_name, add_code() });

}

void DecoratedRadio::add_signal_at(std::string signal_name, uint32_t index) {
	reserve_code(index);
	signal_lookup.insert({ signal_name, index });
}

void DecoratedRadio::clear_signal(std::string signal_name) {
	auto result = signal_lookup.find(signal_name);
	if (result != signal_lookup.end()) {
		Radio::clear_code(result->second);
	}
}

void DecoratedRadio::emit_signal(std::string signal_name) {
	auto result = signal_lookup.find(signal_name);
	if (result != signal_lookup.end()) {
		Radio::emit(result->second);
	}
}

void DecoratedRadio::emit_signal(std::string signal_name, void* content) {
	auto result = signal_lookup.find(signal_name);
	if (result != signal_lookup.end()) {
		Radio::emit_with_message(result->second, content);
	}
}

void DecoratedRadio::connect(std::string signal_name, Radio* reciever, RadioSignal signal) {
	auto result = signal_lookup.find(signal_name);
	if (result != signal_lookup.end()) {
		Radio::connect(result->second, reciever, signal);
	}
}
