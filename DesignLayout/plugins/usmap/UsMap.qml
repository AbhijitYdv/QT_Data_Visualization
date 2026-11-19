import QtQuick
import DataScope.MapPlugin

// US map: one dot per filtered row at its latitude/longitude, drawn by the
// C++ UsMapView (QPainter, in this plugin). The lat/lon columns are found automatically and
// can be changed with the two pickers. Hover a dot to see its row, or a
// state to see its name.
// The dashboard sets `backend` (a MapBackend) when it loads this file.
Item {
    id: root
    anchors.fill: parent

    property QtObject backend

    function reload() {
        if (root.backend)
            root.backend.fillMap(map)
    }

    Row {
        id: pickers
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 8
        spacing: 12
        visible: root.backend && root.backend.numberColumns.length > 0

        ColumnPicker {
            width: 150
            label: qsTr("Latitude")
            options: root.backend ? root.backend.numberColumns : []
            current: root.backend ? root.backend.latColumn : ""
            onPicked: function(value) { root.backend.latColumn = value }
        }

        ColumnPicker {
            width: 150
            label: qsTr("Longitude")
            options: root.backend ? root.backend.numberColumns : []
            current: root.backend ? root.backend.lonColumn : ""
            onPicked: function(value) { root.backend.lonColumn = value }
        }
    }

    // How many rows are on the map, and why any are not.
    Text {
        anchors.right: parent.right
        anchors.bottom: pickers.bottom
        anchors.rightMargin: 12
        anchors.bottomMargin: 6
        visible: root.backend && root.backend.hasCoordinates
                 && map.pointCount + root.backend.outside > 0
        text: !root.backend ? ""
              : qsTr("%1 on the map").arg(map.pointCount)
                + (root.backend.outside > 0
                   ? qsTr(" · %1 outside the lower 48").arg(root.backend.outside) : "")
                + (root.backend.missing > 0
                   ? qsTr(" · %1 without coordinates").arg(root.backend.missing) : "")
        color: "#8a9399"
        font.pixelSize: 11
    }

    UsMapView {
        id: map
        anchors.top: pickers.visible ? pickers.bottom : parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 4
    }

    // Tooltip: the hovered row, or the hovered state's name
    Rectangle {
        id: tip
        readonly property string body: map.hoveredRow >= 0 && root.backend
                                        ? root.backend.rowSummary(map.hoveredRow)
                                        : map.hoveredState
        visible: body !== ""
        x: Math.min(map.x + map.hoverPosition.x + 14, root.width - width - 4)
        y: Math.min(map.y + map.hoverPosition.y + 14, root.height - height - 4)
        width: tipText.implicitWidth + 16
        height: tipText.implicitHeight + 10
        radius: 4
        color: "#22292d"
        border.color: "#2f383d"

        Text {
            id: tipText
            anchors.centerIn: parent
            text: tip.body
            color: "#e6e8ea"
            font.pixelSize: 11
            lineHeight: 1.2
        }
    }

    Text {
        anchors.centerIn: parent
        visible: !root.backend || !root.backend.hasCoordinates
        text: !root.backend || root.backend.rowCount === 0
              ? qsTr("Load a dataset with latitude and longitude")
              : qsTr("No latitude/longitude columns found.\nPick them above if they have other names.")
        horizontalAlignment: Text.AlignHCenter
        color: "#5a6369"
        font.pixelSize: 13
    }

    Connections {
        target: root.backend
        function onChanged() { root.reload() }
    }

    onBackendChanged: reload()
}
