set(PROJECT_NAME Library)

################################################################################
# Source groups
################################################################################
set(DesignPatterns
    "Source/DesignPatterns/SingletonTemplate.h"
)
source_group("DesignPatterns" FILES ${DesignPatterns})

set(GUI
    "Source/GUI/backends/imgui_impl_glfw.cpp"
    "Source/GUI/backends/imgui_impl_glfw.h"
    "Source/GUI/backends/imgui_impl_opengl3.cpp"
    "Source/GUI/backends/imgui_impl_opengl3.h"
    "Source/GUI/imconfig.h"
    "Source/GUI/imgui.cpp"
    "Source/GUI/imgui.h"
    "Source/GUI/imgui_demo.cpp"
    "Source/GUI/imgui_draw.cpp"
    "Source/GUI/imgui_internal.h"
    "Source/GUI/imgui_tables.cpp"
    "Source/GUI/imgui_widgets.cpp"
)
source_group("GUI" FILES ${GUI})

set(GameControl
    "Source/GameControl/Settings.cpp"
    "Source/GameControl/Settings.h"
)
source_group("GameControl" FILES ${GameControl})

set(Inputs
    "Source/Inputs/KeyboardController.cpp"
    "Source/Inputs/KeyboardController.h"
    "Source/Inputs/MouseController.cpp"
    "Source/Inputs/MouseController.h"
)
source_group("Inputs" FILES ${Inputs})

set(Primitives
    "Source/Primitives/Collider.cpp"
    "Source/Primitives/Collider.h"
    "Source/Primitives/CollisionManager.cpp"
    "Source/Primitives/CollisionManager.h"
    "Source/Primitives/Entity2D.cpp"
    "Source/Primitives/Entity2D.h"
    "Source/Primitives/Entity3D.cpp"
    "Source/Primitives/Entity3D.h"
    "Source/Primitives/IDGenerator.cpp"
    "Source/Primitives/IDGenerator.h"
    "Source/Primitives/LevelOfDetails.cpp"
    "Source/Primitives/LevelOfDetails.h"
    "Source/Primitives/Mesh.cpp"
    "Source/Primitives/Mesh.h"
    "Source/Primitives/MeshBuilder.cpp"
    "Source/Primitives/MeshBuilder.h"
    "Source/Primitives/SpriteAnimation.cpp"
    "Source/Primitives/SpriteAnimation.h"
)
source_group("Primitives" FILES ${Primitives})

set(RenderControl
    "Source/RenderControl/Shader.h"
    "Source/RenderControl/ShaderManager.cpp"
    "Source/RenderControl/ShaderManager.h"
)
source_group("RenderControl" FILES ${RenderControl})

set(System
    "Source/System/filesystem.h"
    "Source/System/ImageLoader.cpp"
    "Source/System/ImageLoader.h"
    "Source/System/LoadOBJ.cpp"
    "Source/System/LoadOBJ.h"
    "Source/System/rapidcsv.h"
)
source_group("System" FILES ${System})

set(TimeControl
    "Source/TimeControl/FPSCounter.cpp"
    "Source/TimeControl/FPSCounter.h"
    "Source/TimeControl/StopWatch.cpp"
    "Source/TimeControl/StopWatch.h"
)
source_group("TimeControl" FILES ${TimeControl})

set(ALL_FILES
    ${DesignPatterns}
    ${GUI}
    ${GameControl}
    ${Inputs}
    ${Primitives}
    ${RenderControl}
    ${System}
    ${TimeControl}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Common)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "$<$<CONFIG:Release>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../freetype/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Lua/include"
    ">"
    "${CMAKE_CURRENT_SOURCE_DIR}/../glew/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../glfw/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../glm;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../SOIL"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG;"
        "UNICODE;"
        "_UNICODE"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG;"
        "_MBCS"
    ">"
    "_VARIADIC_MAX=10;"
    "WIN32;"
    "_LIB"
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
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF
        >
        /SUBSYSTEM:WINDOWS
    )
endif()

