import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
    id: root

    RowLayout {
        id: rowlayout
        anchors.fill: parent

        PTabBar {
            id: ptbar
            Layout.fillHeight: true
            Layout.preferredWidth: 50
//            Layout.Center:
        }

        SwipeView {
            id: swipeview
            currentIndex: ptbar.page_index
            orientation: Qt.Vertical
            Layout.fillWidth: true
            Layout.fillHeight: true

            Item {
                id: page1

                StackView {
                    id: stack
                    initialItem: searchPageComp
                    anchors.fill: parent
                }

                Component {
                    id: searchPageComp
                    SearchPage {
                        id: searchPage
                    }
                }

                Component {
                    id: detailPageComp
                    DetailPage {
                        id: detailPage
                    }
                }
            }

            Item {
                id: page2

                Text {
                    id: tmptext
                    text: qsTr("PlayerPage")
                }
            }
        }
    }


}
