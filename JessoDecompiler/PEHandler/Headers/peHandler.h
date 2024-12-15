#pragma once
# define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char readCodeSection(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection);

#ifdef __cplusplus
}
#endif