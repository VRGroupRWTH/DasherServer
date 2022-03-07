cmake_minimum_required(VERSION 3.6)

project("DasherServer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(ASIO_STANDALONE _WEBSOCKETPP_CPP11_STRICT_)
set(CMAKE_SUPPRESS_REGENERATION true)

# Apparently wrongly assigned in asio
IF(WIN32)
	add_compile_definitions(HAVE_ROUND XML_STATIC _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
ENDIF()

###############################
# Building Dasher Library
###############################

add_subdirectory("DasherCore")


###############################
# Dasher Server
###############################

ADD_Executable(DasherServer
	${PROJECT_SOURCE_DIR}/Server/main.cpp
	${PROJECT_SOURCE_DIR}/Server/DasherController.cpp
	${PROJECT_SOURCE_DIR}/Server/DasherController.h
	${PROJECT_SOURCE_DIR}/Server/ParameterDefinitions.h
	${PROJECT_SOURCE_DIR}/Server/Declarations.h
	${PROJECT_SOURCE_DIR}/Server/ServerInput.cpp
	${PROJECT_SOURCE_DIR}/Server/ServerInput.h
	${PROJECT_SOURCE_DIR}/Server/ServerScreen.cpp
	${PROJECT_SOURCE_DIR}/Server/ServerScreen.h
	${PROJECT_SOURCE_DIR}/Server/ServerFontMeasure.h
	${PROJECT_SOURCE_DIR}/Server/ServerFontMeasure.cpp
)

###############################
# Third Party Includes
###############################
target_include_directories(DasherServer PRIVATE ${PROJECT_SOURCE_DIR}/Thirdparty/rapidjson/include)
target_include_directories(DasherServer PRIVATE ${PROJECT_SOURCE_DIR}/Thirdparty/websocketpp/)
target_include_directories(DasherServer PRIVATE ${PROJECT_SOURCE_DIR}/Thirdparty/asio/asio/include/)

###############################
# Choose Startup Project
###############################

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DasherServer)

add_dependencies(DasherServer DasherCore)
target_link_libraries(DasherServer DasherCore)
