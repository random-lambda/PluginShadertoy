# PluginShadertoy
SHADERed plugin that simplifies the process of porting your Shadertoy projects.

![Screenshot](./screen.gif)

## How to build
Clone the project:
```bash
git clone https://github.com/dfranx/PluginShadertoy.git
git submodule update --init
```

### Linux
1. Install libcurl development files (version 7.85 or later).

2. Build:
```bash
cmake .
make
```

### Windows
1. Install libcurl through your favourite package manager (I recommend vcpkg)
2. Run cmake-gui and set CMAKE_TOOLCHAIN_FILE variable
3. Press Configure and then Generate if no errors occured
4. Open the .sln and build the project!

## How to use
This plugin requires at least SHADERed v1.3.5.

Copy the .dll/.so file to `plugins/ShadertoyImport` folder in your SHADERed's installation directory

After you start SHADERed, click on `File -> Import Shadertoy project`. Enter Shadertoy URL that contains
the ID & choose a path where you want to save SHADERed project. Press `Save`.

## TODO
- cubemaps
- audio shaders


## Maintained Linux build

This fork fixes a reproducible crash when SHADERed and the original plugin
export incompatible cpp-httplib C++ symbols. The transport uses system libcurl,
internal C++ symbols are hidden, and only plugin entry points and a small C
import API are exported. Original conversion semantics remain largely unchanged;
this is not a claim of full Shadertoy compatibility.

Dependencies: CMake >= 3.13, a C++11 compiler, libcurl development files >= 7.85,
and the pinned submodules. OpenSSL 1.1 is no longer a direct plugin dependency.
On Linux:

```sh
git submodule update --init
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --parallel 4
ctest --test-dir build --output-on-failure
```

With SHADERed closed, back up its existing plugin, then copy
`build/bin/plugin.so` into `plugins/ShadertoyImport/`. Retain the MIT license.
Set `SHADERTOY_API_KEY` in the environment before starting SHADERed. The source
build ships without a key; keep real credentials outside Git and command history.
The old compile-time key remains a compatibility fallback, but is discouraged.

The importer reports missing keys, HTTPS failures, HTTP status failures, and
missing shader payloads separately. HTTPS verifies certificates using libcurl's
normal trust settings, respects proxy environment variables, and has bounded
connection/request timeouts. Texture download failures mark the project incomplete
instead of silently creating an empty texture. A failed import can leave partial
project files; use an empty output directory and inspect it before reusing it.

For local automation, `ShadertoyImportProject(const char* id, const char* directory)`
returns 1 on success or 0 on failure. `ShadertoyImportLastError()` returns a
thread-local diagnostic, valid until the next import on that thread. Call these
through the plugin's C ABI, not its private C++ functions.

### Validation and limits

A conflicting host export reproduced the original Request destructor collision
(exit 86). The regression test loads the real rebuilt plugin into such a host,
then checks missing-key handling and transport failure through a deliberately
refused loopback proxy. It performs no external requests or project writes.

Local smoke testing uses SHADERed 1.5.6 on x86_64 Manjaro with Intel Mesa OpenGL
4.6. Source building and regression checks pass. In the actual editor, submitting the
same URL/path displays the missing-key diagnostic without crashing; local shader
rendering still works. A live successful import of
`l3cfW4` remains unverified because no user API key has been configured. Public
API visibility and API credentials are still required even after the crash fix.
ARM64/TX2 builds, cubemaps, audio, video, and complete uniform compatibility are
not validated by this change.
