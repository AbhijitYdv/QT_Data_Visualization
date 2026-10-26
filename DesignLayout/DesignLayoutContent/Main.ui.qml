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

        Column {
            id: pluginList
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 20
            spacing: 6

            Text {
                id: pluginsHeading
                text: qsTr("Plugins")
                color: "#8a9399"
                font.pixelSize: 13
                bottomPadding: 6
            }

            PluginRow {
                id: linePlugin
                width: parent.width
                text: qsTr("Line chart")
                checked: true
            }

            PluginRow {
                id: barPlugin
                width: parent.width
                text: qsTr("Bar chart")
                checked: true
            }

            PluginRow {
                id: simpleLinePlugin
                width: parent.width
                text: qsTr("Line graph")
                checked: true
            }
        }

        // ---- Column pickers (logic lives in DataControls.qml) ----
        Text {
            id: dataHeading
            anchors.top: pluginList.bottom
            anchors.topMargin: 28
            anchors.left: parent.left
            anchors.leftMargin: 20
            text: qsTr("Data")
            color: "#8a9399"
            font.pixelSize: 13
        }

        DataControls {
            id: dataControls
            anchors.top: dataHeading.bottom
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 20
            anchors.rightMargin: 20
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

    // ================= Chart grid =================
    GridLayout {
        id: chartGrid
        anchors.top: header.bottom
        anchors.left: sidebar.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        columns: 2
        rowSpacing: 16
        columnSpacing: 16

        DashboardCard {
            id: lineCard
            title: qsTr("Line chart")
            tag: "line"
            visible: linePlugin.checked
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 100
            Layout.preferredHeight: 100

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
            Layout.fillHeight: true
            Layout.preferredWidth: 100
            Layout.preferredHeight: 100

            BarGraph {
                id: barGraph
            }
        }

        DashboardCard {
            id: simpleLineCard
            title: qsTr("Line graph")
            tag: "line-simple"
            visible: simpleLinePlugin.checked
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 100
            Layout.preferredHeight: 100

            SimpleLineGraph {
                id: simpleLineGraph
            }
        }

        // Placeholder slot for future plugins
        Rectangle {
            id: addVisualization
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 100
            Layout.preferredHeight: 100
            color: "#171b1e"
            radius: 4
            border.color: "#2a3034"

            Text {
                id: addLabel
                anchors.centerIn: parent
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("+\nAdd visualization")
                color: "#8a9399"
                font.pixelSize: 13
                lineHeight: 1.4
            }
        }
    }
}
