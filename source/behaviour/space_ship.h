#pragma once
#include "Orbiter.h"
#include "mesh.h"

class SpaceShip : public Orbiter {
public:
	SpaceShip(io::DataStructure*, Orbiter*);

	void draw_step(double) override;

	Mesh* main_shape;
};

