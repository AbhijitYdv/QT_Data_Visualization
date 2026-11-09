pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

// Every row that passes the filter, from graphController.tableModel
// (a QAbstractTableModel in C++). Click a column header to sort by it;
// click again to reverse. Numbers and dates are right-aligned.
// TableView only creates the cells on screen, so large files scroll fine.
Item {
    id: root
    anchors.fill: parent

    readonly property var model: graphController.tableModel

    HorizontalHeaderView {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        syncView: table
        clip: true

        delegate: Rectangle {
            id: headerCell
            required property int column
            required property string display

            implicitWidth: 120
            implicitHeight: 30
            color: headerMouse.containsMouse ? "#262d31" : "#22292d"

            Text {
                anchors.fill: parent
                leftPadding: 10
                rightPadding: 10
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: root.model.isNumericColumn(headerCell.column)
                                     ? Text.AlignRight : Text.AlignLeft
                text: (root.model.sortColumn === headerCell.column
                       ? (root.model.sortAscending ? "▲  " : "▼  ") : "")
                      + headerCell.display
                color: "#e6e8ea"
                font.pixelSize: 12
                font.bold: true
                elide: Text.ElideRight
            }

            MouseArea {
                id: headerMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: root.model.sortByColumn(headerCell.column)
            }
        }
    }

    TableView {
        id: table
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        model: root.model

        // Share the width between columns, but never narrower than 100 px.
        columnWidthProvider: function(column) {
            return Math.max(100, table.width / Math.max(1, table.columns))
        }
        onWidthChanged: forceLayout()

        delegate: Rectangle {
            id: cell
            required property int row
            required property string display
            required property bool numeric

            implicitHeight: 28
            color: row % 2 ? "#1e2427" : "transparent"

            Text {
                anchors.fill: parent
                leftPadding: 10
                rightPadding: 10
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: cell.numeric ? Text.AlignRight : Text.AlignLeft
                text: cell.display
                color: "#d0d5d8"
                font.pixelSize: 12
                elide: Text.ElideRight
            }
        }

        ScrollBar.vertical: ScrollBar { }
        ScrollBar.horizontal: ScrollBar { }
    }

    Text {
        anchors.centerIn: parent
        visible: graphController.loading || root.model.count === 0
        text: graphController.loading ? qsTr("Loading…")
              : graphController.rowCount === 0 ? qsTr("Load a dataset to see its rows")
              : qsTr("No rows match the filter")
        color: "#5a6369"
        font.pixelSize: 13
    }
}
