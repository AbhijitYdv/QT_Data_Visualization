import QtQuick

// Sidebar card showing which file is loaded and the parser's status.
Rectangle {
    id: root
    width: 200
    height: info.implicitHeight + 24
    radius: 4
    color: "#171b1e"
    border.color: "#2a3034"

    Column {
        id: info
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 12
        spacing: 4

        Text {
            width: parent.width
            elide: Text.ElideMiddle
            text: graphController.fileName !== ""
                  ? graphController.fileName
                  : qsTr("No dataset loaded")
            color: "#e6e8ea"
            font.pixelSize: 13
        }

        Text {
            width: parent.width
            wrapMode: Text.WordWrap
            text: graphController.statusMessage !== ""
                  ? graphController.statusMessage
                  : qsTr("Use Load dataset to open a .csv or .txt file")
            color: "#8a9399"
            font.pixelSize: 11
        }
    }
}
