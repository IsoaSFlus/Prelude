import QtQuick 2.2

import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

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
