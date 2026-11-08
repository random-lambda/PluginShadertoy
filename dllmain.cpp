#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files

#ifdef _WIN32
	#include <windows.h>
#endif

#include <imgui/imgui.h>
#include "Shadertoy.h"

#ifdef WIN32
# define FEXPORT __declspec(dllexport)
#else
# define FEXPORT __attribute__((visibility("default")))
#endif

namespace st {
    bool Generate(const std::string&, const std::string&);
    const std::string& LastImportError();
}
extern "C" {
    // Small C interface for local automation and regression tests.
    FEXPORT int ShadertoyImportProject(const char* id, const char* path) {
        try { return st::Generate(id ? id : "", path ? path : "") ? 1 : 0; }
        catch (...) { return 0; }
    }
    FEXPORT const char* ShadertoyImportLastError() {
        return st::LastImportError().c_str();
    }

	FEXPORT st::Shadertoy* CreatePlugin() {
		return new st::Shadertoy();
	}
	FEXPORT void DestroyPlugin(st::Shadertoy* ptr) {
		delete ptr;
	}
	FEXPORT int GetPluginAPIVersion() {
		return 3;
	}
	FEXPORT int GetPluginVersion() {
		return 2;
	}
	FEXPORT const char* GetPluginName() {
		return "Shadertoy";
	}
}

#ifdef _WIN32
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif