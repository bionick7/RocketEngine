#pragma once
#include "Orbiter.h"
#include "mesh.h"
#include "scene_manager.h"

class SpaceShip : public Orbiter {
public:
	SpaceShip(io::DataStructurePtr, Orbiter*);

	void draw_step(double) override;

	Mesh* main_shape;
};

