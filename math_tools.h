#ifndef MATH_TOOLS_H
#define MATH_TOOLS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <math.h>

typedef double LongVectorType;

typedef unsigned int pixel;

const int base_line_thickness = 3;
const glm::vec3 base_color = glm::vec3(0, 1, 0);

const double pi = 3.14159265358979;
const double pi_halfs = pi / 2;
const double pi_quart = pi / 4;
const double G = 6.67e-11;

struct Screenpos {
	pixel x;
	pixel y;

	Screenpos();
	Screenpos(int _x, int _y);
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
	glm::vec3 to_float_vec();

	LongVector operator+ (LongVector);
	LongVector operator* (LongVectorType);
};

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

#endif // !MATH_TOOLS_H