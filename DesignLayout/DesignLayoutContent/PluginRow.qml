import QtQuick

// One selectable row in the sidebar "Plugins" list. Click to toggle.
// While `locked` is true the row can be turned off but not on (used for the
// limit on how many charts the dashboard shows at once).
Rectangle {
    id: root
    property string text
    property bool checked: false
    property bool locked: false

    readonly property bool blocked: locked && !checked
    opacity: blocked ? 0.45 : 1

    width: 200
    height: 36
    radius: 4
    color: checked ? "#22292d" : (mouse.containsMouse ? "#1f2528" : "transparent")
    border.color: checked ? "#2f383d" : "transparent"

    Rectangle {
        id: dot
        width: 6
        height: 6
        radius: 3
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        color: root.checked ? "#4f9a86" : "#5a6369"
    }

    Text {
        anchors.left: dot.right
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        text: root.text
        color: root.checked ? "#e6e8ea" : "#aab2b8"
        font.pixelSize: 13
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: root.blocked ? Qt.ForbiddenCursor : Qt.PointingHandCursor
        onClicked: {
            if (!root.blocked)
                root.checked = !root.checked
        }
    }
}
