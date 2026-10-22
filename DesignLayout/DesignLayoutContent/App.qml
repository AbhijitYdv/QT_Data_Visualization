import QtQuick
import DesignLayout

// The window now has a sensible laptop size and the dashboard fills it,
// instead of a fixed 1920x1080 screen centred (and cut off) in the window.
Window {
    width: 1440
    height: 900
    minimumWidth: 1000
    minimumHeight: 640
    visible: true
    title: "DataScope"
    color: "#171b1e"

    Main {
        id: mainScreen
        anchors.fill: parent
    }
}
