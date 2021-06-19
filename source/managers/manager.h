#pragma once
#include "radio.h"

class Manager abstract
{
public:
	Manager();
	~Manager();
		
	void setup();
	virtual void step(double dt);
	virtual void interprete_signal(RadioSignal signal);

	DecoratedRadio* radio;
};
