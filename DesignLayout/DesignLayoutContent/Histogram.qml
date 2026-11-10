import QtQuick
import QtQuick.Controls
import QtCharts

// Histogram of the Y column: how many rows fall in each value range.
// The bars are one outline drawn by C++ (fillHistogramSeries) on a real number
// axis, so the X labels are values, not one label per bar.
// The slider changes the number of bins; "Auto" goes back to the default.
Item {
    id: root
    anchors.fill: parent

    property var hist: ({ counts: [], edges: [], labels: [], maxCount: 0, bins: 0 })
    property string hoverText: ""

    // Whole-number Y axis for counts: the smallest step of 1, 2 or 5 x 10^k
    // that needs at most 5 steps, e.g. 3 -> 0..3 by 1, 23 -> 0..25 by 5.
    function countAxis(maxCount) {
        const target = Math.max(1, maxCount)
        for (let scale = 1; ; scale *= 10) {
            for (const m of [1, 2, 5]) {
                const step = m * scale
                const steps = Math.ceil(target / step)
                if (steps <= 5)
                    return { max: steps * step, ticks: steps + 1 }
            }
        }
    }
    readonly property var yAxis: countAxis(hist.maxCount)

    function reload() {
        root.hist = graphController.histogramData()
        graphController.fillHistogramSeries(outline)
        root.hoverText = ""
    }

    ChartView {
        id: chart
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: controls.top

        legend.visible: false
        antialiasing: true
        backgroundColor: "transparent"
        plotAreaColor: "transparent"
        margins.top: 4
        margins.bottom: 4
        margins.left: 4
        margins.right: 4

        ValueAxis {
            id: axisX
            min: root.hist.edges.length > 0 ? root.hist.edges[0] : 0
            max: root.hist.edges.length > 0 ? root.hist.edges[root.hist.edges.length - 1] : 1
            tickCount: 6
            labelFormat: "%g"
            labelsColor: "#8a9399"
            labelsFont.pixelSize: 10
            gridVisible: false
            color: "#2a3034"
        }

        ValueAxis {
            id: axisY
            min: 0
            max: root.yAxis.max
            tickCount: root.yAxis.ticks
            labelFormat: "%d"
            labelsColor: "#8a9399"
            labelsFont.pixelSize: 10
            gridLineColor: "#262c30"
            color: "#2a3034"
        }

        AreaSeries {
            id: bars
            axisX: axisX
            axisY: axisY
            color: "#b34f9a86"       // teal at 70% opacity
            borderColor: "#4f9a86"
            borderWidth: 1
            upperSeries: LineSeries { id: outline }

            onHovered: function(point, state) {
                if (!state) {
                    root.hoverText = ""
                    return
                }
                const e = root.hist.edges
                for (let i = 0; i + 1 < e.length; ++i) {
                    if (point.x >= e[i] && (point.x < e[i + 1] || i + 2 === e.length)) {
                        const n = root.hist.counts[i]
                        root.hoverText = qsTr("%1: %2 rows").arg(root.hist.labels[i]).arg(n)
                        return
                    }
                }
            }
        }

        Text {
            anchors.centerIn: parent
            visible: graphController.loading || root.hist.bins === 0
            text: graphController.loading ? qsTr("Loading…")
                                          : qsTr("Load a dataset to see this chart")
            color: "#5a6369"
            font.pixelSize: 13
        }
    }

    // Hovered bin, top right of the plot
    Rectangle {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 8
        visible: root.hoverText !== ""
        width: hoverLabel.implicitWidth + 16
        height: hoverLabel.implicitHeight + 10
        radius: 4
        color: "#22292d"
        border.color: "#2f383d"

        Text {
            id: hoverLabel
            anchors.centerIn: parent
            text: root.hoverText
            color: "#e6e8ea"
            font.pixelSize: 11
        }
    }

    Row {
        id: controls
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        height: 32
        spacing: 10
        visible: root.hist.bins > 0

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Bins")
            color: "#8a9399"
            font.pixelSize: 11
        }

        Slider {
            id: binSlider
            anchors.verticalCenter: parent.verticalCenter
            width: Math.max(80, controls.width - 160)
            from: 3
            to: 60
            stepSize: 1
            snapMode: Slider.SnapAlways
            value: graphController.histogramBins > 0 ? graphController.histogramBins
                                                     : graphController.histogramAutoBins
            onMoved: graphController.histogramBins = value
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.hist.bins
            color: "#e6e8ea"
            font.pixelSize: 11
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: graphController.histogramBins > 0 ? qsTr("Auto") : qsTr("(auto)")
            color: graphController.histogramBins > 0
                   ? (autoMouse.containsMouse ? "#e6e8ea" : "#4f9a86") : "#5a6369"
            font.pixelSize: 11

            MouseArea {
                id: autoMouse
                anchors.fill: parent
                enabled: graphController.histogramBins > 0
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: graphController.histogramBins = 0
            }
        }
    }

    Connections {
        target: graphController
        function onDataLoaded() { root.reload() }
        function onHistogramChanged() { root.reload() }
    }

    Component.onCompleted: reload()
}
