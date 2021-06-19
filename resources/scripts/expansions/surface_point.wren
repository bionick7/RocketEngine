import "linear_algebra" for Vec3, Transform
import "agent_interface" for Agency

class SurfacePoint is Agency {
	//foreign get_radio()
	
	construct new(x, y){
		super("SurfacePoint")
		
		_x_pos = x * 6.28
		_y_pos = y * 6.28
		agent.set("name", "Surface Point @ %(x); %(y)")
		_out = Vec3.set(
			_x_pos.sin * _y_pos.cos,
			_x_pos.cos * _y_pos.cos,
			_y_pos.sin
		)
		
		// Calculate transform  c.f. circle.cpp
		var v = Vec3.set(0.0, 1.0, 0.0)  // Chosen arbitrarily
		if (Vec3.dot(v, _out) > 0.9) {	 // Not risking linear dependance
			v = Vec3.set(1.0, 0.0, 0.0)
		}

		v = (v - _out * Vec3.dot(v, _out)).normalized()  // Project on plane to get orthogonal vector
	
		_custom_transform = Transform.set(
			v,
			Vec3.cross(v, _out),
			_out,
			Vec3.set(0, 0, 0)
		)
	}
	
	setup(){  // Called from deferred setup
		_planet = agent.get_parent()
		System.print(_planet)
		super.setup()
	}
	
	step(dt){
		//super.step(dt)
		
		if (agent.get_parent() == null){
			return null
		}
		var radius = agent.get_parent().get("radius")
		//System.print(agent.get_parent().get("surface_transform"))
		//System.print(_x_pos)
		
		var relative = _out * radius
		
		var world_pos = _planet.get("position") + _planet.get("surface_transform") * relative
				
		agent.set("position", world_pos)
	}
	
	get_focus_transform() {
		var add = _planet.get("surface_transform")		
		return add * super.get_focus_transform()
	}
	
	radio_interprete(signal){
		super.radio_interprete(signal)
	}
}
