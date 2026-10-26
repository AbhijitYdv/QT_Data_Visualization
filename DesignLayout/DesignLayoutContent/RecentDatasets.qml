pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

// Sidebar "Recent" list: the last 10 datasets loaded this session.
// Click a row to bring that data back (replaces the current chart).
// The × on a row removes it; "Clear" empties the list.
// Hidden until something has been loaded.
Column {
    id: root
    width: 200
    spacing: 6
    visible: graphController.historyCount > 0

    readonly property int rowHeight: 40
    readonly property int maxVisibleRows: 4   // scroll for the rest

    Item {
        width: root.width
        height: 18

        Text {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Recent")
            color: "#8a9399"
            font.pixelSize: 13
        }

        Text {
            id: clearLink
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Clear")
            color: clearMouse.containsMouse ? "#e6e8ea" : "#5a6369"
            font.pixelSize: 11

            MouseArea {
                id: clearMouse
                anchors.fill: parent
                anchors.margins: -4
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: graphController.clearHistory()
            }
        }
    }

    ListView {
        id: list
        width: root.width
        height: Math.min(count, root.maxVisibleRows) * (root.rowHeight + spacing)
        spacing: 4
        clip: true
        interactive: count > root.maxVisibleRows
        boundsBehavior: Flickable.StopAtBounds
        model: graphController.history

        ScrollBar.vertical: ScrollBar {
            policy: list.count > root.maxVisibleRows ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
        }

        delegate: Rectangle {
            id: row
            required property int index
            required property string fileName
            required property string summary
            required property bool isCurrent

            width: ListView.view.width
            height: root.rowHeight
            radius: 4
            color: isCurrent ? "#22292d"
                             : (rowMouse.containsMouse ? "#1f2528" : "transparent")
            border.color: isCurrent ? "#2f383d" : "transparent"
            opacity: graphController.loading ? 0.5 : 1.0

            Rectangle {
                id: dot
                width: 6
                height: 6
                radius: 3
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: row.isCurrent ? "#4f9a86" : "#5a6369"
            }

            Column {
                anchors.left: dot.right
                anchors.leftMargin: 10
                anchors.right: removeButton.left
                anchors.rightMargin: 4
                anchors.verticalCenter: parent.verticalCenter
                spacing: 2

                Text {
                    width: parent.width
                    elide: Text.ElideMiddle
                    text: row.fileName
                    color: row.isCurrent ? "#e6e8ea" : "#aab2b8"
                    font.pixelSize: 12
                }

                Text {
                    width: parent.width
                    elide: Text.ElideRight
                    text: row.summary
                    color: "#5a6369"
                    font.pixelSize: 10
                }
            }

            // Listed before the remove button so the button's MouseArea is on top.
            MouseArea {
                id: rowMouse
                anchors.fill: parent
                hoverEnabled: true
                enabled: !graphController.loading
                cursorShape: Qt.PointingHandCursor
                onClicked: graphController.openFromHistory(row.index)
            }

            Text {
                id: removeButton
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                text: "×"
                visible: rowMouse.containsMouse || removeMouse.containsMouse
                color: removeMouse.containsMouse ? "#e6e8ea" : "#8a9399"
                font.pixelSize: 14

                MouseArea {
                    id: removeMouse
                    anchors.fill: parent
                    anchors.margins: -6
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: graphController.removeFromHistory(row.index)
                }
            }
        }
    }
}
