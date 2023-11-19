import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 2.1
import QtQml 2.1

import Transcription 1.0

Window {
    id: window

    width: 640
    height: 480
    visible: true
    title: qsTr("Piano transcription")

    property int maxDimension: width

    onWidthChanged: {
        if (width > height)
            maxDimension = width
        else
            maxDimension = height
    }

    onHeightChanged: {
        if (width > height)
            maxDimension = width
        else
            maxDimension = height
    }

    StackView {
        id: stackview

        initialItem: MainMenu {}
        //        initialItem: RecordingScreen {}
        anchors.fill: parent

        onCurrentItemChanged: {

            //            console.log("StackChanged: ", stackview.children)
        }
    }
    Transcription {
        id: transcription

        onNotesChanged: {
            console.log("Transcription done ")
            console.log("Showing piano roll")
            //            stackview.push(pianoroll)
            stackview.replace(pianoroll)
        }
    }

    Component {
        id: pianoroll
        PianoRoll {}
    }

    Component {
        id: recording_screen

        RecordingScreen {}
    }

    Component {
        id: song_loading_screen

        SongLoadingScreen {}
    }

    Component {
        id: transcription_waiting_screen

        TranscriptionWaitingScreen {}
    }

    function doTranscription(type) {
        console.log("Pushing transcription waiting screen")
        stackview.push(transcription_waiting_screen)
        console.log("Starting transcription")

        if (type == "recorded") {
            transcription.startRecordedTranscription()
        } else if (type == "live") {
            transcription.startLiveTranscription()
        }
    }
}
