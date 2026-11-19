import QtQuick
import QtCharts

// Scatter plot: one dot per row (X against Y), no grid. Large data is an
// even sample of 5000 points. Uses a date axis when X holds dates.
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
        gridVisible: false
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
        gridVisible: false
        color: "#2a3034"
    }

    ValueAxis {
        id: axisY
        min: chart.backend ? chart.backend.minY : 0
        max: chart.backend ? chart.backend.maxY : 1
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
        visible: !chart.isDate
        color: "#c0874f"
        borderColor: "transparent"
        markerSize: 6
    }

    ScatterSeries {
        id: dateSeries
        axisX: dateAxisX
        axisY: axisY
        visible: chart.isDate
        color: "#c0874f"
        borderColor: "transparent"
        markerSize: 6
    }

    Text {
        anchors.centerIn: parent
        visible: !chart.backend || chart.backend.pointCount === 0
        text: qsTr("Load a dataset to see this chart")
        color: "#5a6369"
        font.pixelSize: 13
    }

    function reload() {
        valueSeries.clear()
        dateSeries.clear()
        // Read xIsDate from the backend, not `isDate`: this can run before
        // that binding has caught up with the same `changed` signal.
        if (backend) // one C++ call fills the series
            backend.fillSeries(backend.xIsDate ? dateSeries : valueSeries)
    }

    Connections {
        target: chart.backend
        function onChanged() { chart.reload() }
    }

    onBackendChanged: reload()
}
