
Agent::Agent() {
	flags = 0;
	cycle = LifeCycle::Instantiated;
	shapes = new Shape* [MAX_SHAPES];
	for (int i = 0; i < MAX_SHAPES; i++) {
		shapes[i] = nullptr;
	}
}

Agent::~Agent() {
	for (int i = 0; i < MAX_SHAPES; i++) {
		if (shapes[i] != nullptr) {
			delete shapes[i];
		}
	}
	delete[] shapes;
}

void Agent::instantiate() {
	cycle = LifeCycle::Instantiated;
}

void Agent::destroy() {
	cycle = LifeCycle::Dead;
}

void Agent::draw(const Camera* cam) {
	for (int i = 0; i < MAX_SHAPES; i++) {
		if (shapes[i] != nullptr) {
			shapes[i]->draw(cam);
		}
	}
}

void Agent::logic_step(double delta) {

}

int Agent::add_shape(Shape* shape) {
	int id = 0;
	while (shapes[id] != nullptr) id++;
	shapes[id] = shape;
	shape_number++;
	return id;
}

void Agent::delete_shape(int id, bool disconnect_only=false) {
	if (!disconnect_only) {
		delete shapes[id];
	}
	shapes[id] = nullptr;
	shape_number--;
}

Shape* Agent::get_shape(int id) {
	return shapes[id];
}

Type Agent::get_type() {
	return Type::NONE;
}