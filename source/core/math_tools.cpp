#include "math_tools.h"

Screenpos::Screenpos(int _x, int _y) {
	x = (pixel) _x;
	y = (pixel) _y;
}

Screenpos::Screenpos() {
	x = 0;
	y = 0;
}

GLPos::GLPos(float _x, float _y) {
	x = _x;
	y = _y;
}

GLPos::GLPos() {
	x = 0;
	y = 0;
}

LongVector::LongVector(glm::vec3 vec) {
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

LongVector::LongVector(float _x, float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
}

LongVector::LongVector(LongVectorType _x, LongVectorType _y, LongVectorType _z) {
	x = _x;
	y = _y;
	z = _z;
}

LongVector::LongVector() {
	x = 0;
	y = 0;
	z = 0;
}

LongVector::~LongVector() {

}

LongVector LongVector::operator+ (LongVector other) {
	return LongVector(x + other.x, y + other.y, z + other.z);
}

LongVector LongVector::operator- (LongVector other) {
	return LongVector(x - other.x, y - other.y, z - other.z);
}

LongVector LongVector::operator* (LongVectorType d) {
	return LongVector(x * d, y * d, z * d);
}

LongVector LongVector::operator/ (LongVectorType d) {
	return LongVector(x / d, y / d, z / d);
}

double LongVector::distance_to(LongVector other) {
	LongVectorType dx = x - other.x, dy = y - other.y, dz = z - other.z;
	return sqrt(dx*dx + dy * dy + dz * dz);
}

glm::vec3 const LongVector::to_float_vec() {
	return glm::vec3(x, y, z);
}

LongVector LongVector::normalized() {
	return *this / longvec_magnitude(*this);
}

LongQuaternion::LongQuaternion(double _x, double _y, double _z, double _w) {
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

LongQuaternion::LongQuaternion(double angle, LongVector axis, bool angle_in_rad) {
	if (!angle_in_rad) {
		angle *= deg2rad;
	}
	x = axis.x * sin(angle / 2);
	y = axis.y * sin(angle / 2);
	z = axis.z * sin(angle / 2);
	w = cos(angle / 2);
}

LongQuaternion::LongQuaternion() {
	x = 0;
	y = 0;
	z = 0;
	w = 1;
}

LongQuaternion::~LongQuaternion() {

}

LongVector LongQuaternion::operator* (LongVector vec) {
	LongQuaternion res = get_inverse() * LongQuaternion(vec.x, vec.y, vec.z, 0) * *this;
	return LongVector(res.x, res.y, res.z);
}

LongQuaternion LongQuaternion::operator* (LongQuaternion other) {
	return LongQuaternion(
		x*other.w + w * other.x - z * other.y + y * other.z,
		y*other.w + z * other.x + w * other.y - x * other.z,
		z*other.w - y * other.x + x * other.y + w * other.z,
		w*other.w - x * other.x - y * other.y - z * other.z
	);
}

LongQuaternion LongQuaternion::get_inverse() {
	return LongQuaternion(x, y, z, -w);
}

glm::quat LongQuaternion::to_float_quaternion() {
	return glm::quat(x, y, z, w);
}

LongVectorType longvec_magnitude(LongVector v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

LongVectorType longvec_sqrmagnitude(LongVector v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

LongVectorType longvec_dot(LongVector l, LongVector r) {
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

LongVector longvec_cross(LongVector a, LongVector b) {
	return LongVector(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

LongVectorType longvec_angle(LongVector a, LongVector b) {
	return longvec_dot(a, b) / (longvec_magnitude(a) * longvec_magnitude(b));
}

LongVector project_vector(LongVector v, LongVector projection) {
	return projection * (longvec_dot(v, projection) / longvec_sqrmagnitude(projection));
}

LongVector project2plane(LongVector v, LongVector plane_normal) {
	return v - project_vector(v, plane_normal);
}

LongMatrix4x4::LongMatrix4x4(
	LongVectorType p00, LongVectorType p01, LongVectorType p02, LongVectorType p03,
	LongVectorType p10, LongVectorType p11, LongVectorType p12, LongVectorType p13,
	LongVectorType p20, LongVectorType p21, LongVectorType p22, LongVectorType p23,
	LongVectorType p30, LongVectorType p31, LongVectorType p32, LongVectorType p33
) {
	a00 = p00; a01 = p01; a02 = p02; a03 = p03;
	a10 = p10; a11 = p11; a12 = p12; a13 = p13;
	a20 = p20; a21 = p21; a22 = p22; a23 = p23;
	a30 = p30; a31 = p31; a32 = p32; a33 = p33;
}

LongMatrix4x4::LongMatrix4x4(glm::mat4 m) {
	LongMatrix4x4(
		m[0].x, m[1].x, m[2].x, m[3].x,
		m[0].y, m[1].y, m[2].y, m[3].y,
		m[0].z, m[1].z, m[2].z, m[3].z,
		m[0].w, m[1].w, m[2].w, m[3].w
	);
}

LongMatrix4x4::LongMatrix4x4() {
	LongMatrix4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

glm::mat4 LongMatrix4x4::to_float_mat() {
	return glm::mat4(
		a00, a01, a02, a03,
		a10, a11, a12, a13,
		a20, a21, a22, a23,
		a30, a31, a32, a33
	);
}

LongMatrix4x4 LongMatrix4x4::operator*(LongVectorType t) {
	return LongMatrix4x4(
		a00 * t, a01 * t, a02 * t, a03 * t,
		a10 * t, a11 * t, a12 * t, a13 * t,
		a20 * t, a21 * t, a22 * t, a23 * t,
		a30 * t, a31 * t, a32 * t, a33 * t
	);
}

LongMatrix4x4 LongMatrix4x4::operator*(LongMatrix4x4 m) {
	return LongMatrix4x4(
		a00 * m.a00, a01 * m.a10, a02 * m.a20, a03 * m.a30,
		a10 * m.a01, a11 * m.a11, a12 * m.a21, a13 * m.a31,
		a20 * m.a02, a21 * m.a12, a22 * m.a22, a23 * m.a32,
		a30 * m.a03, a31 * m.a13, a32 * m.a23, a33 * m.a33
	);
}

LongMatrix4x4 LongMatrix4x4::operator/(LongMatrix4x4 m) {
	return *this * inverse(m);
}

LongMatrix4x4 LongMatrix4x4::operator+(LongMatrix4x4 m) {
	return LongMatrix4x4(
		a00 + m.a00, a01 + m.a01, a02 + m.a02, a03 + m.a03,
		a10 + m.a10, a11 + m.a11, a12 + m.a12, a13 + m.a13,
		a20 + m.a20, a21 + m.a21, a22 + m.a22, a23 + m.a23,
		a30 + m.a30, a31 + m.a31, a32 + m.a32, a33 + m.a33
	);
}

LongMatrix4x4 LongMatrix4x4::operator-(LongMatrix4x4 m) {
	return LongMatrix4x4(
		a00 - m.a00, a01 - m.a01, a02 - m.a02, a03 - m.a03,
		a10 - m.a10, a11 - m.a11, a12 - m.a12, a13 - m.a13,
		a20 - m.a20, a21 - m.a21, a22 - m.a22, a23 - m.a23,
		a30 - m.a30, a31 - m.a31, a32 - m.a32, a33 - m.a33
	);
}

// TBD
LongVectorType determinant(LongMatrix4x4 m) {
	return 0;
}

// TBD
LongMatrix4x4 inverse(LongMatrix4x4 m) {
	return m;
}


LongMatrix4x4 interpolate_matrix(LongMatrix4x4 a, LongMatrix4x4 b, LongVectorType t) {
	return a * t + b * (1 - t);
}


void disect_transform_matrix(glm::mat4 transform, glm::vec3* translation, glm::vec3* scale, glm::quat* rotation) {
	*translation = glm::vec3(transform[3]);
	*scale = glm::vec3(
		glm::length(transform[0]),
		glm::length(transform[1]),
		glm::length(transform[2])
	);
	glm::mat3 rotation_matrix = glm::mat3(
		glm::vec3(transform[0]) / scale->x,
		glm::vec3(transform[1]) / scale->y,
		glm::vec3(transform[2]) / scale->z
	);
	*rotation = glm::quat_cast(rotation_matrix);
}

glm::mat4 interpolate_matrix(glm::mat4 a, glm::mat4 b, float t) {
	// cutoff
	return t < 0.5 ? a : b;
	// Interpolate values
	/* return glm::mat4(
		a[0] * t + b[0] * (t - 1),
		a[1] * t + b[1] * (t - 1),
		a[2] * t + b[2] * (t - 1),
		a[3] * t + b[3] * (t - 1)
	);*/
	glm::vec3 translation_a, translation_b;
	glm::vec3 scale_a, scale_b;
	glm::quat rotation_a, rotation_b;
	disect_transform_matrix(a, &translation_a, &scale_a, &rotation_a);
	disect_transform_matrix(b, &translation_b, &scale_b, &rotation_b);

	glm::vec3 translation = translation_a * t + translation_b * (t-1);
	glm::vec3 scale = scale_b * t + scale_b * (t-1);
	glm::quat rotation = glm::lerp(rotation_a, rotation_b, t);
	return glm::translate(glm::scale(glm::mat4_cast(rotation), scale), translation);
}
