
foreign class DataStructure {
	construct load(path) {}
	
	foreign get_name()
	foreign get_ds(path)
	
	foreign get_int(name, def, quiet)
	get_int(name, def){ get_int(name, def, false) }
	get_int(name){ get_int(name, 0, false) }
	foreign get_double(name, def, quiet)
	get_double(name, def){ get_double(name, def, false) }
	get_double(name){ get_double(name, 0, false) }
	foreign get_bool(name, def, quiet)
	get_bool(name, def){ get_bool(name, def, false) }
	get_bool(name){ get_bool(name, 0, false) }
	foreign get_string(name, def, quiet)
	get_string(name, def){ get_string(name, def, false) }
	get_string(name){ get_string(name, 0, false) }
	foreign get_vec(name, def, quiet)
	get_vec(name, def){ get_vec(name, def, false) }
	get_vec(name){ get_vec(name, 0, false) }
	
	foreign set_int(name, v)
	foreign set_double(name, v)
	foreign set_bool(name, v)
	foreign set_string(name, v)
	foreign set_vec(name, v)
	
	toString {
		return "DataStructure [%(get_name())]"
	}
}
