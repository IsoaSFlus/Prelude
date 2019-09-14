import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Rectangle {
    id: root
    color: "transparent"

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
        clip: true

    }

    GridView {
        id: gv
        objectName: "grid_view"
        clip: true
        model: album_model.model
        delegate: albumDelegate
        signal album_clicked(string upc, string title)


        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        cellWidth: 340
        cellHeight: 380

        function unsetBusy() {
            blur.opacity = 0;
            gv.enabled = true;
        }
        function handleTracksData(album_title, cover) {
            blur.opacity = 0;
            gv.enabled = true;
            stack.push(detailPageComp, {"album_title": album_title, "cover_url": cover});
        }
    }

    FastBlur {
        source: gv
        opacity: 0
        visible: opacity === 0 ? false : true
        id: blur
        anchors.fill: gv
        radius: 70
        transparentBorder: false
        layer.enabled: true
        layer.effect: HueSaturation {
            cached: true
            lightness: 0.6
        }

        Behavior on opacity {
            NumberAnimation { duration: 300 }
        }

        BusyIndicator {
            running: parent.visible
            width: parent.height * 0.15
            height: parent.height * 0.15
            anchors.centerIn: parent
        }
    }

}
