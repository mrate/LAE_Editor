PROJECT(LAE_Tools)
 
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#FILE(GLOB QTROOTS /media/Windows7_OS/Qt/qt-everywhere-opensource-src-4.7.4/bin)
#FIND_PROGRAM(QT_QMAKE_EXECUTABLE NAMES qmake qmake4 qmake-qt4 qmake-mac PATHS ${QTROOTS})

#set(CMAKE_INCLUDE_PATH /media/Windows7_OS/Qt/qt-everywhere-opensource-src-4.7.4/bin)
#set(CMAKE_PREFIX_PATH /media/Windows7_OS/Qt/qt-everywhere-opensource-src-4.7.4/bin)
set(CMAKE_INCLUDE_PATH /usr/local/Trolltech/Qt-4.8.5/bin)
set(CMAKE_PREFIX_PATH /usr/local/Trolltech/Qt-4.8.5)

FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL TRUE)
INCLUDE(${QT_USE_FILE})

ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES(
${QT_INCLUDES}
${QT_INCLUDE_DIR}
${QT_QT_INCLUDE_DIR}
${QT_QTCORE_INCLUDE_DIR}
${QT_QTGUI_INCLUDE_DIR}
${QT_QTOPENGL_INCLUDE_DIR}
)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)
SET(BOOST_VERSION 1_54_0)
SET(PROJECT_BINARY_DIR ${CMAKE_BUILD_TYPE})

INCLUDE_DIRECTORIES(source)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(..)
INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(../../libs/boost_${BOOST_VERSION})
INCLUDE_DIRECTORIES(../../libs/fmodapi44418linux/api/inc)
INCLUDE_DIRECTORIES(../../libs/tinyxml)
INCLUDE_DIRECTORIES(../../libs/luabind-0.9.1)
INCLUDE_DIRECTORIES(/usr/include/lua5.1)

if(DEFINED WIN32)
  MESSAGE(">> WINDOWS <<")
  ADD_DEFINITIONS(-DNOMINMAX -DUNICODE)
  INCLUDE_DIRECTORIES(../../libs/libpng/include)
  INCLUDE_DIRECTORIES(../../libs/zlib5)
  INCLUDE_DIRECTORIES("c:\\Program Files (x86)\\FMOD SoundSystem\\FMOD Programmers API Windows\\api\\inc")
endif(DEFINED WIN32)

SET(MAIN_SOURCES
    source/AnimationEditorWidget.cpp
    source/AnimationEditorWindow.cpp
    source/AnimationSelectorWindow.cpp
    source/lae_tools.cpp
    source/main.cpp
    source/PropertyTable.cpp
    source/qanimationlistbox.cpp
    source/qspritelistbox.cpp
    source/RoomEditorWidget.cpp
    source/RoomEditorWindow.cpp
    source/RoomObjectTableDelegate.cpp
    source/SelectFileWidget.cpp
    source/SpriteSelectorWindow.cpp
)

SET(MAIN_HEADERS
    source/AnimationEditorWidget.h
    source/AnimationEditorWindow.h
    source/AnimationSelectorWindow.h
    source/lae_tools.h
    source/ObjectIdentifier.h
    source/PropertyTable.h
    source/qanimationlistbox.h
    source/qspritelistbox.h
    source/RoomEditorWidget.h
    source/RoomEditorWindow.h
    source/RoomObjectTableDelegate.h
    source/SelectFileWidget.h
    source/SpriteSelectorWindow.h
)

QT4_WRAP_CPP(MAIN_HEADERS_MOC ${MAIN_HEADERS})

SET(MAIN_FORMS
    source/AnimationEditorWindow.ui
    source/AnimationSelectorWindow.ui
    source/lae_tools.ui
    source/RoomEditorWindow.ui
    source/SelectFileWidget.ui
    source/SpriteSelectorWindow.ui
)

QT4_WRAP_UI(MAIN_FORMS_HEADERS ${MAIN_FORMS})

SET(MAIN_RESOURCES
    lae_tools.qrc 
)

QT4_ADD_RESOURCES(MAIN_RESOURCES_RCC ${MAIN_RESOURCES})

ADD_EXECUTABLE(LAE_Tools 
  ${MAIN_SOURCES} 
  ${MAIN_HEADERS_MOC}
  ${MAIN_FORMS_HEADERS}
  ${MAIN_RESOURCES_RCC}
)

if(DEFINED UNIX)
  MESSAGE(">>> LINUX CONFIG <<<")

  TARGET_LINK_LIBRARIES(LAE_Tools /media/Windows7_OS/kotal/projects/MSVC/LuaAdventureEngine/libs/libLAE_Core.a)

  TARGET_LINK_LIBRARIES(LAE_Tools /media/Windows7_OS/kotal/projects/MSVC/libs/boost_1_54_0/stage/lib/libboost_thread.a)
  TARGET_LINK_LIBRARIES(LAE_Tools /media/Windows7_OS/kotal/projects/MSVC/libs/boost_1_54_0/stage/lib/libboost_system.a)
  TARGET_LINK_LIBRARIES(LAE_Tools /media/Windows7_OS/kotal/projects/MSVC/libs/boost_1_54_0/stage/lib/libboost_program_options.a)

  TARGET_LINK_LIBRARIES(LAE_Tools z)
  TARGET_LINK_LIBRARIES(LAE_Tools lua5.1)
  TARGET_LINK_LIBRARIES(LAE_Tools glut)
  TARGET_LINK_LIBRARIES(LAE_Tools GL)
  TARGET_LINK_LIBRARIES(LAE_Tools GLU)
  TARGET_LINK_LIBRARIES(LAE_Tools png)
  TARGET_LINK_LIBRARIES(LAE_Tools fmodex) 
  
  TARGET_LINK_LIBRARIES(LAE_Tools ${QT_LIBRARIES})
  
  ADD_DEFINITIONS(-Dnullptr=0)

  SET(CMAKE_CXX_FLAGS "-m32 -Wall -std=c++0x -pipe")
  SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
  SET(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -g3 -D_DEBUG")
  
  SET(CMAKE_C_FLAGS "-m32 -Wall")
  SET(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
  SET(CMAKE_C_FLAGS_DEBUG "-ggdb3 -g3 -D_DEBUG")

  ADD_DEFINITIONS(-Doverride=)
  ADD_DEFINITIONS(-DTIXML_USE_STL)
endif(DEFINED UNIX)

