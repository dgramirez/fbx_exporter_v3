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
#### Q: Why not export Binormals?  
The binormal can be obtained by doing a cross product using the normal and tangent.
Example:
```
//Setup TBN
vec3 T = normalize(mat3(mvp.model_transposed_inversed) * tangent.xyz);
vec3 N = normalize(mat3(mvp.model_transposed_inversed) * normal.xyz);

T = normalize(T - dot(T,N) * N);

vec3 B = cross(N,T); //<--Right here is the binormal.

frag_TBN = mat3(T, B, N);
```
#### Q: What data type is used when exporting these mesh data?
By default, the vector data types are floats.  

#### Q: Can I export using "double" Data Type?
If you download the source code, you can compile it to use doubles by flipping the "PRECISION_DOUBLE"(define.h) define to 1. This will trigger a different set of defines (using the same name) to export in doubles. Of course, flipping "PRECISION_DOUBLE" to 0 will export it back to floats.
