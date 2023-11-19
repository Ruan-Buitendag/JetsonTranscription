cmake_minimum_required(VERSION 3.16)

project(Flickable VERSION 0.1 LANGUAGES C CXX)



set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS " -Wall -ffast-math -fopenmp ")
set(CMAKE_C_FLAGS " -Wall -ffast-math -fopenmp ")


find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Concurrent Multimedia)

qt_standard_project_setup()

qt_add_executable(appFlickable
    main.cpp
    activations.c CMakeLists.txt dictionary.c dynamicarray.c evaluation.c full_transcription.c matrix.c activation_postprocessing.c spectrogram.c stft.c wav.c
    activations.h dictionary.h dynamicarray.h evaluation.h full_transcription.h main.cpp matrix.h activation_postprocessing.h spectrogram.h stft.h transcription.cpp transcription.h wav.h
    midifile.h midifile.c domain_adaptation.c domain_adaptation.h
)

qt_add_qml_module(appFlickable
    URI Flickable
    VERSION 1.0
SOURCES
RESOURCES
QML_FILES Keyboard.qml KeyboardNote.qml Main.qml NoteActivation.qml Octave.qml GridBlock.qml CustomGrid.qml PianoRoll.qml
QML_FILES MainMenu.qml TranscriptionWaitingScreen.qml
QML_FILES SongLoadingScreen.qml
QML_FILES RecordingScreen.qml
QML_FILES BackButton.qml
QML_FILES DebugBox.qml
RESOURCES images/buttons/back/white.png images/buttons/go/white.png images/line.png
QML_FILES AudioDeviceSelector.qml
QML_FILES IterationSelector.qml
)


find_package(OpenMP)


target_link_libraries(appFlickable
    PRIVATE Qt6::Quick PUBLIC Qt6::Concurrent Qt6::Multimedia OpenMP::OpenMP_CXX
)


include(GNUInstallDirs)
install(TARGETS appFlickable
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

