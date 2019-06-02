import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Rectangle {
    id: root
    width: 340; height: 360
    radius: 4
    border.color: "transparent"
    border.width: 1

    Image {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top:parent.top
            topMargin: 4
        }

        visible: opacity === 0 ? false : true

        Behavior on opacity {
            NumberAnimation { duration: 300 }
        }

        fillMode: Image.PreserveAspectFit
        sourceSize.height: 300
        sourceSize.width: 300

        id: img_default
        asynchronous: true
        source: "qrc:///artworks/cd.svg"
    }

    Image {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top:parent.top
            topMargin: 4
        }

        Behavior on opacity {
            NumberAnimation { duration: 300 }
        }

        opacity: 0
        fillMode: Image.PreserveAspectFit
        width: 300
        height: 300
        id: img
        asynchronous: true
        source: model.display.cover
        onStatusChanged: {
            if (img.status === Image.Ready) {
                img.opacity = 1;
                img_default.opacity = 0;
            } else {
                img.opacity = 0;
                img_default.opacity = 1;
            }
        }
    }

    Label {
        anchors {
            top: img.bottom
            horizontalCenter: parent.horizontalCenter
        }
        id: title
        width: 320
        height: 55
        horizontalAlignment: Text.AlignHCenter
        text: model.display.title
        wrapMode: Text.WordWrap
        elide: Text.ElideRight
    }

    MouseArea {
        id: ma
        hoverEnabled: true
        anchors.fill: parent

        onEntered: {
            root.border.color = "#409eff"
        }
        onExited: {
            root.border.color = "transparent"
        }

        onClicked: {
            blur.opacity = 1;
            gv.enabled = false;
            gv.album_clicked(model.display.upc, model.display.title);

        }
    }
}
