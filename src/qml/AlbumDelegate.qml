import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Rectangle {
    id: root
    width: 340; height: 410
    radius: 4
    border.color: "transparent"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        Rectangle {
            Layout.topMargin: 4
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 320
            Layout.preferredHeight: 320
            Image {
                anchors.fill: parent

                visible: opacity === 0 ? false : true

                Behavior on opacity {
                    NumberAnimation { duration: 300 }
                }

                fillMode: Image.PreserveAspectFit
                sourceSize.height: 320
                sourceSize.width: 320

                id: img_default
                asynchronous: true
                source: "qrc:///artworks/cd.svg"
            }

            Image {
                anchors.fill: parent

                Behavior on opacity {
                    NumberAnimation { duration: 300 }
                }

                opacity: 0
                fillMode: Image.PreserveAspectFit
                width: 320
                height: 320
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
        }


        Label {
            Layout.preferredWidth: 320
            Layout.fillHeight: true
            id: title
            maximumLineCount: 2
            horizontalAlignment: Text.AlignHCenter
            text: model.display.title
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
        }

        Label {
            maximumLineCount: 1
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 320
            Layout.preferredHeight: 22
            id: date
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignTop
            text: model.display.date.toLocaleString(Qt.locale(), "yyyy") + (model.display.hires ? "-HiRes" : "")
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
        }
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
