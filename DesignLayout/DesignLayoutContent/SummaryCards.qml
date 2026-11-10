pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

// A row of number tiles for the Y column: rows shown, min, max, average, median.
// Everything comes from graphController.summary, so the tiles follow the
// Y picker and the filter on their own.
RowLayout {
    id: root
    spacing: 16

    readonly property var stats: graphController.summary
    readonly property bool hasData: stats.count > 0
    readonly property string column: stats.column ? stats.column : ""

    // 1234567 -> "1,234,567"; 0.000123 -> "1.23e-4"; keeps about 5 significant digits.
    function fmt(v) {
        if (!root.hasData || !isFinite(v))
            return "–"
        const a = Math.abs(v)
        if (a !== 0 && (a >= 1e9 || a < 1e-3))
            return v.toExponential(2)
        const rounded = Number(v.toPrecision(5))
        const decimals = Math.min(4, (String(rounded).split(".")[1] || "").length)
        return rounded.toLocaleString(Qt.locale(), "f", decimals)
    }

    Repeater {
        model: [
            { label: qsTr("Rows shown"),
              value: graphController.tableModel.count.toLocaleString(Qt.locale(), "f", 0) },
            { label: qsTr("Min · %1").arg(root.column), value: root.fmt(root.stats.min) },
            { label: qsTr("Max · %1").arg(root.column), value: root.fmt(root.stats.max) },
            { label: qsTr("Average · %1").arg(root.column), value: root.fmt(root.stats.mean) },
            { label: qsTr("Median · %1").arg(root.column), value: root.fmt(root.stats.median) }
        ]

        delegate: Rectangle {
            id: tile
            required property var modelData

            Layout.fillWidth: true
            Layout.preferredWidth: 100
            Layout.preferredHeight: 72
            color: "#1b2023"
            border.color: "#2a3034"
            radius: 4

            Column {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 14
                anchors.rightMargin: 14
                anchors.verticalCenter: parent.verticalCenter
                spacing: 6

                Text {
                    width: parent.width
                    text: tile.modelData.label
                    color: "#8a9399"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }

                Text {
                    width: parent.width
                    text: tile.modelData.value
                    color: "#e6e8ea"
                    font.pixelSize: 20
                    elide: Text.ElideRight
                }
            }
        }
    }
}
