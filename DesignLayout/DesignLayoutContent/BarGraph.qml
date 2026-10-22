import QtQuick
import QtCharts

// Bar chart: up to 50 teal bars; bigger data is grouped and averaged.
// Labels are the X values (MM-dd when X is a date; group start when grouped).
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
        visible: graphController.loading || graphController.pointCount === 0
        text: graphController.loading ? qsTr("Loading…")
                                      : qsTr("Load a dataset to see this chart")
        color: "#5a6369"
        font.pixelSize: 13
    }

    function label(x) {
        if (graphController.xIsDate)
            return Qt.formatDate(new Date(x), "MM-dd")
        return Number.isInteger(x) ? String(x) : String(Number(x.toPrecision(4)))
    }

    function reload() {
        // At most 50 bars; larger data is grouped and averaged in C++.
        const data = graphController.barData()
        const labels = []
        for (let i = 0; i < data.x.length; ++i)
            labels.push(label(data.x[i]))
        axisX.categories = labels
        barSet.values = data.y
    }

    Connections {
        target: graphController
        function onDataLoaded() { chart.reload() }
    }

    Component.onCompleted: reload()
}
