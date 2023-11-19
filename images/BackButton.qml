import QtQuick 2.9

Item {

    width: image.width + 20
    height: image.height + 20

    Image {
        anchors.centerIn: parent

        id: image

        source: "images/buttons/back/white.png"

        //        x: 10
        //        y: x
        width: 50
        height: width

        MouseArea {
            anchors.fill: parent

            onClicked: {
                stackview.pop()
            }
        }
    }
}
