#include <dlfcn.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
// Reproduce SHADERed exporting a different cpp-httplib Request destructor.
extern "C" void host_destructor() asm("_ZN7httplib7RequestD1Ev");
extern "C" void host_destructor() { std::_Exit(86); }
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    void* plugin = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
    if (!plugin) { std::cerr << dlerror() << '\n'; return 3; }
    using Import = int(*)(const char*, const char*);
    using Error = const char*(*)();
    auto run = reinterpret_cast<Import>(dlsym(plugin, "ShadertoyImportProject"));
    auto error = reinterpret_cast<Error>(dlsym(plugin, "ShadertoyImportLastError"));
    if (!run || !error || !dlsym(plugin, "CreatePlugin")) return 4;
    unsetenv("SHADERTOY_API_KEY");
    if (run("l3cfW4", "/unused") != 0 || !std::strstr(error(), "SHADERTOY_API_KEY")) return 5;
    // Exercise the transport under a conflicting host symbol. Force a local,
    // immediately refused proxy, so this test never contacts Shadertoy.
    setenv("SHADERTOY_API_KEY", "test-placeholder", 1);
    setenv("https_proxy", "http://127.0.0.1:1", 1);
    setenv("NO_PROXY", "", 1); setenv("no_proxy", "", 1);
    if (run("l3cfW4", "/unused") != 0 || !std::strstr(error(), "HTTPS request failed")) return 6;
    if (dlsym(plugin, "_ZN7httplib7RequestD1Ev")) return 7;
    std::cout << "PASS: no host HTTP destructor interposition; missing-key and transport failures diagnosed\n";
    dlclose(plugin); return 0;
}
