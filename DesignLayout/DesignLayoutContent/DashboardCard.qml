import QtQuick

// Dark card with a title row (name on the left, plugin tag on the right).
// Anything placed inside the card fills the area below the title row.
Rectangle {
    id: card
    property string title
    property string tag
    default property alias content: body.data

    color: "#1b2023"
    border.color: "#2a3034"
    border.width: 1
    radius: 4

    Item {
        id: headerRow
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.verticalCenter: parent.verticalCenter
            text: card.title
            color: "#e6e8ea"
            font.pixelSize: 13
        }

        Text {
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.verticalCenter: parent.verticalCenter
            text: card.tag
            color: "#8a9399"
            font.pixelSize: 11
            font.family: Qt.platform.os === "osx" ? "Menlo"
                       : Qt.platform.os === "windows" ? "Consolas"
                       : "DejaVu Sans Mono"
        }
    }

    Rectangle {
        id: divider
        anchors.top: headerRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: "#2a3034"
    }

    Item {
        id: body
        anchors.top: divider.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 6
        clip: true
    }
}
