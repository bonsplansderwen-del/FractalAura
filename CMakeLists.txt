cmake_minimum_required(VERSION 3.22)
project(FractalAura VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE as a subdirectory (from git submodule)
add_subdirectory(JUCE)

# Define the plugin target
juce_add_plugin(FractalAura
    COMPANY_NAME "AuraDSP"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE AuDSP
    PLUGIN_CODE FrAu
    FORMATS VST3 AU
    PRODUCT_NAME "Fractal Aura"
    VST3_VERSION "1.0.0"
    AU_VERSION "1.0.0"
)

# Add source files
target_sources(FractalAura PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/GUI/MainEditor.cpp
    Source/GUI/MainEditor.h
    Source/AudioEngine/SpectralProcessor.cpp
    Source/AudioEngine/SpectralProcessor.h
    Source/AudioEngine/GranularEngine.h
    Source/AudioEngine/ChaosLFO.h
)

# Compile definitions
target_compile_definitions(FractalAura PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# macOS-specific settings
if(APPLE)
    set_target_properties(FractalAura PROPERTIES
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.auradsp.fractalaura"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
    )
    
    # Use native Clang with optimizations
    target_compile_options(FractalAura PRIVATE
        -ffast-math
        -march=native
        -mtune=generic
        -flto
    )
endif()

# Link JUCE modules
target_link_libraries(FractalAura PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_graphics
)
