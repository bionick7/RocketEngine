#pragma once
#include "core.h"
#include "manager.h"
#include "data_input/data_structure.h"

class Agent;
class Orbiter;
class CameraContainer;

class SceneManager:
	public Manager
{
public:
	SceneManager();
	~SceneManager();

	void step(double) override;
	void init_scene(io::DataStructurePtr);

	Orbiter* orbit_root;
	CameraContainer* main_camera;

	std::vector<Agent*> loose_agents = std::vector<Agent*>();

	double time, real_time; // seconds
	double time_delta, real_time_delta; // seconds

	double time_scale;

	void interprete_signal(RadioSignal) override;
};

extern SceneManager* scene_manager;
