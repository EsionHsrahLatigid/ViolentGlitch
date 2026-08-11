cmake_minimum_required(VERSION 3.22)

project(ViolentGlitch VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VIOLENTGLITCH_BUILD_PLUGIN "Build the JUCE plug-in targets" ON)
option(VIOLENTGLITCH_BUILD_TESTS "Build processor integration tests" ON)
set(VIOLENTGLITCH_ARTIFACT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/artifacts" CACHE PATH
    "Directory for final VST3, AU, and Standalone products")

if(VIOLENTGLITCH_BUILD_TESTS)
    enable_testing()
endif()

if(VIOLENTGLITCH_BUILD_PLUGIN)
    include(FetchContent)

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
        add_subdirectory(JUCE)
    else()
        FetchContent_Declare(
            JUCE
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG 8.0.15
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(JUCE)
    endif()

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
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
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
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags
            juce::juce_recommended_warning_flags)

    set(violentglitch_juce_products "${CMAKE_CURRENT_BINARY_DIR}/ViolentGlitch_artefacts/$<CONFIG>")
    set(violentglitch_staged_products "${VIOLENTGLITCH_ARTIFACT_DIR}/$<CONFIG>")
    set(violentglitch_artifact_dependencies ViolentGlitch_VST3 ViolentGlitch_Standalone)
    set(violentglitch_artifact_commands
        COMMAND ${CMAKE_COMMAND} -E make_directory "${violentglitch_staged_products}"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${violentglitch_staged_products}/VST3"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${violentglitch_staged_products}/Standalone"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${violentglitch_juce_products}/VST3" "${violentglitch_staged_products}/VST3"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${violentglitch_juce_products}/Standalone" "${violentglitch_staged_products}/Standalone")

    if(TARGET ViolentGlitch_AU)
        list(APPEND violentglitch_artifact_dependencies ViolentGlitch_AU)
        list(APPEND violentglitch_artifact_commands
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${violentglitch_staged_products}/AU"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${violentglitch_juce_products}/AU" "${violentglitch_staged_products}/AU")
    endif()

    if(APPLE)
        list(APPEND violentglitch_artifact_commands
            COMMAND codesign --force --deep --sign -
                    "${violentglitch_staged_products}/VST3/ViolentGlitch.vst3"
            COMMAND codesign --force --deep --sign -
                    "${violentglitch_staged_products}/Standalone/ViolentGlitch.app"
            COMMAND codesign --force --deep --sign -
                    "${violentglitch_staged_products}/AU/ViolentGlitch.component")
    endif()

    add_custom_target(ViolentGlitch_Artifacts
        ${violentglitch_artifact_commands}
        DEPENDS ${violentglitch_artifact_dependencies}
        COMMENT "Staging ViolentGlitch products in ${VIOLENTGLITCH_ARTIFACT_DIR}/$<CONFIG>")

    if(VIOLENTGLITCH_BUILD_TESTS)
        add_executable(ViolentGlitchIntegrationTests Tests/PluginIntegrationTest.cpp)
        target_include_directories(ViolentGlitchIntegrationTests PRIVATE
            Source
            "${CMAKE_CURRENT_BINARY_DIR}/ViolentGlitch_artefacts/JuceLibraryCode")
        target_link_libraries(ViolentGlitchIntegrationTests PRIVATE ViolentGlitch juce::juce_audio_utils)
        add_dependencies(ViolentGlitchIntegrationTests ViolentGlitch_Artifacts)
        add_test(NAME ViolentGlitchPluginIntegrationTests COMMAND ViolentGlitchIntegrationTests)
        add_test(
            NAME ViolentGlitchArtifactChecks
            COMMAND ${CMAKE_COMMAND}
                    -DVIOLENTGLITCH_STAGED_DIR=${VIOLENTGLITCH_ARTIFACT_DIR}/$<CONFIG>
                    -DVIOLENTGLITCH_EXPECT_AU=$<BOOL:${APPLE}>
                    -P ${CMAKE_CURRENT_SOURCE_DIR}/Tests/VerifyArtifacts.cmake)
    endif()
endif()
