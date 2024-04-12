#pragma once

// stlib
#include <fstream> // stdout, stderr..
#include <string>
#include <tuple>
#include <vector>

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>
#include <SDL_mixer.h>

// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>						 // vec2
#include <glm/ext/vector_int2.hpp> // ivec2
#include <glm/vec3.hpp>						 // vec3
#include <glm/mat3x3.hpp>					 // mat3
using namespace glm;

#include "ecs/ecs.hpp"

// Simple utility functions to avoid mistyping directory name
// audio_path("audio.ogg") -> data/audio/audio.ogg
// Get defintion of PROJECT_SOURCE_DIR from:
#include "../ext/project_path.hpp"
inline std::string data_path() { return std::string(PROJECT_SOURCE_DIR) + "data"; };
inline std::string shader_path(const std::string &name) { return std::string(PROJECT_SOURCE_DIR) + "/shaders/" + name; };
inline std::string textures_path(const std::string &name) { return data_path() + "/textures/" + std::string(name); };
inline std::string fonts_path(const std::string& name) { return data_path() + "/fonts/" + std::string(name); };
inline std::string audio_path(const std::string &name) { return data_path() + "/audio/" + std::string(name); };
inline std::string mesh_path(const std::string &name) { return data_path() + "/meshes/" + std::string(name); };
inline std::string sheets_path(const std::string &name) { return data_path() + "/sheets/" + std::string(name); };

// game artstyle was generated for 1024x512 window
const int window_width_px = 1920;
const int window_height_px = 1024;
// game window size both, width and height
const int game_window_size_px = 960;
// game window block size, width and height
const int game_window_block_size = 64;
const float aspect_ratio = (float)window_width_px / (float)window_height_px;

// Colors
const vec3 COLOR_WHITE = vec3(1.0f, 1.0f, 1.0f);
const vec3 COLOR_BLACK = vec3(0.0f, 0.0f, 0.0f);
const vec3 COLOR_RED = vec3(1.0f, 0.15f, 0.15f);
//const vec3 COLOR_GREEN = vec3(0.26f, 0.97f, 0.19f);
// 153, 229, 80
const vec3 COLOR_BRIGHT_GREEN = vec3(0.6f, 0.9f, 0.31f);
// 135, 229, 80 
const vec3 COLOR_GREEN = vec3(0.53f, 0.9f, 0.31f);
// 75, 105, 47
const vec3 COLOR_DARK_GREEN = vec3(0.29f, 0.41f, 0.18f);
// 217, 160, 102
const vec3 COLOR_GOLD = vec3(0.85f, 0.63f, 0.4f);
const vec3 COLOR_BLUE = vec3(0.5f, 0.75f, 1.0f);
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// The 'Transform' component handles transformations passed to the Vertex shader
// (similar to the gl Immediate mode equivalent, e.g., glTranslate()...)
// We recomment making all components non-copyable by derving from ComponentNonCopyable
struct Transform
{
	mat3 mat = {{1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}}; // start with the identity
	void scale(vec2 scale);
	void rotate(float radians);
	void translate(vec2 offset);
};

bool gl_has_errors();
