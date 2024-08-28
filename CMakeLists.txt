cmake_minimum_required(VERSION 3.10)
project(SensiaAravis)

# Habilitar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Politicas de CMake - compatible con versiones anteriores
if(POLICY CMP0990)
  cmake_policy(SET CMP0990 NEW)
endif()

# Mover CMakeUserPresets.json a la carpeta cmake/
if(EXISTS "${CMAKE_BINARY_DIR}/CMakeUserPresets.json")
    file(RENAME "${CMAKE_BINARY_DIR}/CMakeUserPresets.json" "${CMAKE_SOURCE_DIR}/cmake/CMakeUserPresets.json")
endif()

# Detectar el sistema operativo
if(WIN32)
    message(STATUS "Configurando para Windows")
    # Configuraciones específicas de Windows
    # Definir la macro _CRT_SECURE_NO_WARNINGS
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(UNIX)
    message(STATUS "Configurando para Unix/Linux")
    # Configuraciones específicas de Unix/Linux
endif()

# Añadir rutas de Conan para encontrar paquetes
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

# Buscar Aravis
find_path(aravis_INCLUDE_DIR
    NAMES arv.h
    PATHS ${aravis_PKGCONF_INCLUDE_DIRS} 
)
message("path: ${aravis_PKGCONF_INCLUDE_DIRS}")
message("include: ${INCLUDE_DIR}")

find_package(aravis REQUIRED)

add_executable(SensiaAravis
    src/main.cpp
    src/SensiaBridge.cpp  
)

# Configurar las rutas de inclusión y bibliotecas según el sistema operativo
if(WIN32)
    target_include_directories(SensiaAravis PUBLIC ${CONAN_INCLUDE_DIRS})
    target_link_libraries(SensiaAravis PUBLIC ${CONAN_LIBS} aravis::aravis)
elseif(UNIX)
    target_include_directories(SensiaAravis PUBLIC ${CONAN_INCLUDE_DIRS})
    target_link_libraries(SensiaAravis PUBLIC ${CONAN_LIBS} aravis::aravis)
endif()

message(STATUS "paso 5")

# Mensajes para verificar las rutas de inclusión y las bibliotecas
message("Include directories: ${CONAN_INCLUDE_DIRS}")
message("Libraries: ${CONAN_LIBS}")

# Incluir configuración de clang-tidy y clang-format desde un archivo en cmake/clang_setup.cmake
include(${CMAKE_SOURCE_DIR}/cmake/clang_setup.cmake)