import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
    id: root

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
