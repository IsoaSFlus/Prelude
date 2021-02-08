import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
    id: root
    property int page_index
    ColumnLayout {
        id: tablayout
        anchors.fill: parent
        PIconTabButton {
            source_svg: checked ? "qrc:///artworks/search_checked.svg" : "qrc:///artworks/search.svg"
            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: 40
            Layout.preferredWidth: 40
            checked: true
            onClicked: {
                root.page_index = 0
            }
        }
        PIconTabButton {
            source_svg: checked ? "qrc:///artworks/playing_checked.svg" : "qrc:///artworks/playing.svg"
            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: 40
            Layout.preferredWidth: 40
            onClicked: {
                root.page_index = 1
            }
        }
    }
}
