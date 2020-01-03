#include "fbx_export.h"
#include "fbx_meshinfo.h"

unsigned short get_scene(const char *_meshFilepath, FbxScene **_outFbxScene);
unsigned short process_fbx(FbxNode *_fbxScene, const unsigned char _propertyMask, const float& _scale, const unsigned short &_failMask, const char* _meshFilepath);

unsigned short fbx_export(const char* _fbxFilepath, const char* _meshFilepath, const char* _materialFilepath, const char* _animationFilepath, const float _scale, const unsigned char _propertyMask, const unsigned short _noFailMask)
{
	//Get Scene
	FbxScene *fbxScene;
	unsigned short failure = get_scene(_fbxFilepath, &fbxScene);
	if (failure)
		return failure;

	process_fbx(fbxScene->GetRootNode(), _propertyMask, _scale, _noFailMask, _meshFilepath);
	return failure;
}

unsigned short get_scene(const char* _meshFilepath, FbxScene** _outFbxScene)
{
	//Initialize the SDK manager. This object handles all our memory management.
	FbxManager* fbx_manager = FbxManager::Create();

	//Create the IO settings object.
	FbxIOSettings* ios = FbxIOSettings::Create(fbx_manager, IOSROOT);
	fbx_manager->SetIOSettings(ios);

	//Create an importer using the SDK manager.
	FbxImporter* importer = FbxImporter::Create(fbx_manager, "");

	//Use the first argument as the filename for the importer.
	if (!importer->Initialize(_meshFilepath, -1, fbx_manager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
		return F_FBXIMPORT;
	}

	//Create a new scene so that it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(fbx_manager, "myScene");

	//Import the contents of the file into the scene.
	importer->Import(lScene);

	//The file is imported; so get rid of the importer.
	importer->Destroy();

	//Loop through all the nodes using the root node
	*_outFbxScene = lScene;

	//Return Success
	return 0;
}
unsigned short process_fbx(FbxNode *_fbxNode, const unsigned char _propertyMask, const float &_scale, const unsigned short& _failMask, const char* _meshFilepath)
{
	//Get the amount of children the node has
	unsigned int child_count = _fbxNode->GetChildCount();

	for (unsigned int i = 0; i < child_count; ++i)
	{
		FbxNode* child_node = _fbxNode->GetChild(i);
		FbxMesh* mesh = child_node->GetMesh();

		if (mesh != nullptr)
		{
			//Setup Mesh Vertex Data
			int count_pv = 0, count_cp = 0;
			int* polygon_vertex = nullptr;
			Vertex* control_point = nullptr;
			setup_vertex(mesh, _failMask, count_pv, &polygon_vertex, count_cp, &control_point);
			
			#if 0
			//Setup Material Data
			int count_material = 0;
			MaterialComponent** material = nullptr; //Double Pointer: 1st: Material Amount, 2: Component Amount.
			setup_materials(scene, count_material, &material);
			#endif //Material Loading
			
			#if 0
			//Setup Animation Data (WILL DO ANOTHER TIME)
			VertexAnm* control_point_anm = nullptr;
			Bindpose* bindpose = nullptr;
			Bindpose* bindpose_inv = nullptr;
			Clip* clip = nullptr;
//			setup_animation();
			#endif //Animation Loading

			//Export Mesh
			export_mesh(_meshFilepath, i, _propertyMask, _scale, count_pv, polygon_vertex, count_cp, control_point);

			#if 0
			//Export Mesh
			if (control_point_anm)
				export_mesh(_meshFilepath, i, _failMask, count_pv, polygon_vertex, count_cp, control_point_anm);
			else
				export_mesh(_meshFilepath, i, _failMask, count_pv, polygon_vertex, count_cp, control_point);

			//Export Material
			if (material)
			{
//				export_materials(count_material, material);
			}

			//Export Animation
			if (vertexanm)
			{
//				export_animation(bindpose, bindpose_inv, clip);
			}
			#endif //The Ideal

		}

		process_fbx(child_node, _propertyMask, _scale, _failMask, _meshFilepath);
	}

	return 0;
}
