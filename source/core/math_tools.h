#pragma once
#include "core.h"

//#define LIN_ALG_PAINTOVER

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

double lerp(double, double, double);

#ifdef LIN_ALG_PAINTOVER
typedef glm::highp_dmat4x4 LongMatrix4x4;
typedef glm::highp_vec4 LongVector;
typedef glm::highp_quat LongQuaternion;
#else

struct LongVector;
struct LongQuaternion;
struct LongMatrix3x3;
struct LongMatrix4x4;

// ======================
//		VECTOR
// ======================

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
	LongVector operator- ();
};

LongVectorType longvec_dot(LongVector, LongVector);
LongVector longvec_cross(LongVector, LongVector);
LongVectorType longvec_magnitude(LongVector);
LongVectorType longvec_sqrmagnitude(LongVector);
LongVectorType longvec_angle(LongVector, LongVector);
LongVector project_vector(LongVector, LongVector);
LongVector project2plane(LongVector, LongVector);

// ======================
//		QUATERNION
// ======================

struct LongQuaternion {
	double x, y, z, w;

	LongQuaternion(double, double, double, double);
	LongQuaternion(double, LongVector, bool);
	LongQuaternion(glm::quat);
	LongQuaternion();
	~LongQuaternion();

	LongVector operator* (LongVector);
	LongQuaternion operator* (LongQuaternion);

	LongQuaternion get_conjugate();
	LongQuaternion normalized();

	glm::quat to_float_quaternion();
};

LongQuaternion interpolate_rotation(LongQuaternion, LongQuaternion, LongVectorType);
LongQuaternion rotation_matrix_to_quaternion(LongMatrix3x3);

// ======================
//		MATRIX 3x3
// ======================

struct LongMatrix3x3 {
	LongVectorType
		a00, a01, a02,
		a10, a11, a12,
		a20, a21, a22;

	LongMatrix3x3(
		LongVectorType, LongVectorType, LongVectorType,
		LongVectorType, LongVectorType, LongVectorType,
		LongVectorType, LongVectorType, LongVectorType
	);

	LongMatrix3x3(glm::mat3);
	LongMatrix3x3(LongVector, LongVector, LongVector);
	LongMatrix3x3();

	glm::mat3 to_float_mat();

	LongMatrix3x3 operator*(LongVectorType);
	LongVector operator*(LongVector);
	LongMatrix3x3 operator*(LongMatrix3x3);
	LongMatrix3x3 operator/(LongMatrix3x3);
	LongMatrix3x3 operator+(LongMatrix3x3);
	LongMatrix3x3 operator-(LongMatrix3x3);

	LongVector get_row(int row);
	LongVector get_column(int col);
	void set(int, int, LongVectorType);

	bool is_rigid_transform();
	bool decompose(LongVector* scale, LongQuaternion* rotation);
};

LongVectorType determinant(LongMatrix3x3);
LongMatrix3x3 transpose(LongMatrix3x3);
LongMatrix3x3 inverse(LongMatrix3x3);

// ======================
//		MATRIX 4x4
// ======================

struct LongMatrix4x4 {
	LongVectorType
		a00, a01, a02, a03,
		a10, a11, a12, a13,
		a20, a21, a22, a23,
		a30, a31, a32, a33;


	struct Row {
		LongVectorType x, y, z, w;
		Row(LongVectorType _x, LongVectorType _y, LongVectorType _z, LongVectorType _w);
		LongVectorType operator[] (int idx);
		LongVector first();
	};

	LongMatrix4x4(
		LongVectorType, LongVectorType, LongVectorType, LongVectorType,
		LongVectorType, LongVectorType, LongVectorType, LongVectorType,
		LongVectorType, LongVectorType, LongVectorType, LongVectorType,
		LongVectorType, LongVectorType, LongVectorType, LongVectorType
	);
	LongMatrix4x4(glm::mat4);
	LongMatrix4x4(LongMatrix3x3);
	LongMatrix4x4();

	glm::mat4 to_float_mat();
	LongVector mul(LongVector, LongVectorType);

	LongMatrix4x4 operator*(LongVectorType);
	LongMatrix4x4 operator*(LongMatrix4x4);
	LongMatrix4x4 operator/(LongMatrix4x4);
	LongMatrix4x4 operator+(LongMatrix4x4);
	LongMatrix4x4 operator-(LongMatrix4x4);

	Row get_row(int row);
	Row get_column(int col);
	void set(int row, int col, LongVectorType a);

	LongMatrix3x3 get_base_matrix();
	bool is_rigid_transform();
	bool decompose(LongVector* scale, LongQuaternion* rotation, LongVector* translation);
};

LongVectorType determinant(LongMatrix4x4);
LongVectorType quick_transform_determinant(LongMatrix4x4);
LongMatrix4x4 transpose(LongMatrix4x4);
LongMatrix4x4 inverse(LongMatrix4x4);

LongMatrix4x4 interpolate_matrix(LongMatrix4x4, LongMatrix4x4, LongVectorType);

LongMatrix4x4 get_rotation_matrix(LongQuaternion);
LongMatrix4x4 get_translation_matrix(LongVector);
LongMatrix4x4 get_scale_matrix(LongVector);
LongMatrix4x4 get_scale_matrix(LongVectorType);

#endif // LIN_ALG_PAINTOVER
