#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#if defined(_MSC_VER)
#if defined(OOPETRIS_LIBRARY_C_WRAPPER_TYPE) && OOPETRIS_LIBRARY_C_WRAPPER_TYPE == 0 
#if defined(OOPETRIS_LIBRARY_C_WRAPPER_EXPORT)
#define OOPETRIS_C_WRAPPER_EXPORTED __declspec(dllexport)
#else
#define OOPETRIS_C_WRAPPER_EXPORTED __declspec(dllimport)
#endif
#else
#define OOPETRIS_C_WRAPPER_EXPORTED
#endif
#else
#define OOPETRIS_C_WRAPPER_EXPORTED
#endif


#ifdef __cplusplus
}
#endif
