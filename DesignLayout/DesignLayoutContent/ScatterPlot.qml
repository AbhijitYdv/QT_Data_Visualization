import QtQuick
import QtCharts

// Scatter plot: one dot per row (X against Y), no grid.
// Uses a date axis when the X column holds dates.
// Gets every point, or an even sample of 5000 for large files
// (graphController.fillScatterSeries), unlike the line chart's min/max thinning.
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
        gridVisible: false
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
        gridVisible: false
        color: "#2a3034"
    }

    ValueAxis {
        id: axisY
        min: graphController.minY
        max: graphController.maxY
        tickCount: 6
        labelFormat: "%g"
        labelsColor: "#8a9399"
        labelsFont.pixelSize: 10
        gridVisible: false
        color: "#2a3034"
    }

    ScatterSeries {
        id: valueSeries
        axisX: axisX
        axisY: axisY
        visible: !graphController.xIsDate
        color: "#c0874f"
        borderColor: "transparent"
        markerSize: 6
    }

    ScatterSeries {
        id: dateSeries
        axisX: dateAxisX
        axisY: axisY
        visible: graphController.xIsDate
        color: "#c0874f"
        borderColor: "transparent"
        markerSize: 6
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
        valueSeries.clear()
        dateSeries.clear()
        // One C++ call: all points, or an even sample of 5000.
        graphController.fillScatterSeries(graphController.xIsDate ? dateSeries : valueSeries, 5000)
    }

    Connections {
        target: graphController
        function onDataLoaded() { chart.reload() }
        function onGraphCleared() { chart.reload() }
    }

    Component.onCompleted: reload()
}
