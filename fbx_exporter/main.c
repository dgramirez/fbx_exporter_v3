#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "defines.h"
#include "fbx_export.h"

//Error Helper Function
void show_error(const char* display_error);
void show_error_arg(const char* display_error, const char* argument);
void check_error(const errno_t e, const char* display_error);

//Other Helper Functions
errno_t set_output_path(const char* _inputFilepath, const char* _fileExtension, char **_outFilepath);
int check_combo_all(const char *argument, unsigned char *combo_check);
void print_variales(const char *mesh_file, const char *mat_file, const char *anm_file, float scale, unsigned char prop_mask, unsigned short fail_mask);
void print_failure(unsigned short code);

int main(_In_ int argc, _In_reads_(argc) _Pre_z_ char** argv, _In_z_ char** envp)
{
	//Error Check: No Command Line Parameter
	if (argc < 2)
	{
		show_error("Need FBX File Name!");
		__debugbreak();
		return 0;
	}

	//Preparation Variables
	char* o_mesh		= NULL;	//Default: [Filepath].msh
	char* o_material	= NULL;	//Default: [Filepath].mat
	char* o_animation	= NULL;	//Default: [Filepath].anm
	
	unsigned char combo_check = 0;		//[Not Property]: check for type of combo (o = 1, m = 2, a = 4)
	unsigned char property_mask = 0;	//Default = 0 
	unsigned short no_fail_mask = 0;	//Default = All Failpoints	
	float scale = 1.0f;					//Default: 1.0f

	//Loop through all flags:
	unsigned int chk = 0;
	for (int i = 2; i < argc; ++i) {

		//Combo Check
		chk = check_combo_all(argv[i], &combo_check);
		if (chk)
		{
			errno_t e = 0;

			if (chk & 1)
			{
				if (++i == argc || argv[i][0] == '-')
				{
					e = set_output_path(argv[1], ".msh", &o_mesh);
					check_error(e, "Failed to set Mesh Filepath");

					e = set_output_path(argv[1], ".mat", &o_material);
					check_error(e, "Failed to set Material Filepath");

					e = set_output_path(argv[1], ".anm", &o_animation);
					check_error(e, "Failed to set Animation Filepath");
					--i;
				}
				else
				{
					e = set_output_path(argv[i], ".msh", &o_mesh);
					check_error(e, "Failed to set Mesh Filepath");

					e = set_output_path(argv[i], ".mat", &o_material);
					check_error(e, "Failed to set Material Filepath");

					e = set_output_path(argv[i], ".anm", &o_animation);
					check_error(e, "Failed to set Animation Filepath");
				}
			}
			else
			{
				errno_t e = 0;
				if (combo_check == 0x3) //om, mo
				{
					if (++i == argc || argv[i][0] == '-')
					{
						e = set_output_path(argv[1], ".msh", &o_mesh);
						check_error(e, "Failed to set Mesh Filepath");

						e = set_output_path(argv[1], ".mat", &o_material);
						check_error(e, "Failed to set Material Filepath");
						--i;
					}
					else
					{
						e = set_output_path(argv[i], ".msh", &o_mesh);
						check_error(e, "Failed to set Mesh Filepath");

						e = set_output_path(argv[i], ".mat", &o_material);
						check_error(e, "Failed to set Material Filepath");
					}
				}
				else if (combo_check == 0x5) //oa, ao
				{
					if (++i == argc || argv[i][0] == '-')
					{
						e = set_output_path(argv[1], ".msh", &o_mesh);
						check_error(e, "Failed to set Mesh Filepath");

						e = set_output_path(argv[1], ".anm", &o_animation);
						check_error(e, "Failed to set Animation Filepath");
						--i;
					}
					else
					{
						e = set_output_path(argv[i], ".msh", &o_mesh);
						check_error(e, "Failed to set Mesh Filepath");

						e = set_output_path(argv[i], ".anm", &o_animation);
						check_error(e, "Failed to set Animation Filepath");
					}
				}
				else if (combo_check == 0x6) //ma, am
				{
					if (++i == argc || argv[i][0] == '-')
					{
						e = set_output_path(argv[1], ".msh", &o_mesh);
						check_error(e, "Failed to set Mesh Filepath");

						e = set_output_path(argv[1], ".mat", &o_material);
						check_error(e, "Failed to set Material Filepath");

						e = set_output_path(argv[1], ".anm", &o_animation);
						check_error(e, "Failed to set Animation Filepath");
						--i;
					}
					else
					{
						e = set_output_path(argv[i], ".msh", &o_mesh);
						check_error(e, "Failed to set Mesh Filepath");

						e = set_output_path(argv[i], ".mat", &o_material);
						check_error(e, "Failed to set Material Filepath");

						e = set_output_path(argv[i], ".anm", &o_animation);
						check_error(e, "Failed to set Animation Filepath");
					}
				}
			}

			continue;
		}

		//Mesh Filepath
		if (!strcmp(argv[i], "-o")) {
			errno_t e = 0;
			if (++i == argc || argv[i][0] == '-')
			{
				e = set_output_path(argv[1], ".msh", &o_mesh);
				--i;
			}
			else
				e = set_output_path(argv[i], NULL, &o_mesh);
			check_error(e, "Failed to set Mesh Filepath");
			continue;
		}

		//Material Filepath
		if (!strcmp(argv[i], "-m")) {
			errno_t e = 0;
			if (++i == argc || argv[i][0] == '-')
			{
				e = set_output_path(argv[1], ".mat", &o_material);
				--i;
			}
			else
				e = set_output_path(argv[i], NULL, &o_material);
			check_error(e, "Failed to set Filepath for Output");
			continue;
		}

		//Animation Filepath
		if (!strcmp(argv[i], "-a")) {
			errno_t e = 0;
			if (++i == argc || argv[i][0] == '-')
			{
				e = set_output_path(argv[1], ".anm", &o_animation);
				--i;
			}
			else
				e = set_output_path(argv[i], NULL, &o_animation);
			check_error(e, "Failed to set Filepath for Output");
			continue;
		}

		//Scale
		if (!strcmp(argv[i], "-s")) {
			if (++i == argc || argv[i][0] == '-') {
				show_error_arg("Invalid Scale Value: ", argv[i]);
				return 0;
			}
			scale = (float)atof(argv[i]);
			if (scale == 0.0f) {
				show_error_arg("Invalid Scale Value: ", argv[i]);
				return 0;
			}
			property_mask |= SCALE;
			continue;
		}

		//Flip V
		if (!strcmp(argv[i], "-v")) {
			property_mask |= FLIP_V;
			continue;
		}

		//Negate Y
		if (!strcmp(argv[i], "-y")) {
			property_mask |= NEGATE_Y;
			continue;
		}

		//Force Warnings
		if (!strcmp(argv[i], "-f")) {
			if (++i == argc || argv[i][0] == '-') {
				show_error_arg("Invalid Fail Mask Value: ", argv[i]);
				return 0;
			}
			no_fail_mask = atoi(argv[++i]);
			continue;
		}
		
		show_error_arg("Invalid Argument: ", argv[i]);
		return 0;
	}

	//Set Defaults
	if (!o_mesh || !strcmp(o_mesh, ""))
	{
		errno_t e = 0;
		e = set_output_path(argv[1], ".msh", &o_mesh);
		check_error(e, "Failed to set Mesh Filepath");
	}
	no_fail_mask |= F_FBXIMPORT | F_INDEX | F_VERTEX | F_EXPORTMESH | F_EXPORTMATERIAL | F_EXPORTANIMATION;

	unsigned short success = fbx_export(argv[1], o_mesh, o_material, o_animation, scale, property_mask, no_fail_mask);
	if (success & no_fail_mask)
		print_failure(success);

	free(o_mesh);
	free(o_material);
	free(o_animation);

	return 0;
}

//Error Functions
void show_error(const char* display_error)
{
	printf("fbx_exporter.exe [FBX Filepath]\n");
	printf("\tFlags:\n");
	printf("\t\t-o [Filepath]: Output Mesh File [Default: $FILEPATH.msh]\n");
	printf("\t\t-m [Filepath]: Create Material File [Default: $FILEPATH.mat]\n");
	printf("\t\t-a [Filepath]: Create Animation File [Default: $FILEPATH.anm]\n");
	printf("\t\t-[oma] [Filepath]: Create Mesh, Material and Animation File [reference -o -m -a for Default]\n");
	printf("\t\t-s [Float]: Scale of Model [Default: 1.0]\n");
	printf("\t\t-y: Negate the Y value (Change Up Direction). [Default: Off]\n");
	printf("\t\t-f [Integer]: Force Flags to ensure optional data is applied correctly [Default: 0xB007]\n");
	printf("\n\tError: %s\n\n", display_error);
}
void show_error_arg(const char* display_error, const char* argument)
{
	printf("fbx_exporter.exe [FBX Filepath]\n");
	printf("\tFlags:\n");
	printf("\t\t-o [Filepath]: Output Mesh File [Default: $FILEPATH.msh]\n");
	printf("\t\t-m [Filepath]: Create Material File [Default: $FILEPATH.mat]\n");
	printf("\t\t-a [Filepath]: Create Animation File [Default: $FILEPATH.anm]\n");
	printf("\t\t-[oma] [Filepath]: Create Mesh, Material and Animation File [reference -o -m -a for Default]\n");
	printf("\t\t-s [Float]: Scale of Model [Default: 1.0]\n");
	printf("\t\t-y: Negate the Y value (Change Up Direction). [Default: Off]\n");
	printf("\t\t-f [Integer]: Force Flags to ensure optional data is applied correctly [Default: 0xB007]\n");
	printf("\n\tError: %s %s\n\n", display_error, argument);
}
void check_error(const errno_t e, const char* display_error)
{
	if (e)
	{
		show_error(display_error);
		printf("Press the enter key to continue . . .");
		getchar();
		exit(0);
	}
}

//Helper Functions
errno_t set_output_path(const char* _inputFilepath, const char* _fileExtension, char** _outFilepath)
{
	//Error Checking: _outFilepath or _inputFilepath NULL
	if (!_outFilepath || !_inputFilepath)
		return 1;

	//Setup Variables
	errno_t error;
	char buffer[256];	//255 Filepath cap + 1 for delimiter
	
	//Copy over Input to Buffer
	unsigned short len_input = (unsigned short)strlen(_inputFilepath) + 1;
	error = strcpy_s(buffer, len_input, _inputFilepath);
	if (error)
		return error;

	//Copy over File Extension
	if (_fileExtension && strcmp(_fileExtension, ""))
	{
		unsigned short len_ext = (unsigned short)strlen(_fileExtension) + len_input;
		error = strcat_s(buffer, len_ext, _fileExtension);
		if (error)
			return error;
	}

	//Memory Allocate the return
	char *output = (char*)calloc(strlen(buffer) + 1, sizeof(char));
	if (!output)
		return 1;

	//Copy the string to output
	error = strcpy_s(output, strlen(buffer) + 1, buffer);
	if (error)
		return error;

	//Set the out to the new char
	*_outFilepath = output;

	//Return no error
	return 0;
}
int check_combo_all(const char* argument, unsigned char *combo_check)
{
	if (!strcmp(argument, "-oma") || !strcmp(argument, "-oam") ||
		!strcmp(argument, "-moa") || !strcmp(argument, "-mao") ||
		!strcmp(argument, "-aom") || !strcmp(argument, "-amo"))
	{
		return 1;
	}
	
	if (!strcmp(argument, "-om") || !strcmp(argument, "-mo"))
	{
		*combo_check = 0x3;
		return 2;
	}

	if (!strcmp(argument, "-oa") || !strcmp(argument, "-ao"))
	{
		*combo_check = 0x5;
		return 2;
	}

	if (!strcmp(argument, "-ma") || !strcmp(argument, "-am"))
	{
		*combo_check = 0x6;
		return 2;
	}

	*combo_check = 0;
	return 0;
}
void print_variales(const char* mesh_file, const char* mat_file, const char* anm_file, float scale, unsigned char prop_mask, unsigned short fail_mask)
{
	printf("Mesh Filepath: %s\n",		mesh_file);
	printf("Material Filepath: %s\n",	mat_file);
	printf("Animation Filepath: %s\n",	anm_file);
	printf("Scale: %f\n",				scale);
	printf("Flip V: %d\n",				prop_mask & 0x1);
	printf("Negate Y: %d\n",			prop_mask & 0x2);
	printf("Fail Mask: %d\n",			fail_mask);
}
void print_failure(unsigned short code)
{
	if (code & F_FBXIMPORT)
		printf("Failed to Import FBX Scene.\n");
	if (code & F_INDEX)
		printf("Failed to get Polygon Vertex from FBX file.\n");
	if (code & F_VERTEX)
		printf("Failed to get Control Points from FBX file.\n");
	if (code & W_UV)
		printf("Failed to get UV data from FBX file\n");
	if (code & W_NORMAL)
		printf("Failed to get Normal data from FBX file\n");
	if (code & W_TANGENT)
		printf("Failed to get Tangent data from FBX file.");
	if (code & W_MATERIAL)
		printf("Failed to get Material data from FBX file.");
	if (code & W_BINDPOSE)
		printf("Failed to get Bindpose data from FBX file.");
	if (code & W_JOINTS)
		printf("Failed to get Joint data from FBX file.");
	if (code & W_KEYFRAME)
		printf("Failed to get Keyframe data from FBX file.");
	if (code & W_JOINTIDXWEIGHTS)
		printf("Failed to get Joint Index and Joint Weight data from FBX file.");
	if (code & F_EXPORTMESH)
		printf("Failed to export Mesh data.");
	if (code & F_EXPORTMATERIAL)
		printf("Failed to export Material data");
	if (code & F_EXPORTANIMATION)
		printf("Failed to export Animation data");
	if (code & W_JOKE)
		printf("Why do Programmers get confused between Halloween and Christmas? Because Oct 31 = Dec 25");
}
