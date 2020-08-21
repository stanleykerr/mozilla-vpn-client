import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

RowLayout {
    property alias text: item.text
    signal clicked

    id: root
    width: parent.width
    Layout.fillWidth: true

    Layout.leftMargin: 16
    Layout.rightMargin: 16

    Label {
        id: item
    }

    Item {
        Layout.fillWidth: true
    }

    Image {
        height: 16
        width: 16
        Layout.alignment: Qt.AlignTop
        source: "../resources/delete.svg" // TODO

        MouseArea {
            id: iconMouseArea
            anchors.fill: parent
            onClicked: root.clicked()
        }
    }
}