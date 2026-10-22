import QtQuick
import QtCharts

// Bar chart: one teal bar per point; labels are the X values
// (formatted as MM-dd when X is a date).
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

    BarSeries {
        id: series
        barWidth: 0.8

        axisX: BarCategoryAxis {
            id: axisX
            labelsColor: "#8a9399"
            labelsFont.pixelSize: 10
            gridVisible: false
            color: "#2a3034"
        }

        axisY: ValueAxis {
            id: axisY
            // Bars start from 0 unless the data goes negative.
            min: Math.min(0, graphController.minY)
            max: Math.max(0, graphController.maxY)
            tickCount: 6
            labelFormat: "%g"
            labelsColor: "#8a9399"
            labelsFont.pixelSize: 10
            gridLineColor: "#262c30"
            color: "#2a3034"
        }

        BarSet {
            id: barSet
            label: qsTr("Value")
            color: "#4f9a86"
            borderColor: "#4f9a86"
        }
    }

    Text {
        anchors.centerIn: parent
        visible: graphController.pointCount === 0
        text: qsTr("Load a dataset to see this chart")
        color: "#5a6369"
        font.pixelSize: 13
    }

    function label(x) {
        if (graphController.xIsDate)
            return Qt.formatDate(new Date(x), "MM-dd")
        return Number.isInteger(x) ? String(x) : String(Number(x.toPrecision(4)))
    }

    function reload() {
        const pts = graphController.points()
        const labels = []
        const values = []
        for (let i = 0; i < pts.length; ++i) {
            labels.push(label(pts[i].x))
            values.push(pts[i].y)
        }
        axisX.categories = labels
        barSet.values = values
    }

    Connections {
        target: graphController
        function onDataLoaded() { chart.reload() }
    }

    Component.onCompleted: reload()
}
