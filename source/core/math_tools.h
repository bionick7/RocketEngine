#pragma once
#include "core.h"

typedef double LongVectorType;

typedef short pixel;

const double pi_ = 3.14159265358979;
const double pi_halfs = pi_ / 2;
const double pi_quart = pi_ / 4;
const double G = 6.67e-11;

const double rad2deg = 180.0 / pi_;
const double deg2rad = pi_ / 180.0;

struct Screenpos {
	pixel x;
	pixel y;

	Screenpos();
	Screenpos(int _x, int _y);
};

struct PolarCoordinates {
	double argument;
	double modulus;
};

struct GLPos {
	float x;
	float y;

	GLPos();
	GLPos(float _x, float _y);
};

struct LongVector {
	LongVectorType x;
	LongVectorType y;
	LongVectorType z;

	LongVector(float, float, float);
	LongVector(LongVectorType, LongVectorType, LongVectorType);
	LongVector(glm::vec3);
	LongVector();
	~LongVector();

	LongVectorType distance_to(LongVector);
	glm::vec3 const to_float_vec();
	LongVector normalized();

	LongVector operator+ (LongVector);
	LongVector operator- (LongVector);
	LongVector operator* (LongVectorType);
	LongVector operator/ (LongVectorType);
};

LongVectorType longvec_dot(LongVector, LongVector);
LongVector longvec_cross(LongVector, LongVector);
LongVectorType longvec_magnitude(LongVector);
LongVectorType longvec_sqrmagnitude(LongVector);
LongVectorType longvec_angle(LongVector, LongVector);
LongVector project_vector(LongVector, LongVector);
LongVector project2plane(LongVector, LongVector);

struct LongQuaternion {
	double x, y, z, w;

	LongQuaternion(double, double, double, double);
	LongQuaternion(double, LongVector, bool);
	LongQuaternion();
	~LongQuaternion();

	LongVector operator* (LongVector);
	LongQuaternion operator* (LongQuaternion);

	LongQuaternion get_inverse();

	glm::quat to_float_quaternion();
};

struct LongMatrix4x4 {
	LongVectorType
		a00, a01, a02, a03,
		a10, a11, a12, a13,
		a20, a21, a22, a23,
		a30, a31, a32, a33;

	LongMatrix4x4(
		LongVectorType, LongVectorType, LongVectorType, LongVectorType,
		LongVectorType, LongVectorType, LongVectorType, LongVectorType,
		LongVectorType, LongVectorType, LongVectorType, LongVectorType,
		LongVectorType, LongVectorType, LongVectorType, LongVectorType
	);
	LongMatrix4x4(glm::mat4);
	LongMatrix4x4();

	glm::mat4 to_float_mat();

	LongMatrix4x4 operator*(LongVectorType);
	LongMatrix4x4 operator*(LongMatrix4x4);
	LongMatrix4x4 operator/(LongMatrix4x4);
	LongMatrix4x4 operator+(LongMatrix4x4);
	LongMatrix4x4 operator-(LongMatrix4x4);
};

LongVectorType determinant(LongMatrix4x4);
LongMatrix4x4 inverse(LongMatrix4x4);

//LongMatrix4x4 interpolate_matrix(LongMatrix4x4, LongMatrix4x4, LongVectorType);
glm::mat4 interpolate_matrix(glm::mat4, glm::mat4, float);
