#ifndef FBX_MESHINFO_H
#define FBX_MESHINFO_H
#include "defines.h"
#include "fbxsdk.h"

/*
	Note:
		- Polygon Vertex = Indices (0, 1, 2, 0, 2, 3)
		- Control Points = Vertex Position ( {-0.5, 0.5}, {-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5} ) [up = +y]
*/
unsigned short setup_vertex(FbxMesh *_fbxMesh, const unsigned short &_failMask, int &_outCountPV, int** _outIndex, int &_outCountCP, Vertex **_outVertex);
unsigned short export_mesh(const char* _filepath, const int& _iteration, const int& _propertyMask, const float &_scale, const int& _countPV, int* _index, int& _countCP, Vertex* _vertex);

#endif