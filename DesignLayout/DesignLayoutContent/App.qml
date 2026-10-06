import QtQuick
import DesignLayout

Window {
    width: mainScreen.width
    height: mainScreen.height

    visible: true
    title: "DesignLayout"

    Screen01 {
        id: mainScreen

        anchors.centerIn: parent
    }

}

