#include "mesh_resource.h"

MeshResource::MeshResource() : Resource::Resource() {}

MeshResource::MeshResource(io::DataStructurePtr data) {
	name = data->get_name();
	file_path = data->get_string("path");
	extra_size = data->get_double("size", 1.0, true);
	read_from_file(file_path);

	// wire stuff
}

MeshResource::~MeshResource() {
	kill(); // Needs to be written for each individual resource
}

bool MeshResource::deserialize(std::istream* in, int buffer_length) {
	in->seekg(0);
	std::string line;
	int line_count = 0;
	// First pass: count
	int pos = 0;
	while (std::getline(*in, line) && (pos < buffer_length || buffer_length <= 0))
	{
		if (buffer_length > 0) {
			line_count++;
			if (pos >= buffer_length) {
				line = line.substr(0, line.size() - pos + buffer_length);
			}
		}
		//if (line[0] == 'o' && line[1] == ' ') {
		//	name = line.substr(2);
		//}
		if (line[0] == 'v' && line[1] == ' ') {
			vertex_count++;
		}
		if (line[0] == 'l' && line[1] == ' ') {
			index_count += 2;
		}
		pos += line.size();
	}
	vertecies = new glm::vec3[vertex_count];
	indecies = new uint16_t[index_count];
	uint16_t vertex_idx = 0;
	uint16_t index_idx = 0;
	line_count = 0;
	in->clear();
	in->seekg(0);
	pos = 0;

	// Second pass: fill
	while (std::getline(*in, line) && (pos < buffer_length || buffer_length <= 0))
	{
		if (buffer_length > 0) {
			pos += line.size();
			if (pos >= buffer_length) {
				line = line.substr(0, line.size() - pos + buffer_length);
			}
		}
		if (line[0] == 'v' && line[1] == ' ') {
			vertecies[vertex_idx++] = (io::parse_vector(line.substr(2), ' ') * extra_size).to_float_vec();
		}
		if (line[0] == 'l' && line[1] == ' ') {
			std::string content = line.substr(2);
			uint16_t a, b;
			try {
				size_t semi1 = content.find_first_of(' ');

				a = io::parse_int32(content.substr(0, semi1));
				b = io::parse_int32(content.substr(semi1 + 1));
			}
			catch (std::invalid_argument) {
				errlog("\"" + content + "\" does not contain 2 valid integers line: " + std::to_string(line_count) + "; " + file_path);
			}
			indecies[index_idx++] = a - 1;
			indecies[index_idx++] = b - 1;
		}
		line_count++;
	}
	return true;
}

bool MeshResource::serialize(std::ostream* in, int* buffer_length) {
	in->clear();
	std::streampos start = in->tellp();
	*in << "# Generated by the Rocket Game Engine" << std::endl;
	*in << "o " << name << std::endl;
	for (int i = 0; i < vertex_count; i++) {
		*in << "v " << vertecies[i].x << " " << vertecies[i].y << " " << vertecies[i].z << std::endl;
	}
	for (int i = 0; i < get_segment_count(); i++) {
		*in << "l " << indecies[i * 2] + 1 << " " << indecies[i * 2 + 1] + 1 << std::endl;
	}
	*buffer_length = in->tellp() - start;
	return true;
}

void MeshResource::kill() {
	Resource::kill();
	if (vertecies != nullptr) {
		delete[] vertecies;
		vertecies = nullptr;
	}
	if (indecies != nullptr) {
		delete[] indecies;
		indecies = nullptr;
	}
}

io::ResourceType MeshResource::get_type() {
	return io::ResourceType::NONE;
}

uint16_t const MeshResource::get_vertex_count() {
	return vertex_count;
}

uint16_t const MeshResource::get_triangle_count() {
	return index_count / 3;
}

uint16_t const MeshResource::get_segment_count() {
	return index_count / 2;
}

void MeshResource::generate_vertecies(uint16_t count) {
	if (vertecies != nullptr) {
		delete[] vertecies;
	}
	vertex_count = count;
	vertecies = new glm::vec3[vertex_count];
}

void MeshResource::generate_segments(uint16_t count) {
	if (indecies != nullptr) {
		delete[] indecies;
	}
	index_count = count * 3;
	indecies = new uint16_t[count * 3];
}

void MeshResource::generate_triangles(uint16_t count) {
	if (indecies != nullptr) {
		delete[] indecies;
	}
	index_count = count * 3;
	indecies = new uint16_t[count * 3];
}

glm::vec3 MeshResource::get_vertex(uint16_t index) {
	if (index < vertex_count) {
		return vertecies[index];
	}
	else {
		infolog("Index out of bounds: Tried to index vertex " + std::to_string(index) + " even though there are only " + std::to_string(vertex_count));
	}
}

Segment MeshResource::get_segment(uint16_t index) {
	if (index < get_triangle_count()) {
		return {
			indecies[index * 3 + 0],
			indecies[index * 3 + 1]
		};
	}
	else {
		errlog("Index out of bounds: Tried to index triangle " + std::to_string(index) + " even though there are only " + std::to_string(get_segment_count()));
	}
}

Triangle MeshResource::get_triangle(uint16_t index) {
	if (index < get_triangle_count()) {
		return {
			indecies[index * 3 + 0],
			indecies[index * 3 + 1],
			indecies[index * 3 + 2]
		};
	}
	else {
		errlog("Index out of bounds: Tried to index triangle " + std::to_string(index) + " even though there are only " + std::to_string(get_triangle_count()));
	}
}

void MeshResource::set_vertex(uint16_t index, glm::vec3 value) {
	if (index < vertex_count) {
		vertecies[index] = value;
	}
	else {
		errlog("Index out of bounds: Tried to index vertex " + std::to_string(index) + " even though there are only " + std::to_string(vertex_count));
	}
}

void MeshResource::set_segment(uint16_t index, uint16_t a, uint16_t b) {
	if (index < get_triangle_count()) {
		indecies[index * 3 + 0] = a;
		indecies[index * 3 + 1] = b;
	}
	else {
		errlog("Index out of bounds: Tried to index triangle " + std::to_string(index) + " even though there are only " + std::to_string(get_segment_count()));
	}
}


void MeshResource::set_triangle(uint16_t index, uint16_t a, uint16_t b, uint16_t c) {
	if (index < get_triangle_count()) {
		indecies[index * 3 + 0] = a;
		indecies[index * 3 + 1] = b;
		indecies[index * 3 + 2] = c;
	}
	else {
		errlog("Index out of bounds: Tried to index triangle " + std::to_string(index) + " even though there are only " + std::to_string(get_triangle_count()));
	}
}
