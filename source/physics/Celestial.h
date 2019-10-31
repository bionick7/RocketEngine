#pragma once
#include "../graphics/circle.h"
#include "Orbiter.h"

//typedef unsigned short pixel_indx;

//struct ScreenPos { pixel_indx x; pixel_indx y; };

class Celestial : public Orbiter
{
public:
	Celestial(cfg::DataStructure*);

	double Mu();
	void draw(Camera*) override;
	void update(double) override;

	double radius;

	double mass;
	Circle* circle;
	Orbiter* get_sattelite(unsigned position);

private:
	unsigned short sattelites_num;
	std::vector<Orbiter*> sattelites;

	short radius_meta;
};
