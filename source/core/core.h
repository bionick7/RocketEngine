#pragma once

// provisory should be tied to settings
const double position_scale = 5e-10;
const double radius_enlarger = 1;

#include <string>
#include <regex>
#include <map>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <math.h>
#include <functional>
#include <memory>
#include <queue>
#include <set>
#include <thread>

#include <iostream>
#include <sstream>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <AL/al.h>
#include <AL/alc.h>

#include "wren.hpp"

#define TEXT(_str) settings->get_text_for(_str)
