import QtQuick 2.9
import QtQml 2.8

Item {
    id: audioDevice_selector

    height: listview.height
    width: parent.width

    ListView {
        id: listview

        width: parent.width

        height: contentHeight

        x: 30

        //        model: mediaDevices.audioInputs
        model: transcription.audioInputDevices

        delegate: audioDeviceDelegate

        highlightFollowsCurrentItem: false

        Component.onCompleted: {
            //            transcription.audioInputDevice = model[0]
            currentIndex = 0
            highlighter.y = listview.currentItem.y
        }
    }

    function selectAudioDevice(modelData) {}

    Component {
        id: audioDeviceDelegate

        Item {
            width: parent.width
            height: 30

            Text {
                color: "#ffffff"

                text: modelData

                font.pixelSize: 10
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    listview.currentIndex = index
                    highlighter.y = listview.currentItem.y

                    //                    transcription.audioInputDevice = modelData
                    transcription.setAudioInputDeviceFromName(modelData)
                }
            }
        }
    }
    Rectangle {
        id: highlighter

        x: 10

        width: 10
        height: 10
        radius: 5

        color: "white"

        Behavior on y {
            NumberAnimation {
                duration: 100
                easing.type: Easing.InOutQuad
            }
        }
    }
}
