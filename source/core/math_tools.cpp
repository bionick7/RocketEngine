#include "math_tools.h"
#include "debug.h"

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

double lerp(double a, double b, double t) {
	return a * t + b * (1 - t);
}

#ifndef LIN_ALG_PAINTOVER

// ======================
//		VECTOR
// ======================

#pragma region Vector
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

LongVector LongVector::operator-() {
	return *this * -1;
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
#pragma endregion

// ======================
//		QUATERNION
// ======================

#pragma region Quaternion
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
	if (abs(longvec_magnitude(axis) - 1) > .01) {
		std::cout << longvec_magnitude(axis) << std::endl;
		double inv_m = 1 / sqrt(x * x + y * y + z * z + w * w);
		x *= inv_m;
		y *= inv_m;
		z *= inv_m;
		w *= inv_m;
	}
	//std::cout << x << "i + " << y << "j + " << z << "k + " << w << std::endl;
}

LongQuaternion::LongQuaternion(glm::quat q) {
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
}

LongQuaternion::LongQuaternion() {
	x = 0;
	y = 0;
	z = 0;
	w = 1;
}

LongQuaternion LongQuaternion::normalized() {
	double inv_m = 1 / sqrt(x * x + y * y + z * z + w * w);
	double
		x2 = x * inv_m,
		y2 = y * inv_m,
		z2 = z * inv_m,
		w2 = w * inv_m;
	return LongQuaternion(x2, y2, z2, w2);
}

LongQuaternion::~LongQuaternion() {

}

LongVector LongQuaternion::operator* (LongVector vec) {
	LongQuaternion res = *this * LongQuaternion(vec.x, vec.y, vec.z, 0) * get_conjugate();
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

LongQuaternion LongQuaternion::get_conjugate() {
	return LongQuaternion(-x,- y, -z, w);
}

glm::quat LongQuaternion::to_float_quaternion() {
	return glm::quat(w, x, y, z);
}

LongQuaternion interpolate_rotation(LongQuaternion from, LongQuaternion to, LongVectorType t) {
	//LongQuaternion d = to * from.get_conjugate();
	return LongQuaternion(
		(1 - t) * from.x + t * to.x,
		(1 - t) * from.y + t * to.y,
		(1 - t) * from.z + t * to.z,
		(1 - t) * from.w + t * to.w
	);
}

LongQuaternion rotation_matrix_to_quaternion(LongMatrix3x3 m) {
	return LongQuaternion(glm::quat_cast(m.to_float_mat()));  // Loss of precision. Can do internally
}

#pragma endregion

// ======================
//		MATRIX 3x3
// ======================

#pragma region Matrix3x3
LongMatrix3x3::LongMatrix3x3(
	LongVectorType p00, LongVectorType p01, LongVectorType p02,
	LongVectorType p10, LongVectorType p11, LongVectorType p12,
	LongVectorType p20, LongVectorType p21, LongVectorType p22
) {
	a00 = p00; a01 = p01; a02 = p02;
	a10 = p10; a11 = p11; a12 = p12;
	a20 = p20; a21 = p21; a22 = p22;
}

LongMatrix3x3::LongMatrix3x3(glm::mat3 m) :
	LongMatrix3x3(
		m[0].x, m[1].x, m[2].x,
		m[0].y, m[1].y, m[2].y,
		m[0].z, m[1].z, m[2].z
	) {}

LongMatrix3x3::LongMatrix3x3(LongVector v1, LongVector v2, LongVector v3) :
	LongMatrix3x3(
		v1.x, v2.x, v3.x,
		v1.y, v2.y, v3.y,
		v1.z, v2.z, v3.z
	) {}

LongMatrix3x3::LongMatrix3x3() :
	LongMatrix3x3(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	) {}

glm::mat3 LongMatrix3x3::to_float_mat() {
	return glm::mat3(
		a00, a10, a20,
		a01, a11, a21,
		a02, a12, a22
	);
}

LongMatrix3x3 LongMatrix3x3::operator*(LongVectorType t) {
	return LongMatrix3x3(
		a00 * t, a01 * t, a02 * t,
		a10 * t, a11 * t, a12 * t,
		a20 * t, a21 * t, a22 * t
	);
}

LongVector LongMatrix3x3::operator*(LongVector v) {
	return LongVector(
		a00 * v.x + a01 * v.y + a02 * v.z,
		a10 * v.x + a11 * v.y + a12 * v.z,
		a20 * v.x + a21 * v.y + a22 * v.z
	);
}

LongMatrix3x3 LongMatrix3x3::operator*(LongMatrix3x3 m) {
	return LongMatrix3x3(
		a00 * m.a00 + a01 * m.a10 + a02 * m.a20,
		a00 * m.a01 + a01 * m.a11 + a02 * m.a21,
		a00 * m.a02 + a01 * m.a12 + a02 * m.a22,

		a10 * m.a00 + a11 * m.a10 + a12 * m.a20,
		a10 * m.a01 + a11 * m.a11 + a12 * m.a21,
		a10 * m.a02 + a11 * m.a12 + a12 * m.a22,

		a20 * m.a00 + a21 * m.a10 + a22 * m.a20,
		a20 * m.a01 + a21 * m.a11 + a22 * m.a21,
		a20 * m.a02 + a21 * m.a12 + a22 * m.a22
	);
}

LongMatrix3x3 LongMatrix3x3::operator/(LongMatrix3x3 m) {
	return *this * inverse(m);
}

LongMatrix3x3 LongMatrix3x3::operator+(LongMatrix3x3 m) {
	return LongMatrix3x3(
		a00 + m.a00, a01 + m.a01, a02 + m.a02,
		a10 + m.a10, a11 + m.a11, a12 + m.a12,
		a20 + m.a20, a21 + m.a21, a22 + m.a22
	);
}

LongMatrix3x3 LongMatrix3x3::operator-(LongMatrix3x3 m) {
	return LongMatrix3x3(
		a00 - m.a00, a01 - m.a01, a02 - m.a02,
		a10 - m.a10, a11 - m.a11, a12 - m.a12,
		a20 - m.a20, a21 - m.a21, a22 - m.a22
	);
}

LongVector LongMatrix3x3::get_row(int row) {
	switch (row) {
	case 0: return LongVector(a00, a01, a02);
	case 1: return LongVector(a10, a11, a12);
	case 2: return LongVector(a20, a21, a22);
	}
	return LongVector();
}

LongVector LongMatrix3x3::get_column(int col) {
	switch (col) {
	case 0: return LongVector(a00, a10, a20);
	case 1: return LongVector(a01, a11, a21);
	case 2: return LongVector(a02, a12, a22);
	}
	return LongVector();
}

void LongMatrix3x3::set(int row, int col, LongVectorType a) {
	switch (row * 3 + col) {
#define SET_ID_CASE(i, j) case (i*3 + j): a##i##j = a; break;
		SET_ID_CASE(0, 0)
		SET_ID_CASE(0, 1)
		SET_ID_CASE(0, 2)
		SET_ID_CASE(1, 0)
		SET_ID_CASE(1, 1)
		SET_ID_CASE(1, 2)
		SET_ID_CASE(2, 0)
		SET_ID_CASE(2, 1)
		SET_ID_CASE(2, 2)
#undef SET_ID_CASE
	}
}

bool LongMatrix3x3::is_rigid_transform() {
	const LongVectorType epsilon = 10e-10;
	LongVector 
		c1 = get_column(0),
		c2 = get_column(1),
		c3 = get_column(2);
	return 
		std::abs(longvec_dot(c1, c1) - 1) < epsilon &&
		std::abs(longvec_dot(c2, c2) - 1) < epsilon &&
		std::abs(longvec_dot(c3, c3) - 1) < epsilon &&
		std::abs(longvec_dot(c1, c2)	) < epsilon && 
		std::abs(longvec_dot(c2, c3)	) < epsilon &&
		std::abs(longvec_dot(c1, c3)	) < epsilon;
}

bool LongMatrix3x3::decompose(LongVector* scale, LongQuaternion* rotation) {
	LongVector
		c1 = get_column(0),
		c2 = get_column(1),
		c3 = get_column(2);

	*scale = LongVector(
		longvec_magnitude(c1),
		longvec_magnitude(c2),
		longvec_magnitude(c3)
	);

	if (determinant(*this) < 0) {  // Mirror transform
		scale->x = scale->x * -1;
	}

	LongMatrix3x3 rotation_matrix = LongMatrix3x3(
		c1 / scale->x,
		c2 / scale->y,
		c3 / scale->z
	);

	*rotation = rotation_matrix_to_quaternion(rotation_matrix);
	return true;  // TODO: Condition checking
}

// TBD ??
LongVectorType determinant(LongMatrix3x3 m) {
	return m.a00 * m.a11 * m.a22
		+ m.a10 * m.a21 * m.a02
		+ m.a20 * m.a01 * m.a12
		- m.a20 * m.a11 * m.a02
		- m.a10 * m.a01 * m.a22
		- m.a00 * m.a21 * m.a12;
}

LongMatrix3x3 transpose(LongMatrix3x3 m) {
	return LongMatrix3x3(
		m.a00, m.a10, m.a20,
		m.a01, m.a11, m.a21,
		m.a02, m.a12, m.a22
	);
}

LongMatrix3x3 inverse(LongMatrix3x3 m) {
	// hack, only worls for RT transform matrices
	//return LongMatrix3x3(m.a00, m.a10, 0, m.a01, m.a11, 0, 0, 0, 1) * LongMatrix3x3(0, 0, -m.a02, 0, 0, -m.a12, 0, 0, 1);
	LongVectorType det = determinant(m);
	if (det == 0) {
		return LongMatrix3x3();
	}
	return LongMatrix3x3();
	//return transpose(m) * (1 / det);
}
#pragma endregion

//LongMatrix4x4 LongMatrix4x4::operator*(LongMatrix4x4 m) {
//	return LongMatrix4x4(
//		a00 * m.a00 + a01 * m.a10 + a02 * m.a20 + a03 * m.a30,
//			a00 * m.a01 + a01 * m.a11 + a02 * m.a21 + a03 * m.a31,
//				a00 * m.a02 + a01 * m.a12 + a02 * m.a22 + a03 * m.a32,
//					a00 * m.a03 + a01 * m.a13 + a02 * m.a23 + a03 * m.a33,
//		a10 * m.a00 + a11 * m.a10 + a12 * m.a20 + a13 * m.a30,
//			a10 * m.a01 + a11 * m.a11 + a12 * m.a21 + a13 * m.a31,
//				a10 * m.a02 + a11 * m.a12 + a12 * m.a22 + a13 * m.a32,
//					a10 * m.a03 + a11 * m.a13 + a12 * m.a23 + a13 * m.a33,
//		a20 * m.a00 + a21 * m.a10 + a22 * m.a20 + a23 * m.a30,
//			a20 * m.a01 + a21 * m.a11 + a22 * m.a21 + a23 * m.a31,
//				a20 * m.a02 + a21 * m.a12 + a22 * m.a22 + a23 * m.a32,
//					a20 * m.a03 + a21 * m.a13 + a22 * m.a23 + a23 * m.a33,
//		a30 * m.a00 + a31 * m.a10 + a32 * m.a20 + a33 * m.a30,
//			a30 * m.a01 + a31 * m.a11 + a32 * m.a21 + a33 * m.a31,
//				a30 * m.a02 + a31 * m.a12 + a32 * m.a22 + a33 * m.a32,
//					a30 * m.a03 + a31 * m.a13 + a32 * m.a23 + a33 * m.a33
//	);
//}

// ======================
//		MATRIX 4x4
// ======================
#pragma region Matrix4x4
LongMatrix4x4::Row::Row(LongVectorType _x, LongVectorType _y, LongVectorType _z, LongVectorType _w) {
	x = _x; y = _y; z = _z; w = _w;
}

LongVectorType LongMatrix4x4::Row::operator[] (int idx) {
	switch (idx) {
	case 0: return x;
	case 1: return y;
	case 2: return z;
	case 3: return w;
	}
	return 0;
}

LongVector LongMatrix4x4::Row::first() {
	return LongVector(x, y, z);
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

LongMatrix4x4::LongMatrix4x4(glm::mat4 m) :
	LongMatrix4x4(
		m[0].x, m[1].x, m[2].x, m[3].x,
		m[0].y, m[1].y, m[2].y, m[3].y,
		m[0].z, m[1].z, m[2].z, m[3].z,
		m[0].w, m[1].w, m[2].w, m[3].w
	) {}

LongMatrix4x4::LongMatrix4x4(LongMatrix3x3 m) :
	LongMatrix4x4(
		m.a00, m.a01, m.a02, 0,
		m.a10, m.a11, m.a12, 0,
		m.a20, m.a21, m.a22, 0,
		0, 0, 0, 1
	) {}


LongMatrix4x4::LongMatrix4x4() :
	LongMatrix4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	) {}


glm::mat4 LongMatrix4x4::to_float_mat() {
	return glm::mat4(
		a00, a10, a20, a30,
		a01, a11, a21, a31,
		a02, a12, a22, a32,
		a03, a13, a23, a33
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

LongVector LongMatrix4x4::mul(LongVector v, LongVectorType w) {
	return LongVector(
		a00 * v.x + a01 * v.y + a02 * v.z + a03 * w,
		a10 * v.x + a11 * v.y + a12 * v.z + a13 * w,
		a20 * v.x + a21 * v.y + a22 * v.z + a23 * w
	);
}

LongMatrix4x4 LongMatrix4x4::operator*(LongMatrix4x4 m) {
	return LongMatrix4x4(
		m.a00 * a00 + m.a01 * a10 + m.a02 * a20 + m.a03 * a30,
		m.a00 * a01 + m.a01 * a11 + m.a02 * a21 + m.a03 * a31,
		m.a00 * a02 + m.a01 * a12 + m.a02 * a22 + m.a03 * a32,
		m.a00 * a03 + m.a01 * a13 + m.a02 * a23 + m.a03 * a33,

		m.a10 * a00 + m.a11 * a10 + m.a12 * a20 + m.a13 * a30,
		m.a10 * a01 + m.a11 * a11 + m.a12 * a21 + m.a13 * a31,
		m.a10 * a02 + m.a11 * a12 + m.a12 * a22 + m.a13 * a32,
		m.a10 * a03 + m.a11 * a13 + m.a12 * a23 + m.a13 * a33,
		
		m.a20 * a00 + m.a21 * a10 + m.a22 * a20 + m.a23 * a30,
		m.a20 * a01 + m.a21 * a11 + m.a22 * a21 + m.a23 * a31,
		m.a20 * a02 + m.a21 * a12 + m.a22 * a22 + m.a23 * a32,
		m.a20 * a03 + m.a21 * a13 + m.a22 * a23 + m.a23 * a33,
		
		m.a30 * a00 + m.a31 * a10 + m.a32 * a20 + m.a33 * a30,
		m.a30 * a01 + m.a31 * a11 + m.a32 * a21 + m.a33 * a31,
		m.a30 * a02 + m.a31 * a12 + m.a32 * a22 + m.a33 * a32,
		m.a30 * a03 + m.a31 * a13 + m.a32 * a23 + m.a33 * a33
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

LongMatrix4x4::Row LongMatrix4x4::get_row(int row) {
	switch (row) {
	case 0: return Row(a00, a01, a02, a03);
	case 1: return Row(a10, a11, a12, a13);
	case 2: return Row(a20, a21, a22, a23);
	case 3: return Row(a30, a31, a32, a33); }
	return Row(0, 0, 0, 0);
}

LongMatrix4x4::Row LongMatrix4x4::get_column(int col) {
	switch (col) {
	case 0: return Row(a00, a10, a20, a30);
	case 1: return Row(a01, a11, a21, a31);
	case 2: return Row(a02, a12, a22, a32);
	case 3: return Row(a03, a13, a23, a33);	}
	return Row(0, 0, 0, 0);
}

void LongMatrix4x4::set(int row, int col, LongVectorType a) {
	switch (row*4 + col) {
#define SET_ID_CASE(i, j) case (i*4 + j): a##i##j = a; break;
	SET_ID_CASE(0, 0)
	SET_ID_CASE(0, 1)
	SET_ID_CASE(0, 2)
	SET_ID_CASE(0, 3)
	SET_ID_CASE(1, 0)
	SET_ID_CASE(1, 1)
	SET_ID_CASE(1, 2)
	SET_ID_CASE(1, 3)
	SET_ID_CASE(2, 0)
	SET_ID_CASE(2, 1)
	SET_ID_CASE(2, 2)
	SET_ID_CASE(2, 3)
	SET_ID_CASE(3, 0)
	SET_ID_CASE(3, 1)
	SET_ID_CASE(3, 2)
	SET_ID_CASE(3, 3)
#undef SET_ID_CASE
	}
}

LongMatrix3x3 LongMatrix4x4::get_base_matrix() {
	return LongMatrix3x3(a00, a01, a02, a10, a11, a12, a20, a21, a22);
}

bool LongMatrix4x4::is_rigid_transform() {
	return get_base_matrix().is_rigid_transform();
}

bool LongMatrix4x4::decompose(LongVector* scale, LongQuaternion* rotation, LongVector* translation) {
	// assumes transform o rotation o scale
	*translation = get_column(3).first();
	return get_base_matrix().decompose(scale, rotation);
}

LongVectorType quick_transform_determinant(LongMatrix4x4 m) {
	return determinant(m.get_base_matrix());
}

// TBD
LongVectorType determinant(LongMatrix4x4 m) {
	return 0;
}

// TBD
LongMatrix4x4 inverse(LongMatrix4x4 m) {
	return m;
}

LongMatrix4x4 transpose(LongMatrix4x4 m) {
	return LongMatrix4x4(
		m.a00, m.a10, m.a20, m.a30,
		m.a01, m.a11, m.a21, m.a31,
		m.a02, m.a12, m.a22, m.a32,
		m.a03, m.a13, m.a23, m.a33
	);
}

LongMatrix4x4 get_rotation_matrix(LongQuaternion q) {
	// TBD
	LongVectorType qw = q.w, qx = q.x, qy = q.y, qz = q.z;
	LongMatrix4x4 res;

	glm::tquat<double, glm::precision::highp> q2 = glm::tquat<double, glm::precision::highp>(qw, qx, qy, qz);

	glm::highp_dmat4 m = glm::toMat4(q.to_float_quaternion());

	res = LongMatrix4x4(m);

	// Let's worry about this later
	return res;

	//LongMatrix4x4 res = LongMatrix4x4(
	//	2*(qw*qw+qx*qx) - 1.0,	2 * (qx*qy-qw*qz),			2 * (qx*qz+qw*qy)		, 0,
	//	2*(qx*qy+qw*qz),		2 * (qw*qw+qy*qy) - 1.0,	2 * (qy*qz-qw*qx)		, 0,
	//	2*(qx*qz-qw*qy),		2 * (qy*qz+qw*qx),			2 * (qw*qw+qz*qz) - 1.0	, 0,
	//	0,						0,							0						, 1.0
	//);
	//LongMatrix4x4 res = LongMatrix4x4(
	//	1.0 - 2*(qy*qy+qz*qz),	2 * (qx*qy-qw*qz),			2 * (qx*qz+qw*qy)		, 0,
	//	2*(qx*qy+qw*qz),		1.0 - 2 * (qx*qx+qz*qz),	2 * (qy*qz-qw*qx)		, 0,
	//	2*(qx*qz-qw*qy),		2 * (qy*qz+qw*qx),			1.0 - 2 * (qx*qx+qy*qy)	, 0,
	//	0,						0,							0						, 1.0
	//);

	// Maybe this?

	//tmat3x3<T, P> Result(T(1));
	//T qxx(q.x * q.x);
	//T qyy(q.y * q.y);
	//T qzz(q.z * q.z);
	//T qxz(q.x * q.z);
	//T qxy(q.x * q.y);
	//T qyz(q.y * q.z);
	//T qwx(q.w * q.x);
	//T qwy(q.w * q.y);
	//T qwz(q.w * q.z);

	//Result[0][0] = 1 - 2 * (qyy + qzz);
	//Result[0][1] = 2 * (qxy + qwz);
	//Result[0][2] = 2 * (qxz - qwy);

	//Result[1][0] = 2 * (qxy - qwz);
	//Result[1][1] = 1 - 2 * (qxx + qzz);
	//Result[1][2] = 2 * (qyz + qwx);

	//Result[2][0] = 2 * (qxz + qwy);
	//Result[2][1] = 2 * (qyz - qwx);
	//Result[2][2] = 1 - 2 * (qxx + qyy);
	//return Result;

	return res;
}

LongMatrix4x4 get_translation_matrix(LongVector v) {
	return LongMatrix4x4(
		1, 0, 0, v.x,
		0, 1, 0, v.y,
		0, 0, 1, v.z,
		0, 0, 0, 1
	);
}

LongMatrix4x4 get_scale_matrix(LongVectorType d) {
	return LongMatrix4x4(
		d, 0, 0, 0,
		0, d, 0, 0,
		0, 0, d, 0,
		0, 0, 0, 1
	);
}

LongMatrix4x4 get_scale_matrix(LongVector d) {
	return LongMatrix4x4(
		d.x, 0, 0, 0,
		0, d.y, 0, 0,
		0, 0, d.z, 0,
		0, 0, 0.0, 1
	);
}
#pragma endregion

// From a to b on t
LongMatrix4x4 interpolate_matrix(LongMatrix4x4 a, LongMatrix4x4 b, LongVectorType t) {
	LongVector translation_a, translation_b;
	LongVector scale_a, scale_b;
	LongQuaternion rotation_a, rotation_b;
	a.decompose(&scale_a, &rotation_a, &translation_a);
	b.decompose(&scale_b, &rotation_b, &translation_b);

	// interpolate_vector can be a thing
	LongVector translation = translation_a * (1 - t) + translation_b * t;
	LongVector scale = scale_a * (1 - t) + scale_b * t;
	LongQuaternion rotation = interpolate_rotation(rotation_a, rotation_b, t);

	LongMatrix4x4 rot_matrix = get_rotation_matrix(rotation);
	LongMatrix4x4 scale_matrix = get_scale_matrix(scale);  // glm::scale(glm::mat4(1), scale);
	LongMatrix4x4 translate_matrix = get_translation_matrix(translation);

	return scale_matrix * rot_matrix * translate_matrix;
}

#endif // !LIN_ALG_PAINTOVER
