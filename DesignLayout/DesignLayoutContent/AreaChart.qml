import QtQuick
import QtCharts

// Area chart: the same X/Y data as the line chart, filled down to the
// bottom of the plot. Uses a date axis when the X column holds dates.
ChartView {
    id: chart
    anchors.fill: parent

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
        visible: !graphController.xIsDate
        min: graphController.minX
        max: graphController.maxX
        tickCount: 6
        labelFormat: "%g"
        labelsColor: "#8a9399"
        labelsFont.pixelSize: 10
        gridLineColor: "#262c30"
        color: "#2a3034"
    }

    DateTimeAxis {
        id: dateAxisX
        visible: graphController.xIsDate
        min: new Date(graphController.minX)
        max: new Date(graphController.maxX)
        format: "MMM d"
        tickCount: 6
        labelsColor: "#8a9399"
        labelsFont.pixelSize: 10
        gridLineColor: "#262c30"
        color: "#2a3034"
    }

    ValueAxis {
        id: axisY
        // Start at 0 so the filled area means something, unless the data
        // itself goes negative (minY is padded, so use the real minimum).
        min: graphController.summary.count > 0 && graphController.summary.min < 0
             ? graphController.minY : 0
        max: graphController.maxY
        tickCount: 6
        labelFormat: "%g"
        labelsColor: "#8a9399"
        labelsFont.pixelSize: 10
        gridLineColor: "#262c30"
        color: "#2a3034"
    }

    AreaSeries {
        id: valueArea
        axisX: axisX
        axisY: axisY
        visible: !graphController.xIsDate
        color: "#404f9a86"       // teal at 25% opacity
        borderColor: "#4f9a86"
        borderWidth: 2
        upperSeries: LineSeries { id: valueLine }
    }

    AreaSeries {
        id: dateArea
        axisX: dateAxisX
        axisY: axisY
        visible: graphController.xIsDate
        color: "#404f9a86"
        borderColor: "#4f9a86"
        borderWidth: 2
        upperSeries: LineSeries { id: dateLine }
    }

    Text {
        anchors.centerIn: parent
        visible: graphController.loading || graphController.pointCount === 0
        text: graphController.loading ? qsTr("Loading…")
                                      : qsTr("Load a dataset to see this chart")
        color: "#5a6369"
        font.pixelSize: 13
    }

    function reload() {
        valueLine.clear()
        dateLine.clear()
        // Same thinned points as the line chart, in one C++ call.
        graphController.fillSeries(graphController.xIsDate ? dateLine : valueLine)
    }

    Connections {
        target: graphController
        function onDataLoaded() { chart.reload() }
    }

    Component.onCompleted: reload()
}
