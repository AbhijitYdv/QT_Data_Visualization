/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DesignLayout

Rectangle {
    id: root
    width: Constants.width
    height: Constants.height
    color: "#171b1e"

    // Chart layout:
    //  - without the map: up to 4 charts in a 2 x 2 grid that fills the screen;
    //  - with the map: the map plus at most 1 other chart, side by side (1 x 2),
    //    each the full height. The map takes 3 of the 4 slots.
    readonly property bool mapOn: mapPlugin.checked
    readonly property int otherCharts: (linePlugin.checked ? 1 : 0)
                                       + (barPlugin.checked ? 1 : 0)
                                       + (areaPlugin.checked ? 1 : 0)
                                       + (histogramPlugin.checked ? 1 : 0)
                                       + (piePlugin.checked ? 1 : 0)
    readonly property int chartsShown: otherCharts + (mapOn ? 1 : 0)
    readonly property int maxCharts: mapOn ? 2 : 4
    readonly property bool chartsFull: chartsShown >= maxCharts
    readonly property bool mapBlocked: !mapOn && otherCharts > 1

    // Space under the summary tiles: two rows of charts, or one tall row with the map.
    readonly property real chartAreaHeight: chartScroll.availableHeight
                                            - (summaryPlugin.checked ? 88 : 0)
    readonly property real cardHeight: mapOn ? Math.max(420, chartAreaHeight)
                                             : Math.max(280, (chartAreaHeight - 16) / 2)

    // ================= Header bar =================
    Rectangle {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 64
        color: "#1b2023"

        Text {
            id: appTitle
            anchors.left: parent.left
            anchors.leftMargin: 24
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("DataScope")
            color: "#e6e8ea"
            font.pixelSize: 18
            font.bold: true
        }

        Row {
            id: headerActions
            anchors.right: parent.right
            anchors.rightMargin: 24
            anchors.verticalCenter: parent.verticalCenter
            spacing: 8

            // Opens the file picker (logic in ImportButton.qml)
            ImportButton {
                id: loadButton
                text: qsTr("Load dataset")
            }

            // Placeholder for the export feature - not wired up yet
            Rectangle {
                id: exportButton
                width: exportLabel.implicitWidth + 28
                height: 34
                radius: 4
                color: "#1b2023"
                border.color: "#3a4348"
                opacity: 0.5

                Text {
                    id: exportLabel
                    anchors.centerIn: parent
                    text: qsTr("Export snapshot")
                    color: "#e6e8ea"
                    font.pixelSize: 13
                }
            }
        }

        Rectangle {
            id: headerBorder
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 1
            color: "#2a3034"
        }
    }

    // ================= Sidebar =================
    Rectangle {
        id: sidebar
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: 240
        color: "#1b2023"

        // Plugins and data pickers scroll; the dataset section stays at the bottom.
        Flickable {
            id: sidebarScroll
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: datasetSection.top
            anchors.bottomMargin: 12
            clip: true
            contentHeight: sidebarContent.height + 40
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar { }

            Column {
                id: sidebarContent
                x: 20
                y: 20
                width: sidebarScroll.width - 40
                spacing: 6

                Text {
                    id: pluginsHeading
                    text: qsTr("Charts") + "  " + root.chartsShown + " / " + root.maxCharts
                    color: "#8a9399"
                    font.pixelSize: 13
                    bottomPadding: 6
                }

                PluginRow {
                    id: linePlugin
                    width: parent.width
                    locked: root.chartsFull
                    text: qsTr("Line chart")
                    checked: true
                }

                PluginRow {
                    id: barPlugin
                    width: parent.width
                    locked: root.chartsFull
                    text: qsTr("Bar chart")
                    checked: true
                }

                PluginRow {
                    id: areaPlugin
                    width: parent.width
                    locked: root.chartsFull
                    text: qsTr("Area chart")
                    checked: false
                }

                PluginRow {
                    id: histogramPlugin
                    width: parent.width
                    locked: root.chartsFull
                    text: qsTr("Histogram")
                    checked: true
                }

                PluginRow {
                    id: piePlugin
                    width: parent.width
                    locked: root.chartsFull
                    text: qsTr("Pie chart")
                    checked: true
                }

                PluginRow {
                    id: mapPlugin
                    width: parent.width
                    locked: root.mapBlocked
                    text: qsTr("US map")
                    checked: false
                }

                Text {
                    id: chartLimitHint
                    visible: root.chartsFull
                    width: parent.width
                    text: qsTr("Turn a chart off to show another.")
                    color: "#5a6369"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                }

                Text {
                    id: mapHint
                    visible: root.mapBlocked
                    width: parent.width
                    text: qsTr("The map shows next to at most one other chart.")
                    color: "#5a6369"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                }

                Text {
                    id: panelsHeading
                    text: qsTr("Panels")
                    color: "#8a9399"
                    font.pixelSize: 13
                    topPadding: 16
                    bottomPadding: 6
                }

                PluginRow {
                    id: summaryPlugin
                    width: parent.width
                    text: qsTr("Summary cards")
                    checked: true
                }

                PluginRow {
                    id: tablePlugin
                    width: parent.width
                    text: qsTr("Data table")
                    checked: true
                }

                // ---- Column pickers (logic lives in DataControls.qml) ----
                Text {
                    id: dataHeading
                    text: qsTr("Data")
                    color: "#8a9399"
                    font.pixelSize: 13
                    topPadding: 22
                    bottomPadding: 4
                }

                DataControls {
                    id: dataControls
                    width: parent.width
                }
            }
        }

        Column {
            id: datasetSection
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 20
            spacing: 8

            RecentDatasets {
                width: parent.width
            }

            Text {
                id: datasetHeading
                text: qsTr("Dataset")
                color: "#8a9399"
                font.pixelSize: 13
            }

            DatasetCard {
                id: datasetCard
                width: parent.width
            }
        }

        Rectangle {
            id: sidebarBorder
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            width: 1
            color: "#2a3034"
        }
    }

    // ================= Chart area =================
    // Summary tiles on top, then the chart cards two per row, then the table.
    // Cards keep a fixed height and the area scrolls, so turning more cards
    // on never squeezes the charts.
    ScrollView {
        id: chartScroll
        anchors.top: header.bottom
        anchors.left: sidebar.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            id: chartColumn
            width: chartScroll.availableWidth
            spacing: 16

            SummaryCards {
                id: summaryCards
                visible: summaryPlugin.checked
                Layout.fillWidth: true
            }

            GridLayout {
                id: chartGrid
                Layout.fillWidth: true
                columns: 2
                rowSpacing: 16
                columnSpacing: 16

                DashboardCard {
                    id: mapCard
                    title: qsTr("US map")
                    tag: "map"
                    visible: mapPlugin.checked
                    Layout.columnSpan: root.otherCharts === 0 ? 2 : 1
                    Layout.fillWidth: true
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: root.cardHeight

                    UsMap {
                        id: usMap
                    }
                }

                DashboardCard {
                    id: lineCard
                    title: qsTr("Line chart")
                    tag: "line"
                    visible: linePlugin.checked
                    Layout.fillWidth: true
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: root.cardHeight

                    LineGraph {
                        id: lineGraph
                    }
                }

                DashboardCard {
                    id: barCard
                    title: qsTr("Bar chart")
                    tag: "bar"
                    visible: barPlugin.checked
                    Layout.fillWidth: true
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: root.cardHeight

                    BarGraph {
                        id: barGraph
                    }
                }

                DashboardCard {
                    id: areaCard
                    title: qsTr("Area chart")
                    tag: "area"
                    visible: areaPlugin.checked
                    Layout.fillWidth: true
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: root.cardHeight

                    AreaChart {
                        id: areaChart
                    }
                }

                DashboardCard {
                    id: histogramCard
                    title: qsTr("Histogram") + " · " + graphController.yColumnName
                    tag: "histogram"
                    visible: histogramPlugin.checked
                    Layout.fillWidth: true
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: root.cardHeight

                    Histogram {
                        id: histogram
                    }
                }

                DashboardCard {
                    id: pieCard
                    title: qsTr("Pie chart")
                    tag: "pie"
                    visible: piePlugin.checked
                    Layout.fillWidth: true
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: root.cardHeight

                    PieChart {
                        id: pieChart
                    }
                }

                DashboardCard {
                    id: tableCard
                    title: qsTr("Data table")
                    tag: graphController.tableModel.count + qsTr(" rows")
                    visible: tablePlugin.checked
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 420

                    DataTable {
                        id: dataTable
                    }
                }

                // Placeholder slot for future plugins
                Rectangle {
                    id: addVisualization
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 72
                    color: "#171b1e"
                    radius: 4
                    border.color: "#2a3034"

                    Text {
                        id: addLabel
                        anchors.centerIn: parent
                        text: qsTr("+ Add visualization")
                        color: "#8a9399"
                        font.pixelSize: 13
                    }
                }
            }
        }
    }
}
