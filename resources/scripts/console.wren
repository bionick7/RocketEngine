import "surface_point" for SurfacePoint
import "linear_algebra" for Vec3, Transform

class Console {
	foreign static focus_on(agent, direct)
	foreign static reload(id)
	foreign static clear_screen()
	
	//foreign static play_sound(name, repeating)
	
	static capitalize(inp){
		var res = ""
		for (b in inp.bytes){
			res = res + String.fromByte((b > 96 && b < 124) ? b - 32: b)
		}
		return res
	}
	
	static is_num(s){
		return true
	}

	static focus(args){
		if (args.count == 0){
			return "Must specify a valid orbiter name to focus on"
		}
		if (args.count > 1 && args[1] == "*D"){
			var tgt = get_game_root().find_in_tree(args[0])
			focus_on(tgt, true)
			return "Focus: %(args[0])"
		} else if (args.count > 3 && args[1] == "*S"){
			var x = Num.fromString(args[2])
			var y = Num.fromString(args[3])
			if (x == null || y == null || x < -180 || x > 180 || y < -90 || y > 90) {
				return "Invalid coordinates: (%(args[2]); %(args[3]))"
			}
			var attachment = SurfacePoint.new(x / 360 % 1.0, y / 360)
			
			var concerned_agent = get_game_root().find_in_tree(args[0])
			if (concerned_agent == null){
				return "No such Celestial: %(args[0])"
			}
			attachment.agent.spawn_on(concerned_agent)
			System.print(concerned_agent.get_type())
			if (concerned_agent.get_type() != "Celestial") {
				return "Cannot focus on the surface of an agent which isn't a celestial body"
			}
			focus_on(attachment.agent, args.contains("*D"))
			return "Focus: %(args[0]) @ (%(args[2]); %(args[3]))"
		} else {
			var tgt = get_game_root().find_in_tree(args[0])
			focus_on(tgt, false)
			return "Focus: %(args[0])"
		}
	}

	static acc_time(a) {
		var time_scale = get_global("timescale")
		time_scale = time_scale * a
		set_global("timescale", time_scale)
		var inc = a > 1 ? "increased" : "decreased"
		return "timescale %(inc) to %(time_scale)"
	}

	static run_on_startup(){
		//var v1 = Vec3.set(-0.5, 9, 1)
		//var v2 = Vec3.set(1.4, -7.8, 3)
		//System.print(Vec3.dot(v1, v2))
		//System.print(Vec3.cross(v1, v2))
		//System.print(Vec3.angle(v1, v2))
		//System.print(Transform.identity)
		
		//var root_structure = get_data_root()
		//var root_agent = get_game_root()
		//System.print(root_agent.toString + "; " + root_agent.get_var("position").toString)
		//var child = root_structure.get_ds("celestial information/Sun")
		//var radius = root_agent.get_var("radius")
		//System.print(root_agent.toString + "; " + radius.toString)
	}

	static interprete(input){
		var args = input.split(" ")
		var command = capitalize(args[0])
		args = args[1..-1]
		if (command == "CLS"){
			clear_screen()
			return ""
		} else if (command == "FOCUS"){
			return focus(args)
		} else if (command == "ACC"){
			return acc_time(3)
		} else if (command == "DEC"){
			return acc_time(0.33)
		} else if (command == "TIME"){
			if (args.count > 0 && is_num(args[0])) {
				set_global("timescale", Num.fromString(args[0]))
				return "timescale %(Num.fromString(args[0]))"
			}
			return "Argument not a valid integer"
		} else if (command == "TRANSFER"){
			return "*calculates transfer"
		} else if (command == "TEST"){
			return test(" and Here's the runtime code")
		} else if (command == "RELOAD"){
			if (args.count > 0){
				if (capitalize(args[0]) == "SETTINGS"){
					reload(0)
					return ""
				} else if (capitalize(args[0]) == "ASSETS") {
					reload(1)
					return ""
				} else if (capitalize(args[0]) == "SCRIPTS") {
					reload(2)
					return ""
				}
				return "No such reload identifier: %(args[0])"
			} else {
				return "Argument count has to be at least 1"
			}
		} else {
			return "No such known command: %(command)"
		}
	}
}
