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

glm::vec3 LongVector::to_float_vec() {
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
		angle *= pi_ / 180;
	}
	x = axis.x * sin(angle / 2),
		y = axis.y * sin(angle / 2),
		z = axis.z * sin(angle / 2),
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
