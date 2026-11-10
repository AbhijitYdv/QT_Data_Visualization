import QtQuick
import QtCharts

// Donut chart: share of rows (or of the Y total) per category of a text
// column. The 7 largest groups get a slice; the rest are merged into "Other".
// When it groups by the filter column it shows every group and pulls out
// the filtered one.
Item {
    id: root
    anchors.fill: parent

    readonly property var palette: ["#4f9a86", "#c0874f", "#5b8cc4", "#b46aa8",
                                    "#c9b458", "#6aa86f", "#c4655b"]
    readonly property color otherColor: "#4a5358"
    property real total: 0
    property string valueName: ""

    function fmt(v) {
        return Number(v.toPrecision(5)).toLocaleString(Qt.locale(), "f",
                                                       Number.isInteger(v) ? 0 : 2)
    }

    function reload() {
        pie.clear()
        const d = graphController.pieData()
        let total = 0
        for (let i = 0; i < d.values.length; ++i)
            total += d.values[i]

        for (let i = 0; i < d.labels.length; ++i) {
            const share = total > 0 ? Math.round(d.values[i] / total * 100) : 0
            const slice = pie.append(qsTr("%1  %2%").arg(d.labels[i]).arg(share), d.values[i])
            const isOther = i === d.labels.length - 1 && d.labels.length > 7
            slice.color = isOther ? root.otherColor : root.palette[i % root.palette.length]
            slice.borderColor = "#1b2023"
            slice.borderWidth = 2
            slice.exploded = i === d.highlight
            slice.explodeDistanceFactor = 0.12
        }
        root.total = total
        root.valueName = d.valueName
    }

    Row {
        id: pickers
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 8
        spacing: 12
        visible: graphController.pieGroupOptions.length > 0

        ColumnPicker {
            width: Math.min(180, (pickers.width - 12) / 2)
            label: qsTr("Group by")
            options: graphController.pieGroupOptions
            current: graphController.pieGroupColumn
            onPicked: function(value) { graphController.pieGroupColumn = value }
        }

        ColumnPicker {
            id: sizePicker
            width: Math.min(180, (pickers.width - 12) / 2)
            label: qsTr("Slice size")
            options: graphController.yColumnName !== ""
                     ? [qsTr("Row count"), qsTr("Sum of %1").arg(graphController.yColumnName)]
                     : [qsTr("Row count")]
            current: graphController.pieSumsY ? options[options.length - 1] : options[0]
            onPicked: function(value) { graphController.pieSumsY = value !== options[0] }
        }
    }

    ChartView {
        id: chart
        anchors.top: pickers.visible ? pickers.bottom : parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        antialiasing: true
        backgroundColor: "transparent"
        plotAreaColor: "transparent"
        margins.top: 0
        margins.bottom: 0
        margins.left: 0
        margins.right: 0
        legend.alignment: Qt.AlignRight
        legend.labelColor: "#aab2b8"
        legend.font.pixelSize: 11
        legend.visible: pie.count > 0

        PieSeries {
            id: pie
            size: 0.85
            holeSize: 0.5
        }
    }

    // Total in the middle of the donut
    Column {
        x: chart.x + chart.plotArea.x + chart.plotArea.width / 2 - width / 2
        y: chart.y + chart.plotArea.y + chart.plotArea.height / 2 - height / 2
        visible: pie.count > 0
        spacing: 2

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.fmt(root.total)
            color: "#e6e8ea"
            font.pixelSize: 16
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.valueName
            color: "#8a9399"
            font.pixelSize: 10
        }
    }

    Text {
        anchors.centerIn: parent
        visible: graphController.loading || pie.count === 0
        text: graphController.loading ? qsTr("Loading…")
              : graphController.rowCount === 0 ? qsTr("Load a dataset to see this chart")
              : graphController.pieGroupOptions.length === 0 ? qsTr("No text column to group by")
              : qsTr("Nothing to show for this column")
        color: "#5a6369"
        font.pixelSize: 13
    }

    Connections {
        target: graphController
        function onDataLoaded() { root.reload() }
        function onPieChanged() { root.reload() }
    }

    Component.onCompleted: reload()
}
