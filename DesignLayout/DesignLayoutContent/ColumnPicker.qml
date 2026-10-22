import QtQuick
import QtQuick.Controls

// Small labelled dropdown used in the sidebar "Data" section.
Column {
    id: root
    property string label
    property var options: []
    property string current
    signal picked(string value)

    width: 200
    spacing: 4

    function sync() {
        combo.currentIndex = options ? Array.prototype.indexOf.call(options, current) : -1
    }
    onOptionsChanged: Qt.callLater(sync)
    onCurrentChanged: Qt.callLater(sync)
    Component.onCompleted: sync()

    Text {
        text: root.label
        color: "#8a9399"
        font.pixelSize: 11
    }

    ComboBox {
        id: combo
        width: root.width
        height: 30
        model: root.options
        enabled: root.options && root.options.length > 0
        onActivated: root.picked(currentText)

        background: Rectangle {
            radius: 4
            color: "#171b1e"
            border.color: combo.hovered ? "#4f9a86" : "#2a3034"
        }

        contentItem: Text {
            leftPadding: 10
            rightPadding: 24
            text: combo.displayText
            color: combo.enabled ? "#e6e8ea" : "#5a6369"
            font.pixelSize: 12
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
}
