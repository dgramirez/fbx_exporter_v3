#ifndef DEFINES
#define DEFINES

enum FAIL_MASK {
	S_SUCCESS = 0x0,
	F_FBXIMPORT = 0x1,
	F_INDEX = 0x2,
	F_VERTEX = 0x4,
	W_UV = 0x8,
	W_NORMAL = 0x10,
	W_TANGENT = 0x20,
	W_MATERIAL = 0x80,
	W_BINDPOSE = 0x100,
	W_JOINTS = 0x200,
	W_KEYFRAME = 0x400,
	W_JOINTIDXWEIGHTS = 0x800,
	F_EXPORTMESH = 0x1000,
	F_EXPORTMATERIAL = 0x2000,
	F_EXPORTANIMATION = 0x4000,
	W_JOKE = 0x8000
};
enum PROP_MASK {
	FLIP_V = 0x1,
	NEGATE_Y = 0x2,
};

#ifdef __cplusplus
#include "glm.hpp"

#define PRECISION_DOUBLE 0

#if PRECISION_DOUBLE
#define DATA_TYPE double
#define VEC_TYPE_4 glm::dvec4
#define VEC_TYPE_2 glm::dvec2
#define VEC_TYPE_I glm::ivec4
#define MAT_TYPE_4 glm::dmat4
#else
#define DATA_TYPE float
#define VEC_TYPE_4 glm::fvec4
#define VEC_TYPE_2 glm::fvec2
#define VEC_TYPE_I glm::ivec4
#define MAT_TYPE_4 glm::fmat4
#endif

struct Vertex {
	VEC_TYPE_4 pos;
	VEC_TYPE_2  uv;
	VEC_TYPE_4 nrm;
	VEC_TYPE_4 tan;
	bool operator==(const Vertex& o) const {
		return (pos == o.pos) && (uv == o.uv) && (nrm == o.nrm) && (tan == o.tan);
	}
};
struct VertexAnm {
	VEC_TYPE_4 pos;
	VEC_TYPE_2  uv;
	VEC_TYPE_4 nrm;
	VEC_TYPE_4 tan;
	VEC_TYPE_I jid;
	VEC_TYPE_4 jwt;
	bool operator==(const Vertex& o) const {
		return (pos == o.pos) && (uv == o.uv) && (nrm == o.nrm) && (tan == o.tan);
	}
};

struct MaterialComponent {
	VEC_TYPE_4 values; //This also includes the factor, at w
	const char* filepath;
};

//For Animation
struct Joints {
	int32_t parent;
	MAT_TYPE_4 pose;
};

//Keyframe & Clip Creation
struct Keyframe { double time = 0.0; Joints* joints; };
struct Clip { double duration = 0.0; Keyframe *frames; };
//Index-Weight Creation (Can be in fbx_exportAnm.cpp)
//struct Influence { unsigned int joint = 0; float weight = 0.0f; };
//using influence_set = std::array<Influence, MAX_INFLUENCE>;
//std::vector<influence_set> control_point_influences;
#endif


#endif