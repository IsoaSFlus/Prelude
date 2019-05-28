import QtQuick 2.2

import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

Rectangle {
    id: root
    color: "transparent"
    implicitWidth: 1200
    implicitHeight: 720

    Component {
        id: albumDelegate
        AlbumDelegate {
        }
    }

    Header {
        id: header
        objectName: "header"
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: 70
    }

    GridView {
        id: gv
        objectName: "grid_view"
        //            signal cu_clicked(string color)
        clip: true
        model: album_model.model
        delegate: albumDelegate

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            topMargin: 10
        }
        cellWidth: 200
        cellHeight: 200

    }
}
