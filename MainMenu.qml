import QtQuick 2.9
import QtQml 2.8

Rectangle {
    anchors.fill: parent

    color: "#2f2f2f"

    Row {
        anchors.centerIn: parent

        spacing: 0.08 * window.maxDimension

        Rectangle {
            id: load_song_button

            width: 0.3 * window.maxDimension
            height: 0.3 * window.maxDimension
            radius: 0.05 * window.maxDimension

            color: "#2fcf64"

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    console.log("Pushing song loading screen")
                    stackview.push(song_loading_screen)
                }
            }

            Text {
                anchors.centerIn: parent
                text: "Load song"
                font.pixelSize: 0.1 * load_song_button.height
            }
        }

        Rectangle {
            id: record_button

            width: 0.3 * window.maxDimension
            height: 0.3 * window.maxDimension
            radius: 0.05 * window.maxDimension

            color: "#f16767"

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    console.log("Pushing recording screen")
                    stackview.push(recording_screen)
                }
            }

            Text {
                anchors.centerIn: parent
                text: "Record"
                font.pixelSize: 0.1 * record_button.height
            }
        }
    }

    Component.onCompleted: {
        anchors.fill = null
    }
}
