#ifndef ARTERY_MACROS_H_3PHIGCXK
#define ARTERY_MACROS_H_3PHIGCXK

#if defined _WIN32 || defined __CYGWIN__
    #define STORYBOARD_DLL_IMPORT __declspec(dllimport)
    #define STORYBOARD_DLL_EXPORT __declspec(dllexport)
    #define STORYBOARD_DLL_LOCAL
#else
    #define STORYBOARD_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define STORYBOARD_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define STORYBOARD_DLL_LOCAL __attribute__ ((visibility ("hidden")))
#endif

// storyboard_EXPORTS is defined by CMake
#ifdef storyboard_EXPORTS
    #define STORYBOARD_API STORYBOARD_DLL_EXPORT
#else
    #define STORYBOARD_API STORYBOARD_DLL_IMPORT
#endif
#define STORYBOARD_LOCAL STORYBOARD_DLL_LOCAL

#endif /* ARTERY_MACROS_H_3PHIGCXK */
