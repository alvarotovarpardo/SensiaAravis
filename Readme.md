## Sensia Bridge

mkdir build
cd build

# Parte Aravis WIN
C:\code3\proyectoSensiaBridge
conan install .. -g CMakeDeps -g CMakeToolchain --build=missing
cmake .. -DCMAKE_POLICY_DEFAULT_CMP0091="NEW" -DCMAKE_TOOLCHAIN_FILE="/generators/conan_toolchain.cmake" -G "Visual Studio 17 2022"
cmake --build . --config Release


# Parte Aravis Ubuntu
C:\code3\proyectoSensiaBridge
conan install .. -g CMakeDeps -g CMakeToolchain --build=missing
cmake .. -DCMAKE_TOOLCHAIN_FILE=./Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" 
cmake --build . --config Release

 