import QtQuick
import QtCharts

// Bar chart view, shipped inside the bar chart plugin.
// The dashboard sets `backend` (a BarBackend) when it loads this file.
ChartView {
    id: chart
    property QtObject backend

    anchors.fill: parent
    legend.visible: false
    antialiasing: true
    backgroundColor: "transparent"
    plotAreaColor: "transparent"
    margins.top: 4
    margins.bottom: 4
    margins.left: 4
    margins.right: 4

    BarSeries {
        id: series
        barWidth: 0.8

        axisX: BarCategoryAxis {
            id: axisX
            categories: chart.backend ? chart.backend.labels : []
            labelsColor: "#8a9399"
            labelsFont.pixelSize: 10
            gridVisible: false
            color: "#2a3034"
        }

        axisY: ValueAxis {
            // Bars start from 0 unless the data goes negative (minY/maxY are padded).
            min: chart.backend ? Math.min(0, chart.backend.minY) : 0
            max: chart.backend ? Math.max(0, chart.backend.maxY) : 1
            tickCount: 6
            labelFormat: "%g"
            labelsColor: "#8a9399"
            labelsFont.pixelSize: 10
            gridLineColor: "#262c30"
            color: "#2a3034"
        }

        BarSet {
            label: qsTr("Value")
            color: "#4f9a86"
            borderColor: "#4f9a86"
            values: chart.backend ? chart.backend.values : []
        }
    }

    Text {
        anchors.centerIn: parent
        visible: !chart.backend || chart.backend.pointCount === 0
        text: qsTr("Load a dataset to see this chart")
        color: "#5a6369"
        font.pixelSize: 13
    }
}
