cmake_minimum_required(VERSION 2.6)

# --------------------------------------------------------------------------------------

PROJECT(EasyWorld)

SET(EW_MAJOR_VERSION 0)
SET(EW_MINOR_VERSION 0)
SET(EW_PATCH_VERSION 1)
SET(EW_SOVERSION 1)

# --------------------------------------------------------------------------------------

MESSAGE(" -- src dir = ${EasyWorld_SOURCE_DIR}")

SET(EW_PROJECT_ROOT ${EasyWorld_SOURCE_DIR})

MESSAGE(" -- building headers ---")
execute_process(COMMAND "${EW_PROJECT_ROOT}/include/build_headers.sh"
		OUTPUT_FILE "/dev/null"
)

MESSAGE(" -- building wrappers ---")
execute_process(COMMAND "${EW_PROJECT_ROOT}/scripts/gen_libX11_symtab_entries.sh"
		OUTPUT_FILE "/dev/null"
)

execute_process(COMMAND "${EW_PROJECT_ROOT}/scripts/gen_libGL_symtab_entries.sh"
		OUTPUT_FILE "/dev/null"
)

execute_process(COMMAND "${EW_PROJECT_ROOT}/scripts/gen_libGLU_symtab_entries.sh"
		OUTPUT_FILE "/dev/null"
)

execute_process(COMMAND "${EW_PROJECT_ROOT}/scripts/gen_libGLX_symtab_entries.sh"
		OUTPUT_FILE "/dev/null"
)

execute_process(COMMAND "${EW_PROJECT_ROOT}/scripts/gen_libfreetype2_symtab_entries.sh"
		OUTPUT_FILE "/dev/null"
)


MESSAGE(" -- CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

MESSAGE(" -- Building on ${CMAKE_SYSTEM_NAME} -- ")

IF (CMAKE_SYSTEM_NAME MATCHES Linux)

ADD_DEFINITIONS(-D_LARGEFILE_SOURCE)
ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)

SET(LINUX_OS 1)
ENDIF (CMAKE_SYSTEM_NAME MATCHES Linux)


IF (CMAKE_SYSTEM_NAME MATCHES FreeBSD)

ADD_DEFINITIONS(-D_LARGEFILE_SOURCE)
ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)
ADD_DEFINITIONS(-D_POSIX_C_SOURCE=200809) # ??

SET(FREEBSD_OS 1)

#INCLUDE_DIRECTORIES(/usr/local/include)
#LINK_DIRECTORIES(/usr/local/lib)

ENDIF (CMAKE_SYSTEM_NAME MATCHES FreeBSD)

# todo : check non zero ${CMAKE_INSTALL_PREFIX}
SET(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
SET(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")

INCLUDE(CheckTypeSize)

CHECK_TYPE_SIZE("void *" SIZE_OF_VOID_PTR)

IF (${SIZE_OF_VOID_PTR}  EQUAL 4)
 SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEW_32_BITS")   # only used implementation
ENDIF (${SIZE_OF_VOID_PTR}  EQUAL 4)

IF (${SIZE_OF_VOID_PTR}  EQUAL 8)
 SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEW_64_BITS")   # only used implementation
ENDIF (${SIZE_OF_VOID_PTR}  EQUAL 8)


SET(EW_VERSION ${EW_MAJOR_VERSION}.${EW_MINOR_VERSION}.${EW_PATCH_VERSION})

# 3rd parts deps
SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${EW_PROJECT_ROOT}/cmake/CMakeModules")


INCLUDE(FindThreads)
INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCXXCompilerFlag)

INCLUDE(FindDoxygen)
INCLUDE(FindOpenGL)


IF(UNIX)
FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(X11 REQUIRED)

MESSAGE("X11_INCLUDE_DIR is set to '${X11_INCLUDE_DIR}'")


# for the moment
FIND_PACKAGE(FreeType REQUIRED)
IF(FREETYPE_FOUND)
 MESSAGE(" -- found freetype devel pkg")
ELSE(FREETYPE_FOUND)
 MESSAGE(FATAL_ERROR "must have freetype devel pkg")
ENDIF(FREETYPE_FOUND)

ADD_DEFINITIONS(-D_REENTRANT)

ENDIF(UNIX)


# common pkg
FIND_PACKAGE(OpenGL)


INCLUDE_DIRECTORIES(
${EW_PROJECT_ROOT}/include
${EW_PROJECT_ROOT}
)

# set default
SET(EW_STD_CPP 2003)

IF (UNIX)

# gcc c++ option
IF(CMAKE_COMPILER_IS_GNUCXX)


# check -std=c++-0x/c++1 options
CHECK_CXX_COMPILER_FLAG(-std=c++11 CXX_HAVE_OPT_stdcplusplus11)
IF (CXX_HAVE_OPT_stdcplusplus11)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  ADD_DEFINITIONS(-D_EW_STD_CPP=2011)
  SET(EW_STD_CPP 2011)

ELSE (CXX_HAVE_OPT_stdcplusplus11)

 CHECK_CXX_COMPILER_FLAG(-std=c++0x CXX_HAVE_OPT_stdcplusplus0x)
 IF (CXX_HAVE_OPT_stdcplusplus0x)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   ADD_DEFINITIONS(-D_EW_STD_CPP=2011)
   SET(EW_STD_CPP 2011)
 ENDIF(CXX_HAVE_OPT_stdcplusplus0x)

ENDIF(CXX_HAVE_OPT_stdcplusplus11)


# OPTIM
IF (${CMAKE_BUILD_TYPE} EQUAL "RELEASE")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer")
ENDIF()

# profiling using gprof
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

CHECK_CXX_COMPILER_FLAG(-Werror CXX_HAVE_OPT_Werror)
IF (CXX_HAVE_OPT_Werror)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
ENDIF(CXX_HAVE_OPT_Werror)

 CHECK_CXX_COMPILER_FLAG(-Wextra CXX_HAVE_OPT_Wextra)
 IF (CXX_HAVE_OPT_Wextra)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
 ENDIF(CXX_HAVE_OPT_Wextra)

 CHECK_CXX_COMPILER_FLAG(-Wall CXX_HAVE_OPT_Wall)
 IF (CXX_HAVE_OPT_Wall)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
 ENDIF(CXX_HAVE_OPT_Wall)

 CHECK_CXX_COMPILER_FLAG(-W CXX_HAVE_OPT_W)
 IF (CXX_HAVE_OPT_W)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W")
 ENDIF(CXX_HAVE_OPT_W)

 CHECK_CXX_COMPILER_FLAG(-Wno-unused-parameter CXX_HAVE_OPT_Wno_unused_parameter)
 IF (CXX_HAVE_OPT_Wno_unused_parameter)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
 ENDIF(CXX_HAVE_OPT_Wno_unused_parameter)

 CHECK_CXX_COMPILER_FLAG(-Wabi CXX_HAVE_OPT_Wabi)
 IF (CXX_HAVE_OPT_Wabi)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wabi")
 ENDIF(CXX_HAVE_OPT_Wabi)

# MUST allow private ctor/dtor for noncopyable_object
# CHECK_CXX_COMPILER_FLAG(-Wctor-dtor-privacy CXX_HAVE_OPT_Wctor_dtor_privacy)
# IF (CXX_HAVE_OPT_Wctor_dtor_privacy)
#   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wctor-dtor-privacy")
# ENDIF(CXX_HAVE_OPT_Wctor_dtor_privacy)

 CHECK_CXX_COMPILER_FLAG(-Wnon-virtual-dtor CXX_HAVE_OPT_Wnon_virtual_dtor)
 IF (CXX_HAVE_OPT_Wnon_virtual_dtor)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor")
 ENDIF(CXX_HAVE_OPT_Wnon_virtual_dtor)

 CHECK_CXX_COMPILER_FLAG(-Wpointer-arith CXX_HAVE_OPT_Wpointer_arith)
 IF (CXX_HAVE_OPT_Wpointer_arith)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith")
 ENDIF(CXX_HAVE_OPT_Wpointer_arith)

 CHECK_CXX_COMPILER_FLAG(-Wsign-compare CXX_HAVE_OPT_Wsign_compare)
 IF (CXX_HAVE_OPT_Wsign_compare)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare")
 ENDIF(CXX_HAVE_OPT_Wsign_compare)

 CHECK_CXX_COMPILER_FLAG(-Wswitch CXX_HAVE_OPT_Wswitch)
 IF (CXX_HAVE_OPT_Wswitch)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch")
 ENDIF(CXX_HAVE_OPT_Wswitch)

# CHECK_CXX_COMPILER_FLAG(-Wsystem-headers CXX_HAVE_OPT_Wsystem_headers)
# IF (CXX_HAVE_OPT_Wsystem_headers)
#   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsystem-headers")
# ENDIF(CXX_HAVE_OPT_Wsystem_headers)

 CHECK_CXX_COMPILER_FLAG(-Wtrigraphs CXX_HAVE_OPT_Wtrigraphs)
 IF (CXX_HAVE_OPT_Wtrigraphs)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wtrigraphs")
 ENDIF(CXX_HAVE_OPT_Wtrigraphs)

 CHECK_C_COMPILER_FLAG(-fvisibility=hidden CXX_HAVE_VISIBILITY)
 IF (CXX_HAVE_VISIBILITY)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
 ENDIF(CXX_HAVE_VISIBILITY)

 CHECK_CXX_COMPILER_FLAG(-fPIC CXX_HAVE_OPT_fpic)
 IF (CXX_HAVE_OPT_fpic)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
 ENDIF(CXX_HAVE_OPT_fpic)

# very strict
#CHECK_CXX_COMPILER_FLAG(-Weffc++ CXX_HAVE_OPT_effcpp)
#IF (CXX_HAVE_OPT_effcpp)
#  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
#ENDIF(CXX_HAVE_OPT_effcpp)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


# DEBUG
#IF (${CMAKE_BUILD_TYPE} EQUAL "DEBUG")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEW_ENABLE_DEBUG")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -ggdb")
#ENDIF()


ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL "PathScale")
# SET EW_USE_GNU_COMPATIBLE_COMPILER
MESSAGE(" --  CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-all-loops")
ENDIF ()


IF (${CMAKE_BUILD_TYPE} STREQUAL RELWITHDEBINFO)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
ENDIF (${CMAKE_BUILD_TYPE} STREQUAL RELWITHDEBINFO)


ENDIF (UNIX)

# set win32 c++ compiler flags
# /Gz for __stdcall : needed for use of  pointer to function in dynamic loaded libraries
#
IF (WIN32)
 SET(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /Gz")
ENDIF(WIN32)

MESSAGE(" --  CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
MESSAGE(" --  CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
MESSAGE(" --  CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG} ")
MESSAGE(" --  CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE} ")
MESSAGE(" --  CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO} ")
MESSAGE(" --  CMAKE_C_FLAGS_MINSIZEREL ${CMAKE_C_FLAGS_MINSIZEREL} ")
MESSAGE(" --  CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG} ")
MESSAGE(" --  CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE} ")
MESSAGE(" --  CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ")
MESSAGE(" --  CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL} ")
MESSAGE(" --  CMAKE_BUILD_TYPE is set to '${CMAKE_BUILD_TYPE}'")
MESSAGE(" --  INCLUDE_DIRECTORIES is set to '${INCLUDE_DIRECTORIES}'")


#
IF (${EW_STD_CPP} LESS 2011)
 MESSAGE(FATAL_ERROR "c++-11 support is required")
ENDIF()


# project dll / test

ADD_SUBDIRECTORY(cmake/ew/core/exception)

IF (UNIX)
ADD_SUBDIRECTORY(cmake/ew/core/syscall)
ADD_SUBDIRECTORY(cmake/ew/core/program)
ENDIF(UNIX)

ADD_SUBDIRECTORY(cmake/ew/core/threading)
ADD_SUBDIRECTORY(cmake/ew/core/object)
ADD_SUBDIRECTORY(cmake/ew/core/dll)
ADD_SUBDIRECTORY(cmake/ew/core/device)
ADD_SUBDIRECTORY(cmake/ew/core/time)
ADD_SUBDIRECTORY(cmake/ew/graphics)
ADD_SUBDIRECTORY(cmake/ew/maths)
ADD_SUBDIRECTORY(cmake/ew/system)
ADD_SUBDIRECTORY(cmake/ew/filesystem)
ADD_SUBDIRECTORY(cmake/ew/network)
ADD_SUBDIRECTORY(cmake/ew/console)
ADD_SUBDIRECTORY(cmake/ew/audio)
ADD_SUBDIRECTORY(cmake/ew/codecs/text/unicode/utf8)

# example

ADD_SUBDIRECTORY(cmake/example/cat)

ADD_SUBDIRECTORY(test)

ADD_SUBDIRECTORY(app)

ADD_SUBDIRECTORY(playground)
