import QtQuick
import QtCharts

// Line chart: teal line with a marker on every point, dark theme.
// Uses a date axis when the X column holds dates.
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
        min: graphController.minY
        max: graphController.maxY
        tickCount: 6
        labelFormat: "%g"
        labelsColor: "#8a9399"
        labelsFont.pixelSize: 10
        gridLineColor: "#262c30"
        color: "#2a3034"
    }

    LineSeries {
        id: valueSeries
        axisX: axisX
        axisY: axisY
        visible: !graphController.xIsDate
        color: "#4f9a86"
        width: 2
        pointsVisible: true
    }

    LineSeries {
        id: dateSeries
        axisX: dateAxisX
        axisY: axisY
        visible: graphController.xIsDate
        color: "#4f9a86"
        width: 2
        pointsVisible: true
    }

    Text {
        anchors.centerIn: parent
        visible: graphController.pointCount === 0
        text: qsTr("Load a dataset to see this chart")
        color: "#5a6369"
        font.pixelSize: 13
    }

    function reload() {
        valueSeries.clear()
        dateSeries.clear()
        const target = graphController.xIsDate ? dateSeries : valueSeries
        const pts = graphController.points()
        for (let i = 0; i < pts.length; ++i)
            target.append(pts[i].x, pts[i].y)
    }

    Connections {
        target: graphController
        function onDataLoaded() { chart.reload() }
    }

    Component.onCompleted: reload()
}
