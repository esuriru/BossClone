set(PROJECT_NAME Application)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "Source/Core/Application.h"
    "Source/Core/Core.h"
    "Source/Core/Layer.h"
    "Source/Core/LayerStack.h"
    "Source/Core/Log.h"
    "Source/Core/Timestep.h"
    "Source/Core/Window.h"
    "Source/Core/KeyCodes.h"

    "Source/Events/ApplicationEvent.h"
    "Source/Events/Event.h"
    "Source/Events/EventDispatcher.h"
    "Source/Events/KeyEvent.h"
    "Source/Events/MouseEvent.h"

    "Source/Renderer/Shader.h"
    "Source/Renderer/ShaderLibrary.h"
    "Source/Renderer/Texture.h"
    "Source/Renderer/Renderer2D.h"
    "Source/Renderer/RenderCommand.h"
    "Source/Renderer/VertexArray.h"
    "Source/Renderer/Buffer.h"
    "Source/Renderer/Texture.h"
    "Source/Renderer/SubTexture2D.h"
    "Source/Renderer/OrthographicCamera.h"

    "Source/Game/MainLayer.h"
    "Source/Game/MenuLayer.h"
    "Source/Game/OrthographicCameraController.h"
    "Source/Game/Tilemap.h"
    "Source/Game/GameManager.h"
    "Source/Game/State.h"
    "Source/Game/StateMachine.h"

    "Source/Scene/Scene.h"
    "Source/Scene/SceneManager.h"

    "Source/Audio/Transition.h"

    "Source/ECS/System.h"
    "Source/ECS/SystemManager.h"
    "Source/ECS/Coordinator.h"
    "Source/ECS/Entity.h"
    "Source/ECS/Component.h"
    "Source/ECS/ComponentArray.h"
    "Source/ECS/IComponentArray.h"

    "Source/ECS/Systems/RenderSystem.h"
    "Source/ECS/Systems/PlayerSystem.h"
    "Source/ECS/Systems/GameSystem.h"
    "Source/ECS/Systems/GUISystem.h"
    "Source/ECS/Systems/BatSystem.h"
    "Source/ECS/Systems/NightborneSystem.h"

    "Source/Utils/Input.h"
    "Source/Utils/Util.h"
    "Source/Utils/Singleton.h"

    "Source/Physics/PhysicsSystem.h"
    "Source/Physics/PhysicsComponent.h"
    "Source/Physics/Quadtree.h"
    "Source/Physics/Collision2D.h"

    "Source/ImGui/ImGuiLayer.h"

    "Source/Utils/MusicPlayer.h"
    "Source/Utils/SoundController.h"
    "Source/Utils/SoundInfo.h"
    "Source/Utils/filesystem.h"

)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "Source/Core/Application.cpp"
    "Source/Core/Layer.cpp"
    "Source/Core/LayerStack.cpp"
    "Source/Core/Log.cpp"
    "Source/Core/Window.cpp"

    "Source/glad.c"
    "Source/main.cpp"

    "Source/Renderer/Shader.cpp"
    "Source/Renderer/ShaderLibrary.cpp"
    "Source/Renderer/Texture.cpp"
    "Source/Renderer/Renderer2D.cpp"
    "Source/Renderer/RenderCommand.cpp"
    "Source/Renderer/VertexArray.cpp"
    "Source/Renderer/Buffer.cpp"
    "Source/Renderer/Texture.cpp"
    "Source/Renderer/SubTexture2D.cpp"
    "Source/Renderer/OrthographicCamera.cpp"

    "Source/Scene/SceneManager.cpp"
    "Source/Scene/Scene.cpp"

    "Source/Game/MenuLayer.cpp"
    "Source/Game/MainLayer.cpp"
    "Source/Game/OrthographicCameraController.cpp"
    "Source/Game/Tilemap.cpp"
    "Source/Game/GameManager.cpp"

    "Source/ECS/Coordinator.cpp"
    "Source/ECS/Entity.cpp"

    "Source/Audio/Transition.cpp"

    "Source/ECS/Systems/RenderSystem.cpp"
    "Source/ECS/Systems/PlayerSystem.cpp"
    "Source/ECS/Systems/GameSystem.cpp"
    "Source/ECS/Systems/GUISystem.cpp"
    "Source/ECS/Systems/BatSystem.cpp"
    "Source/ECS/Systems/NightborneSystem.cpp"
    "Source/Utils/Input.cpp"

    "Source/Physics/PhysicsSystem.cpp"
    "Source/Physics/PhysicsComponent.cpp"
    "Source/Physics/Quadtree.cpp"

    "Source/ImGui/ImGuiLayer.cpp"

    "Source/Utils/MusicPlayer.cpp"
    "Source/Utils/SoundController.cpp"
    "Source/Utils/SoundInfo.cpp"
)

source_group("Source Files" FILES ${Source_Files})

# set (IMGUI_OPENGL_FILES 
#     "../vendor/imgui/imgui/examples/imgui_impl_opengl3.h"
#     "../vendor/imgui/imgui/examples/imgui_impl_opengl3.cpp"
#     "../vendor/imgui/imgui/examples/imgui_impl_opengl3_loader.h"
# )

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
    ${IMGUI_SOURCES}
    # ${IMGUI_OPENGL_FILES}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Application)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "${CMAKE_SOURCE_DIR}/glew/include;"
            "${CMAKE_SOURCE_DIR}/glfw/include;"
            "${CMAKE_SOURCE_DIR}/Common/Source"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_SOURCE_DIR}/vendor/rapidcsv;"
        "${CMAKE_SOURCE_DIR}/vendor/glad/include;"
        "${CMAKE_SOURCE_DIR}/Application/Source;"
        "${CMAKE_SOURCE_DIR}/vendor/glm;"
        "${CMAKE_SOURCE_DIR}/vendor/stb_image;"
        "${CMAKE_SOURCE_DIR}/vendor/assimp/include;"
        "${CMAKE_SOURCE_DIR}/vendor/glfw/include;"
        "${CMAKE_SOURCE_DIR}/vendor/imgui/imgui;"
        "${CMAKE_SOURCE_DIR}/vendor/irrKlang;"
        "${CMAKE_SOURCE_DIR}/Common/Source"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "NOMINMAX;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
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
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE;
            /INCREMENTAL
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    Common
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "assimp-vc143-mtd"
        ">"
        "$<$<CONFIG:Release>:"
            "assimp-vc143-mt"
        ">"
        "glfw3;"
        "opengl32;"
        "winmm;"
        "Common;"
        "irrKlang;"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/vendor/assimp/lib;"
        "${CMAKE_SOURCE_DIR}/vendor/irrKlang/lib;"
        "${CMAKE_SOURCE_DIR}/vendor/glfw/lib-vc2019;"
        "${CMAKE_SOURCE_DIR}/$<CONFIG>"
    )
endif()

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()

get_target_property(output_dir ${PROJECT_NAME} OUTPUT_DIRECTORY_DEBUG)

# Symlinks
set (ASSET_SOURCE "${CMAKE_CURRENT_LIST_DIR}/Assets")
set (ASSET_DEST "${output_dir}/Assets")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${ASSET_SOURCE} ${ASSET_DEST} 
    DEPENDS ${ASSET_DEST}
    COMMENT "symbolic link assets folder from ${ASSET_SOURCE} => ${ASSET_DEST}"
)

set (SHADER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/Shader")
set (SHADER_DEST "${output_dir}/Shader")
# execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${SHADER_SOURCE} ${SHADER_DEST})
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${SHADER_SOURCE} ${SHADER_DEST} 
    DEPENDS ${SHADER_DEST}
    COMMENT "symbolic link shader folder from ${SHADER_SOURCE} => ${SHADER_DEST}"
)
