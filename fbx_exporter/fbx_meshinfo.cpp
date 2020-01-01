#include "fbx_meshinfo.h"
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>

#define CHECK_FAIL(x, y) {if (x & y) return x;}

//This is Boost's Hash Combine Function! Credit to them!
static void hash_combine(size_t& seed, const size_t& v)
{
	//V is already a hash value from GLM's hash algorithm
	seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace {
	const int REF_DIRECT = 0;
	const int REF_INDEX = 1;
	const int REF_INDEX_TO_DIRECT = 2;

	const int MAP_CONTROL_POINT = 1;
	const int MAP_POLYGON_VERTEX = 2;

	const unsigned char MASK_UV_USE			 =  0x1;
	const unsigned char MASK_UV_EXPAND		 =  0x2;
	const unsigned char MASK_NORMAL_USE		 =  0x4;
	const unsigned char MASK_NORMAL_EXPAND	 =  0x8;
	const unsigned char MASK_TANGENT_USE	 = 0x10;
	const unsigned char MASK_TANGENT_EXPAND	 = 0x20;
	const unsigned char MASK_BINORMAL_USE	 = 0x40;
	const unsigned char MASK_BINORMAL_EXPAND = 0x80;
	unsigned char use_mask;
}
namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(const Vertex& vertex) const {
			size_t seed = 0;
			hash_combine(seed, hash<VEC_TYPE_2>()(vertex.uv));
			hash_combine(seed, hash<VEC_TYPE_4>()(vertex.pos));
			hash_combine(seed, hash<VEC_TYPE_4>()(vertex.nrm));
			hash_combine(seed, hash<VEC_TYPE_4>()(vertex.tan));
			return seed;
		}
	};
}

unsigned short get_polygon_vertex(FbxMesh* _fbxMesh, int& _outCount, int** _outIndex)
{
	//Try-Catch Block: For Failures
	try
	{
		//Get Index Count from Mesh
		_outCount = _fbxMesh->GetPolygonVertexCount();

		//Setup Indices Array for both Mesh and output file
		*_outIndex = _fbxMesh->GetPolygonVertices();
	}
	catch (...)
	{
		//It has failed. OR the result bit with failure flag
		return F_INDEX;
	}

	return S_SUCCESS;
}
unsigned short get_control_points(FbxMesh* _fbxMesh, int& _outCount, FbxVector4** _outVertex)
{
	//Try-Catch Block: For Failures
	try
	{
		//Get Index Count from Mesh
		_outCount = _fbxMesh->GetControlPointsCount();

		//Setup Indices Array for both Mesh and output file
		*_outVertex = _fbxMesh->GetControlPoints();
	}
	catch (...)
	{
		//It has failed. OR the result bit with failure flag
		return F_VERTEX;
	}

	return S_SUCCESS;
}
unsigned short get_uv(FbxMesh* _fbxMesh, FbxArray<FbxVector2>& _outUVs)
{
	//Get the count for UV
	int count_uv = _fbxMesh->GetElementUVCount(); 
	if (!count_uv) return W_UV;

	//Get Geometry Element for UV
	auto* element_uv = _fbxMesh->GetElementUV();
	use_mask |= MASK_UV_USE;

	//Check Map Mode.
	auto map_mode = element_uv->GetMappingMode();
	if (!(map_mode == MAP_POLYGON_VERTEX || map_mode == MAP_CONTROL_POINT))
		return W_UV;

	//Check Reference Mode
	auto ref_mode = element_uv->GetReferenceMode();
	switch (ref_mode)
	{
	case REF_DIRECT:
		_outUVs.Resize(element_uv->GetDirectArray().GetCount());
		break;
	case REF_INDEX:
		{
			FbxStringList list;
			_fbxMesh->GetUVSetNames(list);
			if (list.GetCount())
				return _fbxMesh->GetPolygonVertexUVs(list[0], _outUVs) ? S_SUCCESS : W_UV;
		}
		return W_UV;
	case REF_INDEX_TO_DIRECT:
		_outUVs.Resize(element_uv->GetIndexArray().GetCount());
		break;
	default:
		return W_UV;
	}

	if (map_mode & MAP_CONTROL_POINT)
	{
		use_mask |= MASK_UV_EXPAND;
		if (ref_mode == REF_DIRECT) {
			for (int i = 0; i < _outUVs.GetCount(); ++i)
				_outUVs.SetAt(i,element_uv->GetDirectArray().GetAt(i));
			return S_SUCCESS;
		}
		else {
			for (int i = 0; i < _outUVs.GetCount(); ++i)
				_outUVs.SetAt(i, element_uv->GetDirectArray().GetAt(element_uv->GetIndexArray().GetAt(i)));
			return S_SUCCESS;
		}
	}
	else {
		int poly_count = _fbxMesh->GetPolygonCount(), pv_index = 0;
		if (ref_mode == REF_DIRECT) {
			for (int poly_idx = 0; poly_idx < poly_count; ++poly_idx) {
				int poly_size = _fbxMesh->GetPolygonSize(poly_idx);
				for (int i = 0; i < poly_size; ++i) {
					if (pv_index < _outUVs.GetCount()) {
						_outUVs.SetAt(pv_index, element_uv->GetDirectArray().GetAt(element_uv->GetIndexArray().GetAt(pv_index)));
						++pv_index;
					}
				}
			}
			return S_SUCCESS;
		}
		else
		{
			for (int poly_idx = 0; poly_idx < poly_count; ++poly_idx) {
				int poly_size = _fbxMesh->GetPolygonSize(poly_idx);
				for (int i = 0; i < poly_size; ++i) {
					if (pv_index < _outUVs.GetCount()) {
						_outUVs.SetAt(pv_index, element_uv->GetDirectArray().GetAt(element_uv->GetIndexArray().GetAt(pv_index)));
						++pv_index;
					}
				}
			}
			return S_SUCCESS;
		}
	}
	return W_UV;
}
unsigned short get_normals(FbxMesh* _fbxMesh, FbxArray<FbxVector4>& _outNormals)
{
	bool use = _fbxMesh->GetPolygonVertexNormals(_outNormals) ? S_SUCCESS : W_NORMAL;
	if (use)
		use_mask |= MASK_NORMAL_USE;
	return use;

	#if 0
	//Get the count for Normals
	int count_normal = _fbxMesh->GetElementNormalCount();
	if (!count_normal) return W_NORMAL;

	//Get Geometry Element for Normals
	auto* element_normal = _fbxMesh->GetElementNormal();
	use_mask |= MASK_NORMAL_USE;

	//Check Map Mode.
	auto map_mode = element_normal->GetMappingMode();
	if (!(map_mode == MAP_CONTROL_POINT || map_mode == MAP_POLYGON_VERTEX))
		return W_NORMAL;

	//Check Reference Mode
	auto ref_mode = element_normal->GetReferenceMode();
	switch (ref_mode)
	{
	case REF_DIRECT:
		_outNormals.Resize(element_normal->GetDirectArray().GetCount());
		break;
	case REF_INDEX:
		return _fbxMesh->GetPolygonVertexNormals(_outNormals) ? S_SUCCESS : W_NORMAL;
	case REF_INDEX_TO_DIRECT:
		_outNormals.Resize(element_normal->GetIndexArray().GetCount());
		break;
	default:
		return W_TANGENT;
	}

	if (map_mode & MAP_CONTROL_POINT)
	{
		use_mask |= MASK_NORMAL_EXPAND;
		if (ref_mode == REF_DIRECT) {
			for (int i = 0; i < _outNormals.GetCount(); ++i)
				_outNormals.SetAt(i, element_normal->GetDirectArray().GetAt(i));
			return S_SUCCESS;
		}
		else {
			for (int i = 0; i < _outNormals.GetCount(); ++i)
				_outNormals.SetAt(i, element_normal->GetDirectArray().GetAt(element_normal->GetIndexArray().GetAt(i)));
			return S_SUCCESS;
		}
	}
	else {
		int poly_count = _fbxMesh->GetPolygonCount(), pv_index = 0;
		if (ref_mode == REF_DIRECT) {
			for (int poly_idx = 0; poly_idx < poly_count; ++poly_idx) {
				int poly_size = _fbxMesh->GetPolygonSize(poly_idx);
				for (int i = 0; i < poly_size; ++i) {
					if (pv_index < _outNormals.GetCount()) {
						_outNormals.SetAt(pv_index, element_normal->GetDirectArray().GetAt(element_normal->GetIndexArray().GetAt(pv_index)));
						++pv_index;
					}
				}
			}
			return S_SUCCESS;
		}
		else
		{
			for (int poly_idx = 0; poly_idx < poly_count; ++poly_idx) {
				int poly_size = _fbxMesh->GetPolygonSize(poly_idx);
				for (int i = 0; i < poly_size; ++i) {
					if (pv_index < _outNormals.GetCount()) {
						_outNormals.SetAt(pv_index, element_normal->GetDirectArray().GetAt(element_normal->GetIndexArray().GetAt(pv_index)));
						++pv_index;
					}
				}
			}
			return S_SUCCESS;
		}
	}
	return W_NORMAL;
	#endif //The other way of getting normals

}
unsigned short get_tangent(FbxMesh* _fbxMesh, FbxArray<FbxVector4>& _outTangents)
{
	//Get the count for Tangent
	int count_tangent = _fbxMesh->GetElementTangentCount();
	if (!count_tangent) return W_TANGENT;

	//Get Geometry Element for Tangent
	auto* element_tangent = _fbxMesh->GetElementTangent();
	use_mask |= MASK_TANGENT_USE;

	//Check Map Mode.
	auto map_mode = element_tangent->GetMappingMode();
	if (!(map_mode == MAP_POLYGON_VERTEX || map_mode == MAP_CONTROL_POINT))
		return W_TANGENT;

	//Check Reference Mode
	auto ref_mode = element_tangent->GetReferenceMode();
	switch (ref_mode)
	{
	case REF_DIRECT:
		_outTangents.Resize(element_tangent->GetDirectArray().GetCount());
		break;
	case REF_INDEX_TO_DIRECT:
		_outTangents.Resize(element_tangent->GetIndexArray().GetCount());
		break;
	default:
		return W_TANGENT;
	}

	if (map_mode & MAP_CONTROL_POINT)
	{
		use_mask |= MASK_TANGENT_EXPAND;
		if (ref_mode == REF_DIRECT) {
			for (int i = 0; i < _outTangents.GetCount(); ++i)
				_outTangents.SetAt(i, element_tangent->GetDirectArray().GetAt(i));
			return S_SUCCESS;
		}
		else {
			for (int i = 0; i < _outTangents.GetCount(); ++i)
				_outTangents.SetAt(i, element_tangent->GetDirectArray().GetAt(element_tangent->GetIndexArray().GetAt(i)));
			return S_SUCCESS;
		}
	}
	else {
		int poly_count = _fbxMesh->GetPolygonCount(), pv_index = 0;
		if (ref_mode == REF_DIRECT) {
			for (int poly_idx = 0; poly_idx < poly_count; ++poly_idx) {
				int poly_size = _fbxMesh->GetPolygonSize(poly_idx);
				for (int i = 0; i < poly_size; ++i) {
					if (pv_index < _outTangents.GetCount()) {
						_outTangents.SetAt(pv_index, element_tangent->GetDirectArray().GetAt(element_tangent->GetIndexArray().GetAt(pv_index)));
						++pv_index;
					}
				}
			}
			return S_SUCCESS;
		}
		else
		{
			for (int poly_idx = 0; poly_idx < poly_count; ++poly_idx) {
				int poly_size = _fbxMesh->GetPolygonSize(poly_idx);
				for (int i = 0; i < poly_size; ++i) {
					if (pv_index < _outTangents.GetCount()) {
						_outTangents.SetAt(pv_index, element_tangent->GetDirectArray().GetAt(element_tangent->GetIndexArray().GetAt(pv_index)));
						++pv_index;
					}
				}
			}
			return S_SUCCESS;
		}
	}
	return W_TANGENT;
}

void set_fbxvec4_data(VEC_TYPE_4* data, FbxDouble* mData)
{
	for (int i = 0; i < VEC_TYPE_4::length(); ++i)
		data->operator[](i) = static_cast<DATA_TYPE>(mData[i]);
}
void set_fbxvec4_default(VEC_TYPE_4* data, const DATA_TYPE& n)
{
	for (int i = 0; i < VEC_TYPE_4::length(); ++i)
		data->operator[](i) = n;
}
void set_string_append(const char* _filepath, const int& _iteration, char** _outFilepath)
{
	//Setup Buffer
	char* buffer = nullptr;
	size_t len = static_cast<size_t>(strlen(_filepath));

	//If First time (0), simply copy over and return
	if (!_iteration)
	{
		buffer = new char[len + 1];
		strcpy_s(buffer, len + 1, _filepath);
		*_outFilepath = buffer;
		return;
	}

	//Setup Iteration Number
	size_t digits = static_cast<size_t>(log(_iteration));
	char* it_num = new char[digits + 1ull];
	_itoa_s(_iteration, it_num, digits, 10);

	//Copy Filename and It. Number into Buffer
	buffer = new char[len + digits + 1];
	strcpy_s(buffer, len + 1, _filepath);
	strcat_s(buffer, len + digits + 1, it_num);

	//Set Buffer to output and cleanup
	*_outFilepath = buffer;
	delete[] it_num;
}
void vertex_flip_v(const int& _countCP, Vertex** _vertex)
{
	Vertex* vertex = *_vertex;
	for (int i = 0; i < _countCP; ++i)
		vertex[i].uv.y = static_cast<DATA_TYPE>(1) - vertex[i].uv.y;
}
void vertex_negate_y(const int& _countPV, int** _index, const int& _countCP, Vertex** _vertex)
{
	//Negate Y
	Vertex* vertex = *_vertex;
	for (int i = 0; i < _countCP; ++i)
		vertex[i].pos.y = -vertex[i].pos.y;

	//Change Triangle Order
	int* index = *_index;
	for (int i = 0; i < _countPV; i += 3)
		index[i] ^= index[i + 1], index[i + 1] ^= index[i], index[i] ^= index[i + 1];
}
void vertex_compactify(const int& _countPV, int** _index, int& _countCP, Vertex** _vertex)
{
	Vertex* vertex = *_vertex;

	//Setup Buffer Variables
	std::unordered_map<Vertex, int> unique_vertices;
	unique_vertices.reserve(_countPV);
	int* new_index = new int[_countPV];
	_countCP = 0;

	//Compactify
	for (int i = 0; i < _countPV; ++i)
	{
		if (unique_vertices.count(vertex[i]))
			new_index[i] = unique_vertices[vertex[i]];
		else
		{
			unique_vertices[vertex[i]] = _countCP;
			new_index[i] = _countCP++;
		}
	}

	//Copy Vertex
	Vertex* new_vertex = new Vertex[_countCP];
	for (auto v_key : unique_vertices)
		memcpy_s(&new_vertex[v_key.second], sizeof(Vertex), &v_key.first, sizeof(Vertex));

	//Set New Vertex and Index
	delete[] *_vertex;
	delete[] *_index;
	*_vertex = new_vertex;
	*_index = new_index;
}

unsigned short set_vertex(FbxVector4* _vertex, FbxArray<FbxVector2>& _uv, FbxArray<FbxVector4>& _normal, FbxArray<FbxVector4>& _tangent, int& _outCountPV, int** _outIndex, int& _outCountCP, Vertex** _outVertex)
{
	//Create the new Vertex
	Vertex* exp_vertex = new Vertex[_outCountPV];
	int* exp_index = new int[_outCountPV];
	int* _index = *_outIndex;

	//Loop based on Control Points
	for (int i = 0; i < _outCountPV; ++i)
	{
		//Position
		const int pos_idx = _index[i];
		set_fbxvec4_data(&exp_vertex[i].pos, &_vertex[pos_idx].mData[0]);

		//Normal
		if (use_mask & MASK_NORMAL_USE)
		{
			const int nrm_idx = (use_mask & MASK_NORMAL_EXPAND) ? _index[i] : i;
			set_fbxvec4_data(&exp_vertex[i].nrm, &_normal[nrm_idx].mData[0]);
		}
		else
			set_fbxvec4_default(&exp_vertex[i].nrm, 0);

		//UV
		if (use_mask & MASK_UV_USE)
		{
			const int uv_idx = (use_mask & MASK_UV_EXPAND) ? _index[i] : i;
			exp_vertex[i].uv[0] = static_cast<DATA_TYPE>(_uv[uv_idx].mData[0]);
			exp_vertex[i].uv[1] = static_cast<DATA_TYPE>(_uv[uv_idx].mData[1]);
		}
		else
		{
			exp_vertex[i].uv[0] = 0;
			exp_vertex[i].uv[0] = 0;
		}

		//Tangent
		if (use_mask & MASK_TANGENT_USE)
		{
			const int tan_idx = (use_mask & MASK_TANGENT_EXPAND) ? _index[i] : i;
			set_fbxvec4_data(&exp_vertex[i].tan, &_tangent[tan_idx].mData[0]);
		}
		else
			set_fbxvec4_default(&exp_vertex[i].tan, 0);

		//Index
		exp_index[i] = i;
	}

	_outCountCP = _outCountPV;
	*_outVertex = exp_vertex;
	*_outIndex = exp_index;
	return 0;
}
unsigned short setup_vertex(FbxMesh* _fbxMesh, const unsigned short& _failMask, int& _outCountPV, int** _outIndex, int& _outCountCP, Vertex** _outVertex)
{
	//Setup masks
	unsigned short fails = 0;
	use_mask = 0;

	//Set Polygon Vertices
	fails |= get_polygon_vertex(_fbxMesh, _outCountPV, _outIndex);
	CHECK_FAIL(fails, _failMask);
	
	//Set Control Points
	FbxVector4* control_point = nullptr;
	fails |= get_control_points(_fbxMesh, _outCountCP, &control_point);
	CHECK_FAIL(fails, _failMask);

	//Get Normals
	FbxArray<FbxVector4> normal;
	fails |= get_normals(_fbxMesh, normal);
	CHECK_FAIL(fails, _failMask);

	//Get UVs
	FbxArray<FbxVector2> uv;
	fails |= get_uv(_fbxMesh, uv);
	CHECK_FAIL(fails, _failMask);

	//Get Tangents
	FbxArray<FbxVector4> tangent;
	fails |= get_tangent(_fbxMesh, tangent);
	CHECK_FAIL(fails, _failMask);

	//Fill in Vertex
	set_vertex(control_point, uv, normal, tangent, _outCountPV, _outIndex, _outCountCP, _outVertex);

	return 0;
}
unsigned short export_mesh(const char* _filepath, const int& _iteration, const int& _countPV, int* _index, int& _countCP, Vertex* _vertex)
{
	//Compactify
	vertex_compactify(_countPV, &_index, _countCP, &_vertex);

	//Flip V
	vertex_flip_v(_countCP, &_vertex);

	//Negate Y
	vertex_negate_y(_countPV, &_index, _countCP, &_vertex);

	//Export
	FILE* file = 0;
	char* filepath = nullptr;
	set_string_append(_filepath, _iteration, &filepath);
	
	fopen_s(&file, filepath, "wb");
	if (!file)
		throw "File ain't open!";

	//Write Index
	fwrite(&_countPV, sizeof(int), 1, file);
	fwrite(_index, sizeof(int), _countPV, file);

	//Write Mesh
	fwrite(&_countCP, sizeof(int), 1, file);
	fwrite(_vertex, sizeof(Vertex), _countCP, file);

	fclose(file);

#ifdef _DEBUG
	fopen_s(&file, filepath, "rb");
	if (!file)
		throw "File ain't open!";

	//Read Index
	int dbg_countPV = 0;
	fread_s(&dbg_countPV, sizeof(int), sizeof(int), 1, file);

	int* dbg_index = new int[dbg_countPV];
	fread_s(dbg_index, sizeof(int) * dbg_countPV, sizeof(int), dbg_countPV, file);

	//Read Vertex
	int dbg_countCP = 0;
	fread_s(&dbg_countCP, sizeof(int), sizeof(int), 1, file);

	Vertex* dbg_vertex = new Vertex[dbg_countCP];
	fread_s(dbg_vertex, sizeof(Vertex) * dbg_countCP, sizeof(Vertex), dbg_countCP, file);

	//Close File
	fclose(file);

	//A "Not Fool Proof" Way of checking file alignment
	if (
		dbg_countPV != _countPV ||
		dbg_countCP != _countCP ||
		dbg_index[_countPV - 1] != _index[_countPV - 1] ||
		dbg_vertex[_countCP - 1].pos != _vertex[_countCP - 1].pos ||
		dbg_vertex[_countCP - 1].uv  != _vertex[_countCP - 1].uv  ||
		dbg_vertex[_countCP - 1].nrm != _vertex[_countCP - 1].nrm ||
		dbg_vertex[_countCP - 1].tan != _vertex[_countCP - 1].tan
		)
		throw "You done goof!";

	__debugbreak();

	delete[] dbg_index;
	delete[] dbg_vertex;
#endif //Import Test

	return 0;
}
