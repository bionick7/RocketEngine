#include "manager.h"

void interprete_callback(void* obj, RadioSignal signal) {
	((Manager*)obj)->interprete_signal(signal);
}

Manager::Manager() {
	radio = new DecoratedRadio(this, &interprete_callback);
}

Manager::~Manager() {
	delete radio;
}

void Manager::setup() {
}


void Manager::interprete_signal(RadioSignal signal) {

}

void Manager::step(double dt) {

}
