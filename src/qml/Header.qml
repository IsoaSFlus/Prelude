import QtQuick 2.2

import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0


Rectangle {
    id: root
    height: 70
    signal search_clicked(string keywords)


    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 10

        TextField {
            id: tf
            Layout.minimumWidth: 50
            Layout.preferredWidth: 350
//            Layout.alignment: Qt.AlignLeft
            placeholderText: qsTr("Search here!")

            Keys.onReturnPressed: {
                console.log('Search.');
                search_clicked(tf.text);
                blur.opacity = 1;
            }
        }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

        }

        Button {
            text: "Search"
            onClicked: {
                search_clicked(tf.text);
                blur.opacity = 1;
            }
        }
    }
}
