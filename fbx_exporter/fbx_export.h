#ifndef FBX_EXPORT
#define FBX_EXPORT

#ifdef __cplusplus
extern "C" {
#endif

unsigned short fbx_export(const char* _fbxFilepath, const char* _meshFilepath, const char* _materialFilepath, const char* _animationFilepath, const float _scale, const unsigned char _propertyMask, const unsigned short _noFailMask);

#ifdef __cplusplus
}
#endif


#endif