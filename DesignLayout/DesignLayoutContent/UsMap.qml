import QtQuick
import DataScope.Maps

// US map: one dot per filtered row at its latitude/longitude, drawn by the
// C++ UsMapItem (QPainter). The lat/lon columns are found automatically and
// can be changed with the two pickers. Hover a dot to see its row, or a
// state to see its name.
Item {
    id: root
    anchors.fill: parent

    function reload() {
        graphController.fillMap(map)
    }

    Row {
        id: pickers
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 8
        spacing: 12
        visible: graphController.yOptions.length > 0

        ColumnPicker {
            width: 150
            label: qsTr("Latitude")
            options: graphController.yOptions
            current: graphController.mapLatColumn
            onPicked: function(value) { graphController.mapLatColumn = value }
        }

        ColumnPicker {
            width: 150
            label: qsTr("Longitude")
            options: graphController.yOptions
            current: graphController.mapLonColumn
            onPicked: function(value) { graphController.mapLonColumn = value }
        }
    }

    // How many rows are on the map, and why any are not.
    Text {
        anchors.right: parent.right
        anchors.bottom: pickers.bottom
        anchors.rightMargin: 12
        anchors.bottomMargin: 6
        visible: graphController.mapHasCoordinates && map.pointCount + graphController.mapOutside > 0
        text: qsTr("%1 on the map").arg(map.pointCount)
              + (graphController.mapOutside > 0
                 ? qsTr(" · %1 outside the lower 48").arg(graphController.mapOutside) : "")
              + (graphController.mapMissing > 0
                 ? qsTr(" · %1 without coordinates").arg(graphController.mapMissing) : "")
        color: "#8a9399"
        font.pixelSize: 11
    }

    UsMapItem {
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
        readonly property string body: map.hoveredRow >= 0
                                        ? graphController.rowSummary(map.hoveredRow)
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
        visible: graphController.loading || !graphController.mapHasCoordinates
        text: graphController.loading ? qsTr("Loading…")
              : graphController.rowCount === 0 ? qsTr("Load a dataset with latitude and longitude")
              : qsTr("No latitude/longitude columns found.\nPick them above if they have other names.")
        horizontalAlignment: Text.AlignHCenter
        color: "#5a6369"
        font.pixelSize: 13
    }

    Connections {
        target: graphController
        function onDataLoaded() { root.reload() }
        function onMapChanged() { root.reload() }
    }

    Component.onCompleted: reload()
}
