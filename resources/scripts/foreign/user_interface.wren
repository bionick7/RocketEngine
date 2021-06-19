import "linear_algebra" for Vec3

foreign class Canvas {
	foreign spawn(ui_element)
	foreign delete(ui_element)
}

foreign class Element {
	Vec3 position
}
