import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
    id: root
    property string album_title: ""
    property string cover_url: ""
    property bool back_ready: false
    property bool fore_ready: false

    signal addAllTracksClicked(int index)

    Item {
        id: background
        anchors.fill: parent

        Image {
            id: defaultBackground

            source: "qrc:///artworks/default_cover.jpg"
            visible: opacity === 0 ? false : true
            asynchronous: true

            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop

            sourceSize.width: parent.width

            opacity: back_ready && fore_ready ? 0 : 1
            Behavior on opacity {
                NumberAnimation { duration: 300 }
            }
            layer.enabled: true
            layer.effect: HueSaturation {
                cached: true

                lightness: -0.5
                saturation: 0.9

                layer.enabled: true
                layer.effect: GaussianBlur {
                    cached: true

                    radius: 256
                    deviation: 12
                    samples: 129

                    transparentBorder: false
                }
            }
        }

        Image {
            id: newBackground

            source: cover_url

            asynchronous: true

            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop

            sourceSize.width: parent.width

            opacity: back_ready && fore_ready ? 1 : 0
            Behavior on opacity {
                NumberAnimation { duration: 300 }
            }
            layer.enabled: true
            layer.effect: HueSaturation {
                cached: true

                lightness: -0.5
                saturation: 0.9

                layer.enabled: true
                layer.effect: GaussianBlur {
                    cached: true

                    radius: 256
                    deviation: 12
                    samples: 129

                    transparentBorder: false
                }
            }
            onStatusChanged: {
                if (newBackground.status === Image.Ready) {
                    back_ready = true;
                } else {
                    back_ready = false;
                }
            }
        }
    }

    Action {
        id: back_action
        text: "Back"
        icon.source: "qrc:///artworks/back.svg"
        onTriggered: {
            stack.pop();
        }
    }

    IconButton {
        id: back_button
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.height * 0.07
        height: parent.height * 0.07
        action: back_action
    }

    RowLayout {
        id: content
        anchors.fill: parent

        Item {
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredHeight: root.height * 0.6
            Layout.preferredWidth: root.height * 0.6
            Layout.leftMargin: root.width * 0.05
            Layout.rightMargin: root.width * 0.05
            //            Layout.minimumWidth: parent.height * 0.5
            //            Layout.maximumWidth: parent.height * 0.5

            Image {
                id: cover_default
                anchors.fill: parent
                visible: opacity === 0 ? false : true
                opacity: back_ready && fore_ready ? 0 : 1
                Behavior on opacity {
                    NumberAnimation { duration: 300 }
                }

                fillMode: Image.PreserveAspectFit
                asynchronous: true
                source: "qrc:///artworks/cd_large.svg"
            }

            Image {
                id: cover
                anchors.fill: parent
                opacity: back_ready && fore_ready ? 1 : 0
                Behavior on opacity {
                    NumberAnimation { duration: 300 }
                }
                asynchronous: true
                source: cover_url
                fillMode: Image.PreserveAspectFit

                onStatusChanged: {
                    if (cover.status === Image.Ready) {
                        fore_ready = true;
                    } else {
                        fore_ready = false;
                    }
                }

                Rectangle {
                    id: cover_hover_layer
                    width: parent.width
                    height: parent.height
                    color: "black"
                    opacity: 0
                    Behavior on opacity {
                        NumberAnimation { duration: 300 }
                    }
                    MouseArea {
                        id: ma
                        hoverEnabled: true
                        anchors.fill: parent
                        onEntered: {
                            parent.opacity = 0.6;
                        }
                        onExited: {
                            parent.opacity = 0;
                        }
                        Action {
                            id: add_all_action
                            text: "Add all tracks"
                            icon.source: "qrc:///artworks/add_small.svg"
                            onTriggered: {
                                album_model.addTracksToMPD(-1);
                            }
                        }
                        IconButton {
                            id: add_all_button
                            width: parent.height * 0.2
                            height: parent.height * 0.2
                            action: add_all_action
                            anchors.centerIn: parent
                        }
                    }
                }
            }
        }

        ColumnLayout {
            Layout.preferredHeight: root.height * 0.7
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.rightMargin: root.width * 0.05
            Label {
                id: title
                text: album_title
                color: "white"
                font.pointSize: 16
                font.bold: true
                Layout.preferredHeight: root.height * 0.1
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
            }

            ListView {
                Layout.fillWidth: true
                Layout.preferredHeight: root.height * 0.6
                clip: true
                model: album_model.track_model
                delegate: Label {
                    width: parent.width
                    text: model.display.title
                    color: "white"
                    elide: Text.ElideRight
                }
            }
        }
    }
}
