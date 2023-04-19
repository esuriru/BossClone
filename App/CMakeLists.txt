set(PROJECT_NAME App)

################################################################################
# Source groups
################################################################################
set(GameStateManagement
    "Source/GameStateManagement/GameStateBase.h"
    "Source/GameStateManagement/GameStateManager.cpp"
    "Source/GameStateManagement/GameStateManager.h"
    "Source/GameStateManagement/IntroState.cpp"
    "Source/GameStateManagement/IntroState.h"
    "Source/GameStateManagement/MenuState.cpp"
    "Source/GameStateManagement/MenuState.h"
    "Source/GameStateManagement/PauseState.cpp"
    "Source/GameStateManagement/PauseState.h"
    "Source/GameStateManagement/Play3DGameState.cpp"
    "Source/GameStateManagement/Play3DGameState.h"
    "Source/GameStateManagement/PlayGameState.cpp"
    "Source/GameStateManagement/PlayGameState.h"
)
source_group("GameStateManagement" FILES ${GameStateManagement})

set(Header_Files
    "Source/Application.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Scene2D
    "Source/Scene2D/BackgroundEntity.cpp"
    "Source/Scene2D/BackgroundEntity.h"
    "Source/Scene2D/Enemy2D.cpp"
    "Source/Scene2D/Enemy2D.h"
    "Source/Scene2D/Enemy2DManager.cpp"
    "Source/Scene2D/Enemy2DManager.h"
    "Source/Scene2D/GameManager.cpp"
    "Source/Scene2D/GameManager.h"
    "Source/Scene2D/GUI_Scene2D.cpp"
    "Source/Scene2D/GUI_Scene2D.h"
    "Source/Scene2D/InventoryItem.cpp"
    "Source/Scene2D/InventoryItem.h"
    "Source/Scene2D/InventoryManager.cpp"
    "Source/Scene2D/InventoryManager.h"
    "Source/Scene2D/Map2D.cpp"
    "Source/Scene2D/Map2D.h"
    "Source/Scene2D/Physics2D.cpp"
    "Source/Scene2D/Physics2D.h"
    "Source/Scene2D/Player2D.cpp"
    "Source/Scene2D/Player2D.h"
    "Source/Scene2D/Scene2D.cpp"
    "Source/Scene2D/Scene2D.h"
)
source_group("Scene2D" FILES ${Scene2D})

set(Scene3D
    "Source/Scene3D/Camera.cpp"
    "Source/Scene3D/Camera.h"
    "Source/Scene3D/GUI_Scene3D.cpp"
    "Source/Scene3D/GUI_Scene3D.h"
    "Source/Scene3D/Physics3D.cpp"
    "Source/Scene3D/Physics3D.h"
    "Source/Scene3D/Scene3D.cpp"
    "Source/Scene3D/Scene3D.h"
)
source_group("Scene3D" FILES ${Scene3D})

set(Scene3D__CameraEffects
    "Source/Scene3D/CameraEffects/BloodScreen.cpp"
    "Source/Scene3D/CameraEffects/BloodScreen.h"
    "Source/Scene3D/CameraEffects/CameraEffects.cpp"
    "Source/Scene3D/CameraEffects/CameraEffects.h"
    "Source/Scene3D/CameraEffects/CameraEffectsManager.cpp"
    "Source/Scene3D/CameraEffects/CameraEffectsManager.h"
    "Source/Scene3D/CameraEffects/CameraShake.cpp"
    "Source/Scene3D/CameraEffects/CameraShake.h"
    "Source/Scene3D/CameraEffects/CrossHair.cpp"
    "Source/Scene3D/CameraEffects/CrossHair.h"
    "Source/Scene3D/CameraEffects/ScopeScreen.cpp"
    "Source/Scene3D/CameraEffects/ScopeScreen.h"
)
source_group("Scene3D\\CameraEffects" FILES ${Scene3D__CameraEffects})

set(Scene3D__Entities
    "Source/Scene3D/Entities/Enemy3D.cpp"
    "Source/Scene3D/Entities/Enemy3D.h"
    "Source/Scene3D/Entities/EntityManager.cpp"
    "Source/Scene3D/Entities/EntityManager.h"
    "Source/Scene3D/Entities/Hut_Concrete.cpp"
    "Source/Scene3D/Entities/Hut_Concrete.h"
    "Source/Scene3D/Entities/Player3D.cpp"
    "Source/Scene3D/Entities/Player3D.h"
    "Source/Scene3D/Entities/Rock3D.cpp"
    "Source/Scene3D/Entities/Rock3D.h"
    "Source/Scene3D/Entities/SolidObject.cpp"
    "Source/Scene3D/Entities/SolidObject.h"
    "Source/Scene3D/Entities/SolidObjectManager.cpp"
    "Source/Scene3D/Entities/SolidObjectManager.h"
    "Source/Scene3D/Entities/Structure3D.cpp"
    "Source/Scene3D/Entities/Structure3D.h"
    "Source/Scene3D/Entities/TreeKabak3D.cpp"
    "Source/Scene3D/Entities/TreeKabak3D.h"
)
source_group("Scene3D\\Entities" FILES ${Scene3D__Entities})

set(Scene3D__FSM
    "Source/Scene3D/AI/FSM.cpp"
    "Source/Scene3D/AI/FSM.h"
)
source_group("Scene3D\\FSM" FILES ${Scene3D__FSM})

set(Scene3D__Minimap
    "Source/Scene3D/Minimap/Minimap.cpp"
    "Source/Scene3D/Minimap/Minimap.h"
)
source_group("Scene3D\\Minimap" FILES ${Scene3D__Minimap})

set(Scene3D__SceneGraph
    "Source/Scene3D/SceneGraph/SceneNode.cpp"
    "Source/Scene3D/SceneGraph/SceneNode.h"
    "Source/Scene3D/SceneGraph/SpinTower.cpp"
    "Source/Scene3D/SceneGraph/SpinTower.h"
)
source_group("Scene3D\\SceneGraph" FILES ${Scene3D__SceneGraph})

set(Scene3D__SkyBox
    "Source/Scene3D/SkyBox/SkyBox.cpp"
    "Source/Scene3D/SkyBox/SkyBox.h"
)
source_group("Scene3D\\SkyBox" FILES ${Scene3D__SkyBox})

set(Scene3D__SpatialPartition
    "Source/Scene3D/SpatialPartition/FrustumCulling.cpp"
    "Source/Scene3D/SpatialPartition/FrustumCulling.h"
    "Source/Scene3D/SpatialPartition/Grid.cpp"
    "Source/Scene3D/SpatialPartition/Grid.h"
    "Source/Scene3D/SpatialPartition/SpatialPartition.cpp"
    "Source/Scene3D/SpatialPartition/SpatialPartition.h"
)
source_group("Scene3D\\SpatialPartition" FILES ${Scene3D__SpatialPartition})

set(Scene3D__Terrain
    "Source/Scene3D/Terrain/Terrain.cpp"
    "Source/Scene3D/Terrain/Terrain.h"
    "Source/Scene3D/Terrain/TerrainManager.cpp"
    "Source/Scene3D/Terrain/TerrainManager.h"
    "Source/Scene3D/Terrain/Texture.cpp"
    "Source/Scene3D/Terrain/Texture.h"
    "Source/Scene3D/Terrain/vertexBufferObject.cpp"
    "Source/Scene3D/Terrain/vertexBufferObject.h"
)
source_group("Scene3D\\Terrain" FILES ${Scene3D__Terrain})

set(Scene3D__WayPoint
    "Source/Scene3D/Waypoint/Waypoint.cpp"
    "Source/Scene3D/Waypoint/Waypoint.h"
    "Source/Scene3D/Waypoint/WaypointManager.cpp"
    "Source/Scene3D/Waypoint/WaypointManager.h"
)
source_group("Scene3D\\WayPoint" FILES ${Scene3D__WayPoint})

set(Scene3D__WeaponInfo
    "Source/Scene3D/WeaponInfo/Pistol.cpp"
    "Source/Scene3D/WeaponInfo/Pistol.h"
    "Source/Scene3D/WeaponInfo/Projectile.cpp"
    "Source/Scene3D/WeaponInfo/Projectile.h"
    "Source/Scene3D/WeaponInfo/ProjectileManager.cpp"
    "Source/Scene3D/WeaponInfo/ProjectileManager.h"
    "Source/Scene3D/WeaponInfo/WeaponInfo.cpp"
    "Source/Scene3D/WeaponInfo/WeaponInfo.h"
)
source_group("Scene3D\\WeaponInfo" FILES ${Scene3D__WeaponInfo})

set(Source_Files
    "Source/Application.cpp"
    "Source/main.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${GameStateManagement}
    ${Header_Files}
    ${Scene2D}
    ${Scene3D}
    ${Scene3D__CameraEffects}
    ${Scene3D__Entities}
    ${Scene3D__FSM}
    ${Scene3D__Minimap}
    ${Scene3D__SceneGraph}
    ${Scene3D__SkyBox}
    ${Scene3D__SpatialPartition}
    ${Scene3D__Terrain}
    ${Scene3D__WayPoint}
    ${Scene3D__WeaponInfo}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Base)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
    $<$<CONFIG:Debug>:
        MultiThreadedDebugDLL
    >
    $<$<NOT:$<OR:$<CONFIG:Debug>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
)
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "$<$<CONFIG:Release>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../irrKlang;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Lua/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../FreeImage"
    ">"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Library/Source;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../glfw/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../glew/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../glm"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG;"
        "_CRT_SECURE_NO_WARNINGS;"
        "_MBCS"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG;"
        "UNICODE;"
        "_UNICODE"
    ">"
    "_VARIADIC_MAX=10;"
    "WIN32;"
    "_CONSOLE"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Od;
            /ZI
        >
        $<$<CONFIG:Release>:
            /O2;
            /Oi;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
        >
        /W3;
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /INCREMENTAL
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO
        >
        /DEBUG;
        /SUBSYSTEM:CONSOLE
    )
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    Library
)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:Release>:"
        "freetype;"
        "irrKlang;"
        "FreeImage"
    ">"
    "legacy_stdio_definitions;"
    "Library;"
    "winmm;"
    "opengl32;"
    "glu32;"
    "glew32;"
    "glfw3;"
    "SOIL"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Release>:"
        "${CMAKE_SOURCE_DIR}//freetype/win32;"
        "${CMAKE_SOURCE_DIR}//Lua/lib;"
        "${CMAKE_SOURCE_DIR}//FreeImage"
    ">"
    "${CMAKE_SOURCE_DIR}/$<CONFIG>;"
    "${CMAKE_SOURCE_DIR}//glfw/lib-vc2010-32;"
    "${CMAKE_SOURCE_DIR}//glew/lib;"
    "${CMAKE_SOURCE_DIR}//SOIL/lib;"
    "${CMAKE_SOURCE_DIR}//irrKlang/lib"
)

