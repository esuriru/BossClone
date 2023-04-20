set(PROJECT_NAME Application)



################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "Source/Components/BumperCarController.cpp"
    "Source/Components/BumperCarController.h"
    "Source/Components/Confetti.cpp"
    "Source/Components/Confetti.h"
    "Source/Components/Portal.cpp"
    "Source/Components/Portal.h"
    "Source/Scene/MainMenuScene.cpp"
    "Source/Scene/MainMenuScene.h"
)
source_group("" FILES ${no_group_source_files})

set(Header_Files
    "Source/Components/AABB.h"
    "Source/Components/AABB2D.h"
    "Source/Components/Ahri.h"
    "Source/Components/BoundingCircle.h"
    "Source/Components/BoundingSphere.h"
    "Source/Components/BoundingVolume.h"
    "Source/Components/BoundingVolume2D.h"
    "Source/Components/BoxCollider.h"
    "Source/Components/BoxCollider2D.h"
    "Source/Components/Camera.h"
    "Source/Components/CameraController.h"
    "Source/Components/CarouselMovement.h"
    "Source/Components/CircleCollider2D.h"
    "Source/Components/ClawMachineController.h"
    "Source/Components/Collider.h"
    "Source/Components/Collider2D.h"
    "Source/Components/Component.h"
    "Source/Components/CoveringRock.h"
    "Source/Components/DebugMovement.h"
    "Source/Components/Droppable.h"
    "Source/Components/FinishPoint.h"
    "Source/Components/ForceDebug.h"
    "Source/Components/Interactable.h"
    "Source/Components/InteractButton.h"
    "Source/Components/Light.h"
    "Source/Components/MeshFilter.h"
    "Source/Components/MeshRenderer.h"
    "Source/Components/ModelRenderer.h"
    "Source/Components/OBB2D.h"
    "Source/Components/PanningCameraController.h"
    "Source/Components/PlayerCameraController.h"
    "Source/Components/PlayerController.h"
    "Source/Components/PlayerMovement.h"
    "Source/Components/PolygonCollider2D.h"
    "Source/Components/Ray.h"
    "Source/Components/RayVolume.h"
    "Source/Components/Renderer.h"
    "Source/Components/RigidBody.h"
    "Source/Components/RigidBody2D.h"
    "Source/Components/Skybox.h"
    "Source/Components/SphereCollider.h"
    "Source/Components/SpotlightController.h"
    "Source/Components/StallFloor.h"
    "Source/Components/Talkable.h"
    "Source/Components/Terrain.h"
    "Source/Components/TerrainCollider.h"
    "Source/Components/TerrainVolume.h"
    "Source/Components/Text.h"
    "Source/Components/ThrowableRock.h"
    "Source/Components/Transform.h"
    "Source/Components/UIRenderer.h"
    "Source/Core/Application.h"
    "Source/Core/Core.h"
    "Source/Core/Layer.h"
    "Source/Core/LayerStack.h"
    "Source/Core/Log.h"
    "Source/Core/Timestep.h"
    "Source/Core/Window.h"
    "Source/Events/ApplicationEvent.h"
    "Source/Events/Event.h"
    "Source/Events/EventDispatcher.h"
    "Source/Events/KeyEvent.h"
    "Source/Events/MouseEvent.h"
    "Source/Physics/ColliderManager.h"
    "Source/Physics/Collision.h"
    "Source/Physics/Collision2D.h"
    "Source/Physics/Contact.h"
    "Source/Renderer/Material.h"
    "Source/Renderer/Mesh.h"
    "Source/Renderer/MeshBuilder.h"
    "Source/Renderer/SceneRenderer.h"
    "Source/Renderer/Shader.h"
    "Source/Renderer/ShaderLibrary.h"
    "Source/Renderer/Texture.h"
    "Source/Renderer/UniformBuffer.h"
    "Source/Scene/ArcadeScene.h"
    "Source/Scene/BottleThrowScene.h"
    "Source/Scene/BumperCarScene.h"
    "Source/Scene/DebugScene.h"
    "Source/Scene/GameObject.h"
    "Source/Scene/JungleScene.h"
    "Source/Scene/LoadingScene.h"
    "Source/Scene/MainScene.h"
    "Source/Scene/PoolTableScene.h"
    "Source/Scene/Scene.h"
    "Source/Scene/SceneLayer.h"
    "Source/Scene/SceneManager.h"
    "Source/Utils/Font.h"
    "Source/Utils/Input.h"
    "Source/Utils/LoadOBJ.h"
    "Source/Utils/LoadTGA.h"
    "Source/Utils/TextureManager.h"
    "Source/Utils/Util.h"
    "Source/Utils/Vertex.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "Source/Components/AABB.cpp"
    "Source/Components/AABB2D.cpp"
    "Source/Components/Ahri.cpp"
    "Source/Components/BoundingCircle.cpp"
    "Source/Components/BoundingSphere.cpp"
    "Source/Components/BoundingVolume.cpp"
    "Source/Components/BoundingVolume2D.cpp"
    "Source/Components/BoxCollider.cpp"
    "Source/Components/BoxCollider2D.cpp"
    "Source/Components/Camera.cpp"
    "Source/Components/CameraController.cpp"
    "Source/Components/CarouselMovement.cpp"
    "Source/Components/CircleCollider2D.cpp"
    "Source/Components/ClawMachineController.cpp"
    "Source/Components/Collider.cpp"
    "Source/Components/Collider2D.cpp"
    "Source/Components/Component.cpp"
    "Source/Components/CoveringRock.cpp"
    "Source/Components/DebugMovement.cpp"
    "Source/Components/Droppable.cpp"
    "Source/Components/FinishPoint.cpp"
    "Source/Components/ForceDebug.cpp"
    "Source/Components/Interactable.cpp"
    "Source/Components/InteractButton.cpp"
    "Source/Components/Light.cpp"
    "Source/Components/MeshFilter.cpp"
    "Source/Components/MeshRenderer.cpp"
    "Source/Components/ModelRenderer.cpp"
    "Source/Components/OBB2D.cpp"
    "Source/Components/PanningCameraController.cpp"
    "Source/Components/PlayerCameraController.cpp"
    "Source/Components/PlayerController.cpp"
    "Source/Components/PlayerMovement.cpp"
    "Source/Components/PolygonCollider2D.cpp"
    "Source/Components/Ray.cpp"
    "Source/Components/RayVolume.cpp"
    "Source/Components/Renderer.cpp"
    "Source/Components/RigidBody.cpp"
    "Source/Components/RigidBody2D.cpp"
    "Source/Components/Skybox.cpp"
    "Source/Components/SphereCollider.cpp"
    "Source/Components/SpotlightController.cpp"
    "Source/Components/StallFloor.cpp"
    "Source/Components/Talkable.cpp"
    "Source/Components/Terrain.cpp"
    "Source/Components/TerrainCollider.cpp"
    "Source/Components/TerrainVolume.cpp"
    "Source/Components/Text.cpp"
    "Source/Components/ThrowableRock.cpp"
    "Source/Components/Transform.cpp"
    "Source/Components/UIRenderer.cpp"
    "Source/Core/Application.cpp"
    "Source/Core/Layer.cpp"
    "Source/Core/LayerStack.cpp"
    "Source/Core/Log.cpp"
    "Source/Core/Window.cpp"
    "Source/glad.c"
    "Source/main.cpp"
    "Source/Physics/ColliderManager.cpp"
    "Source/Physics/Collision.cpp"
    "Source/Physics/Collision2D.cpp"
    "Source/Physics/Contact.cpp"
    "Source/Renderer/Material.cpp"
    "Source/Renderer/Mesh.cpp"
    "Source/Renderer/MeshBuilder.cpp"
    "Source/Renderer/SceneRenderer.cpp"
    "Source/Renderer/Shader.cpp"
    "Source/Renderer/ShaderLibrary.cpp"
    "Source/Renderer/Texture.cpp"
    "Source/Renderer/UniformBuffer.cpp"

    "Source/Scene/ArcadeScene.cpp"
    "Source/Scene/BottleThrowScene.cpp"
    "Source/Scene/BumperCarScene.cpp"
    "Source/Scene/DebugScene.cpp"
    "Source/Scene/GameObject.cpp"
    "Source/Scene/JungleScene.cpp"
    "Source/Scene/LoadingScene.cpp"
    "Source/Scene/MainScene.cpp"
    "Source/Scene/PoolTableScene.cpp"
    "Source/Scene/SceneLayer.cpp"
    "Source/Scene/SceneManager.cpp"

    "Source/ECS/Component.h"
    "Source/ECS/ComponentArray.h"
    "Source/ECS/IComponentArray.h"
    "Source/ECS/Coordindator.cpp"
    "Source/ECS/Coordindator.h"
    "Source/ECS/Entity.h"
    "Source/ECS/Entity.cpp"
    "Source/ECS/System.h"
    "Source/ECS/SystemManager.h"

    "Source/Utils/Font.cpp"
    "Source/Utils/Input.cpp"
    "Source/Utils/LoadOBJ.cpp"
    "Source/Utils/LoadTGA.cpp"
    "Source/Utils/TextureManager.cpp"
    "Source/Utils/Singleton.h"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${no_group_source_files}
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

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
        "${CMAKE_SOURCE_DIR}/vendor/glad/include;"
        "${CMAKE_SOURCE_DIR}/Application/Source;"
        "${CMAKE_SOURCE_DIR}/vendor/glm;"
        "${CMAKE_SOURCE_DIR}/vendor/stb_image;"
        "${CMAKE_SOURCE_DIR}/vendor/assimp/include;"
        "${CMAKE_SOURCE_DIR}/vendor/glfw/include;"
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
        "Common"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/vendor/assimp/lib;"
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
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${SHADER_SOURCE} ${SHADER_DEST} 
    DEPENDS ${SHADER_DEST}
    COMMENT "symbolic link assets folder from ${SHADER_SOURCE} => ${SHADER_DEST}"
)
