class UIManager {
	foreign spawn_element(type)
	foreign set_transform(id, x, y, width, height)
}

class ScriptManager {

}

class GraphicsManager {
	
}

class HierarchyManager {
	foreign get_field(field_name)
	foreign set_field(field_name, value)
	foreign get_agent(path)
}

class Settings {
	foreign get_field(path)
}