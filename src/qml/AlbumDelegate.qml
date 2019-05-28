import QtQuick 2.2

import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

Rectangle {
    id: root
    width: 200; height: 200
    Image {
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        id: img
        width: 160; height: 160
        asynchronous: true
        source: model.display.cover
    }

    Rectangle {
        anchors.top: img.bottom
        width: 200; height: 40
        Text {
            id: title
            width: 200
            font.pointSize: 7
            horizontalAlignment: Text.AlignHCenter
            text: model.display.title
            wrapMode: Text.WordWrap
        }
    }

    MouseArea {
        id: ma
        hoverEnabled: true
        anchors.fill: parent
    }
    ToolTip.text: model.display.id
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.visible: ma.containsMouse ? true : false
}
