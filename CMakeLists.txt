cmake_minimum_required(VERSION 3.22)

project(ViolentGlitch VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VIOLENTGLITCH_BUILD_PLUGIN "Build the JUCE plug-in targets" ON)
option(VIOLENTGLITCH_BUILD_TESTS "Build processor integration tests" ON)
set(VIOLENTGLITCH_ARTIFACT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/artifacts" CACHE PATH
    "Directory for final VST3, AU, and Standalone products")
set(EHL_JUCE_SOURCE_DIR "" CACHE PATH "Existing JUCE source checkout; avoids network when set")

if(VIOLENTGLITCH_BUILD_TESTS)
    enable_testing()
endif()

if(VIOLENTGLITCH_BUILD_PLUGIN)
    include(FetchContent)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    if(EHL_JUCE_SOURCE_DIR)
        if(NOT EXISTS "${EHL_JUCE_SOURCE_DIR}/CMakeLists.txt")
            message(FATAL_ERROR "EHL_JUCE_SOURCE_DIR does not contain JUCE: ${EHL_JUCE_SOURCE_DIR}")
        endif()
        add_subdirectory("${EHL_JUCE_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/juce")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
        add_subdirectory(JUCE)
    else()
        FetchContent_Declare(
            JUCE
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG 8.0.15
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(JUCE)
    endif()
    add_subdirectory(modules/juce-ehl-design-module EXCLUDE_FROM_ALL)

    set(VIOLENTGLITCH_FORMATS VST3 Standalone)
    if(APPLE)
        list(APPEND VIOLENTGLITCH_FORMATS AU)
    endif()

    juce_add_plugin(ViolentGlitch
        VERSION ${PROJECT_VERSION}
        COMPANY_NAME "EsionHsrahLatigid"
        BUNDLE_ID "jp.ehl.violentglitch"
        PRODUCT_NAME "ViolentGlitch"
        DESCRIPTION "Destructive digital glitch distortion effect"
        PLUGIN_MANUFACTURER_CODE EHL_
        PLUGIN_CODE Vglt
        FORMATS ${VIOLENTGLITCH_FORMATS}
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        COPY_PLUGIN_AFTER_BUILD FALSE
        VST3_CATEGORIES Fx Distortion
        AU_MAIN_TYPE kAudioUnitType_Effect)

    juce_generate_juce_header(ViolentGlitch)

    target_sources(ViolentGlitch PRIVATE
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h)

    target_compile_definitions(ViolentGlitch PUBLIC
        JUCE_REPORT_APP_USAGE=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=0)

    target_link_libraries(ViolentGlitch
        PRIVATE
            juce::juce_audio_utils
            juce::juce_dsp
            EHL::JuceDesign
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags
            juce::juce_recommended_warning_flags)

    set(violentglitch_stage_platform "linux-x64")
    if(APPLE)
        set(violentglitch_stage_platform "macos-arm64")
    elseif(WIN32)
        set(violentglitch_stage_platform "windows-x64")
    endif()
    set(violentglitch_juce_products "${CMAKE_CURRENT_BINARY_DIR}/ViolentGlitch_artefacts/$<CONFIG>")
    set(violentglitch_staged_products "${VIOLENTGLITCH_ARTIFACT_DIR}/plugin-release/${violentglitch_stage_platform}")
    set(violentglitch_artifact_dependencies ViolentGlitch_VST3 ViolentGlitch_Standalone)
    set(violentglitch_artifact_commands
        COMMAND ${CMAKE_COMMAND} -E make_directory "${violentglitch_staged_products}/vst3"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${violentglitch_staged_products}/standalone"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${violentglitch_staged_products}/au")

    if(APPLE)
        list(APPEND violentglitch_artifact_commands
            COMMAND ${CMAKE_COMMAND} -E rm -rf "${violentglitch_staged_products}/vst3/violentglitch_vst3_plugin.vst3"
            COMMAND ${CMAKE_COMMAND} -E rm -rf "${violentglitch_staged_products}/standalone/violentglitch_standalone_plugin.app"
            COMMAND ${CMAKE_COMMAND} -E rm -rf "${violentglitch_staged_products}/au/violentglitch_au_plugin.component"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${violentglitch_juce_products}/VST3/ViolentGlitch.vst3" "${violentglitch_staged_products}/vst3/violentglitch_vst3_plugin.vst3"
            COMMAND ${Python3_EXECUTABLE}
                    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NormalizeVst3ModuleInfo.py"
                    "${violentglitch_staged_products}/vst3/violentglitch_vst3_plugin.vst3/Contents/Resources/moduleinfo.json"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${violentglitch_juce_products}/Standalone/ViolentGlitch.app" "${violentglitch_staged_products}/standalone/violentglitch_standalone_plugin.app")
    elseif(WIN32)
        list(APPEND violentglitch_artifact_commands
            COMMAND ${CMAKE_COMMAND} -E rm -rf "${violentglitch_staged_products}/vst3/violentglitch_vst3_plugin.vst3"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${violentglitch_juce_products}/VST3/ViolentGlitch.vst3" "${violentglitch_staged_products}/vst3/violentglitch_vst3_plugin.vst3"
            COMMAND ${Python3_EXECUTABLE}
                    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NormalizeVst3ModuleInfo.py"
                    "${violentglitch_staged_products}/vst3/violentglitch_vst3_plugin.vst3/Contents/Resources/moduleinfo.json"
            COMMAND ${CMAKE_COMMAND} -E copy
                    "${violentglitch_juce_products}/Standalone/ViolentGlitch.exe" "${violentglitch_staged_products}/standalone/violentglitch_standalone_plugin.exe")
    else()
        list(APPEND violentglitch_artifact_commands
            COMMAND ${CMAKE_COMMAND} -E rm -rf "${violentglitch_staged_products}/vst3/violentglitch_vst3_plugin.vst3"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${violentglitch_juce_products}/VST3/ViolentGlitch.vst3" "${violentglitch_staged_products}/vst3/violentglitch_vst3_plugin.vst3"
            COMMAND ${Python3_EXECUTABLE}
                    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NormalizeVst3ModuleInfo.py"
                    "${violentglitch_staged_products}/vst3/violentglitch_vst3_plugin.vst3/Contents/Resources/moduleinfo.json"
            COMMAND ${CMAKE_COMMAND} -E copy
                    "${violentglitch_juce_products}/Standalone/ViolentGlitch" "${violentglitch_staged_products}/standalone/violentglitch_standalone_plugin")
    endif()

    if(TARGET ViolentGlitch_AU)
        list(APPEND violentglitch_artifact_dependencies ViolentGlitch_AU)
        list(APPEND violentglitch_artifact_commands
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${violentglitch_juce_products}/AU/ViolentGlitch.component" "${violentglitch_staged_products}/au/violentglitch_au_plugin.component")
    endif()

    if(APPLE)
        list(APPEND violentglitch_artifact_commands
            COMMAND codesign --force --deep --sign -
                    "${violentglitch_staged_products}/vst3/violentglitch_vst3_plugin.vst3"
            COMMAND codesign --force --deep --sign -
                    "${violentglitch_staged_products}/standalone/violentglitch_standalone_plugin.app"
            COMMAND codesign --force --deep --sign -
                    "${violentglitch_staged_products}/au/violentglitch_au_plugin.component")
    endif()

    list(APPEND violentglitch_artifact_commands
        COMMAND ${CMAKE_COMMAND}
            -DPRODUCT_NAME=ViolentGlitch
            -DSLUG=violentglitch
            -DBUNDLE_ID=jp.ehl.violentglitch
            -DPLUGIN_CODE=Vglt
            -DSTAGE_DIR=${violentglitch_staged_products}
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/WriteArtifactsManifest.cmake")

    add_custom_target(ehl_stage_products
        ${violentglitch_artifact_commands}
        DEPENDS ${violentglitch_artifact_dependencies}
        COMMENT "Staging ViolentGlitch products under ${VIOLENTGLITCH_ARTIFACT_DIR}/plugin-release")

    add_custom_target(ViolentGlitch_Artifacts DEPENDS ehl_stage_products)

    if(VIOLENTGLITCH_BUILD_TESTS)
        add_executable(ViolentGlitchIntegrationTests Tests/PluginIntegrationTest.cpp)
        target_include_directories(ViolentGlitchIntegrationTests PRIVATE
            Source
            "${CMAKE_CURRENT_BINARY_DIR}/ViolentGlitch_artefacts/JuceLibraryCode")
        target_link_libraries(ViolentGlitchIntegrationTests PRIVATE
            ViolentGlitch
            EHL::JuceDesign
            juce::juce_audio_utils)
        add_dependencies(ViolentGlitchIntegrationTests ViolentGlitch_Artifacts)
        add_test(NAME ViolentGlitchPluginIntegrationTests COMMAND ViolentGlitchIntegrationTests)
        add_test(
            NAME ViolentGlitchArtifactChecks
            COMMAND ${CMAKE_COMMAND}
                    -DBUILD_CONFIG=$<CONFIG>
                    -DSTAGE_DIR=${violentglitch_staged_products}
                    -DSLUG=violentglitch
                    -DEXPECT_AU=$<BOOL:${APPLE}>
                    -DPython3_EXECUTABLE=${Python3_EXECUTABLE}
                    -P ${CMAKE_CURRENT_SOURCE_DIR}/Tests/VerifyArtifacts.cmake)
    endif()
endif()
