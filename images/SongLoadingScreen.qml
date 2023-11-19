import QtQuick 2.9
import QtQuick.Controls 2.1
import QtQuick.Dialogs 1.1
import QtQml 2.8
import Transcription 1.0

//import QtCore 2.15
Rectangle {
    anchors.fill: parent

    color: "#2f2f2f"

    Text {
        anchors.centerIn: parent
        text: "Song loading"
    }

    FileDialog {
        id: wavFileDialog
        folder: "file:///C:/Users/ruanb/OneDrive/Desktop/Piano Transcripton/Piano transcription/MAPS"

        onAccepted: {
            transcription.songName = fileUrl
            console.log("Wav file: " + fileUrl)
        }
    }

    FileDialog {
        id: transcriptionFileDialog

        folder: "file:///C:/Users/ruanb/OneDrive/Desktop/Piano Transcripton/Piano transcription/MAPS"

        onAccepted: {
            transcription.transcriptionFile = fileUrl
            console.log("Transcription file: " + fileUrl)
        }
    }

    Row {
        anchors.centerIn: parent

        Rectangle {

            width: 100
            height: 100

            color: "red"

            //        anchors.centerIn: parent
            MouseArea {
                anchors.fill: parent

                onClicked: transcriptionFileDialog.open()
            }
        }

        Rectangle {
            x: 400

            width: 100
            height: 100

            color: "orange"

            //        anchors.centerIn: parent
            MouseArea {
                anchors.fill: parent

                onClicked: wavFileDialog.open()
            }
        }

        Rectangle {
            x: 500

            width: 100
            height: 100

            color: "green"

            //        anchors.centerIn: parent
            MouseArea {
                anchors.fill: parent

                onClicked: {
                    doTranscription("recorded")
                }
            }
        }
    }

    BackButton {}

    Component.onCompleted: {
        anchors.fill = undefined
    }
}
