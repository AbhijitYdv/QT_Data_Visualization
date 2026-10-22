import QtQuick
import QtQuick.Dialogs

// Flat header button that opens a .csv/.txt picker and loads the file.
Rectangle {
    id: root
    property string text: qsTr("Load dataset")

    width: label.implicitWidth + 28
    height: 34
    radius: 4
    color: mouse.pressed ? "#22292d" : (mouse.containsMouse ? "#20262a" : "#1b2023")
    border.color: mouse.containsMouse ? "#4f9a86" : "#3a4348"
    opacity: graphController.loading ? 0.5 : 1.0

    Text {
        id: label
        anchors.centerIn: parent
        text: graphController.loading ? qsTr("Loading…") : root.text
        color: "#e6e8ea"
        font.pixelSize: 13
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        enabled: !graphController.loading
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: fileDialog.open()
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Choose a data file")
        nameFilters: [
            qsTr("Data files (*.csv *.txt)"),
            qsTr("All files (*)")
        ]
        onAccepted: graphController.loadFile(selectedFile)
    }
}
