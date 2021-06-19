foreign class Radio {
	foreign add_signal(signal_name)
	foreign clear_signal(signal_name)
	foreign emit_signal(signal_name, content)
	emit_signal(signal_name){ 
		emit_signal(signal_name, null)
	}
	foreign connect(signal_name, reciever, callback_name, default_content)
	connect(signal_name, reciever, callback_name) {
		connect(signal_name, reciever, callback_name, null)
	}
}

// Foreign class contains struct with info about foreign behaviour
foreign class Agent {
	construct new(basis_typename, ref) {}
	
	foreign get_id()
	foreign get_agency()
	foreign get(name)
	foreign set(name, value)

	foreign spawn_on(parent)
	foreign get_children()
	foreign get_parent()
	foreign get_radio()
	//foreign get_in_tree()
	foreign find_in_tree(child_name)
	foreign get_focus_transform()
	
	foreign kill()
	foreign is_instance_valid()
	foreign get_type()
	
	toString {
		return "Agent %(get("name"))"
	}
}

// Inheritable class
class Agency {
	unique_id { _agent.get_id() }
	agent { _agent }
	
	construct new(basis_typename) {
		_agent = Agent.new(basis_typename, this)
	}
	
	construct from_existing(agent) {
		_agent = agent
	}
	
	get(name) {
		return _agent.get(name)
	}
	
	set(name, value) {
		_agent.set(name, value)
	}
	
	is_instance_valid() {
		_agent.is_instance_valid()
	}
	
	setup() {}
	step(dt) {}
	die() {}
	radio_interprete(x) {}
	
	get_focus_transform() {
		return _agent.get_focus_transform()
	}
	
	toString {
		return "Agency of %(agent.get("name"))"
	}
}