import QtQuick
import DesignLayout

Window {
    width: mainScreen.width
    height: mainScreen.height

    visible: true
    title: "DesignLayout"

    Main {
        id: mainScreen

        anchors.centerIn: parent
    }

}

