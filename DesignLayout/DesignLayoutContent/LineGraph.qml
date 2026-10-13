import QtQuick
import QtCharts

ChartView {
    id: chart
    anchors.fill: parent

    title: "Line Chart"
    legend.visible: false
    antialiasing: true

    ValueAxis {
        id: axisX
        min: 0
        max: 15
    }

    ValueAxis {
        id: axisY
        min: 0
        max: 15
    }

    LineSeries {
        id: graphLine
        axisX: axisX
        axisY: axisY
    }

    Connections {
        target: graphController

        function onPointAdded(x, y) {
            graphLine.append(x, y)
        }

        function onGraphCleared() {
            graphLine.clear()
        }
    }

    Component.onCompleted: {
        graphController.loadDefaultPoints()
    }
}