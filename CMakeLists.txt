cmake_minimum_required(VERSION 3.22)
project(GalaxyTape VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)

set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_binary_data(GalaxyTapeAssets SOURCES assets/GalaxyTape.png)

juce_add_plugin(GalaxyTape
    COMPANY_NAME "Galaxy Tape"
    BUNDLE_ID "audio.galaxytape.plugin"
    PLUGIN_MANUFACTURER_CODE Galx
    PLUGIN_CODE Gtpe
    PRODUCT_NAME "Galaxy Tape"
    DESCRIPTION "Multi-head tape echo and reverb"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    FORMATS VST3
    VST3_CATEGORIES Fx Delay Reverb
)

target_sources(GalaxyTape PRIVATE
    src/GalaxyTapeProcessor.cpp
    src/GalaxyTapeProcessor.h
    src/GalaxyTapeEditor.cpp
    src/GalaxyTapeEditor.h
)

target_compile_features(GalaxyTape PRIVATE cxx_std_17)
target_compile_definitions(GalaxyTape
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(GalaxyTape
    PRIVATE
        GalaxyTapeAssets
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
