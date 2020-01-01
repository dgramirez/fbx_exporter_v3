# fbx_exporter_v3
This is my third version of this exporter. The purpose of this repository is to be able to get data from "FBX" files. This is the plans I have for this repository:

### 1.) Mesh Exporting [Done]
This will export the FBX mesh information as a file extension ".msh", in the format:
* Vertex:
	* [vec4] Position (pos)
	* [vec2] UV (uv)
	* [vec4] Normals (nrm)
	* [vec4] Tangents (tan)
	* [vec4] Joint Index\*
	* [vec4] Joint Weights\*\*  
(\*: This only exists if you include Animation. This vec4 is based on integer)  
(\*\*: This only exists if you include Animation. This vec4 is based on float/double)

### 2.) Material Exporting [In Progress]
This is currently in planning phase. The idea is to ensure I include as much necessary properties for these materials. Here is what I want to support in regards to this:
* Lambert (Diffuse, Emissive)
* Phong (Specular)
* PBR (Base Color, Roughness, Metallic)
* Common (Normal, Ambient Occlusion, Opacity)

### 3.) Animation Exporting [Future]
After doing Material Exporting (Which shouldn't take too long), I wish to add Animation Support. The Vertex Structure will be altered if animation export is on. When I get here, i will be more thurough on how this will be implemented. 

## Questions:
#### Q: HELP! I don't see the .msh files!
Did you do the following:
* have the "libfbxsdk.dll" in the same directory as the executable? If not, then the dll are in "./fbx_exporter/vendor/fbxsdk/lib".
	* Copy the Debug one if using Debug Configuration
	* Copy the Release one if using Release Configuration

#### Q: Why not export Binormals?  
The binormal can be obtained by doing a cross product using the normal and tangent.
Example:
```
vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
// re-orthogonalize T with respect to N
T = normalize(T - dot(T, N) * N);
// then retrieve perpendicular vector B with the cross product of T and N
vec3 B = cross(N, T);

mat3 TBN = mat3(T, B, N);
```
ref: https://learnopengl.com/Advanced-Lighting/Normal-Mapping @ "One last thing" title.
#### Q: Wait... Isn't that the Bitangent???
Thats correct. However, I wish to be consistent within FBX's terms to avoid confusion. They use "Binormals" within their function names such as **GetElementBinormalCount** and **GetElementBinormal**


#### Q: What data type is used when exporting these mesh data?
By default, the vector data types are floats.  

#### Q: Can I export using "double" Data Type?
If you download the source code, you can compile it to use doubles by flipping the "PRECISION_DOUBLE"(define.h) define to 1. This will trigger a different set of defines (using the same name) to export in doubles. Of course, flipping "PRECISION_DOUBLE" to 0 will export it back to floats.
