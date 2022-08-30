cmake_minimum_required(VERSION 3.6)

project("DasherServer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(ASIO_STANDALONE _WEBSOCKETPP_CPP11_STRICT_ HAVE_ROUND XML_STATIC)
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/DasherServer)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

###############################
# Building Dasher Library
###############################

add_subdirectory("DasherCore")


###############################
# Dasher Server
###############################

IF(WIN32)
	add_compile_definitions(_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING) # Apparently wrongly assigned in asio
	add_compile_definitions(WIN32_LEAN_AND_MEAN) # Apparently wrongly assigned in asio
ENDIF()

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

# Copy Data files
file(GLOB SERVER_DATA_FILES
  ${PROJECT_SOURCE_DIR}/DasherCore/Data/alphabets/alphabet*.xml
  ${PROJECT_SOURCE_DIR}/DasherCore/Data/colours/colour*.xml
  ${PROJECT_SOURCE_DIR}/DasherCore/Data/control/control*.xml
  ${PROJECT_SOURCE_DIR}/DasherCore/Data/training/training*.txt
)
file(COPY ${SERVER_DATA_FILES} DESTINATION ${CMAKE_BINARY_DIR}/DasherServer/Data)

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
set_property(TARGET DasherServer PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/DasherServer/)

add_dependencies(DasherServer DasherCore)
target_link_libraries(DasherServer PRIVATE  DasherCore Threads::Threads)
