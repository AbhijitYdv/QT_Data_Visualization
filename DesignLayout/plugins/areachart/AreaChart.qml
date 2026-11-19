import QtQuick
import QtCharts

// Area chart: the same X/Y data as the line chart, filled down to the
// bottom of the plot. Uses a date axis when the X column holds dates.
// The dashboard sets `backend` (a SeriesBackend) when it loads this file.
ChartView {
    id: chart
    property QtObject backend
    readonly property bool isDate: backend ? backend.xIsDate : false

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
        visible: !chart.isDate
        min: chart.backend ? chart.backend.minX : 0
        max: chart.backend ? chart.backend.maxX : 1
        tickCount: 6
        labelFormat: "%g"
        labelsColor: "#8a9399"
        labelsFont.pixelSize: 10
        gridLineColor: "#262c30"
        color: "#2a3034"
    }

    DateTimeAxis {
        id: dateAxisX
        visible: chart.isDate
        min: new Date(chart.backend ? chart.backend.minX : 0)
        max: new Date(chart.backend ? chart.backend.maxX : 1)
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
        // itself goes negative.
        min: chart.backend && chart.backend.pointCount > 0 && chart.backend.dataMinY < 0
             ? chart.backend.minY : 0
        max: chart.backend ? chart.backend.maxY : 1
        tickCount: 6
        labelFormat: "%g"
        labelsColor: "#8a9399"
        labelsFont.pixelSize: 10
        gridLineColor: "#262c30"
        color: "#2a3034"
    }

    AreaSeries {
        axisX: axisX
        axisY: axisY
        visible: !chart.isDate
        color: "#404f9a86"       // teal at 25% opacity
        borderColor: "#4f9a86"
        borderWidth: 2
        upperSeries: LineSeries { id: valueLine }
    }

    AreaSeries {
        axisX: dateAxisX
        axisY: axisY
        visible: chart.isDate
        color: "#404f9a86"
        borderColor: "#4f9a86"
        borderWidth: 2
        upperSeries: LineSeries { id: dateLine }
    }

    Text {
        anchors.centerIn: parent
        visible: !chart.backend || chart.backend.pointCount === 0
        text: qsTr("Load a dataset to see this chart")
        color: "#5a6369"
        font.pixelSize: 13
    }

    function reload() {
        valueLine.clear()
        dateLine.clear()
        // Read xIsDate from the backend, not `isDate`: this can run before
        // that binding has caught up with the same `changed` signal.
        if (backend) // one C++ call fills the series
            backend.fillSeries(backend.xIsDate ? dateLine : valueLine)
    }

    Connections {
        target: chart.backend
        function onChanged() { chart.reload() }
    }

    onBackendChanged: reload()
}
