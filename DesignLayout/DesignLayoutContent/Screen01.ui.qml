

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import DesignLayout
import QtQuick.Studio.DesignEffects

Rectangle {
    id: rectangle
    width: Constants.width
    height: Constants.height
    color: "#303030"

    Frame {
        id: frame
        x: 8
        y: 0
        width: parent.width * .25
        height: parent.height

        Frame {
            id: lineChartSwitch
            x: -12
            y: 83
            width: parent.width
            height: parent.height * .1

            opacity: 1
            visible: true

            SwitchDelegate {
                id: switchDelegate
                x: 88
                y: -14
                width: 239
                height: 76

                icon.color: "white"

                DesignEffect {
                    visible: false
                    effects: [
                        DesignDropShadow {}
                    ]
                }
            }

            Label {
                id: label1
                x: 32
                y: 5
                width: 123
                height: 38
                text: qsTr("Line Chart")
                font.styleName: "Bold"
                font.family: "Segoe UI"
                font.pointSize: 20
                color: "white"
            }
        }

        Text {
            id: label
            x: 889
            y: 6
            color: "#ffffff"
            text: qsTr("Plugins")
            font.family: Constants.font.family
            anchors.topMargin: 45
            font.styleName: "Light"
            anchors.horizontalCenterOffset: -131
            font.pointSize: 30
            anchors.horizontalCenter: parent.horizontalCenter

            SequentialAnimation {
                id: animation

                ColorAnimation {
                    id: colorAnimation1
                    target: rectangle
                    property: "color"
                    to: "#2294c6"
                    from: Constants.backgroundColor
                }

                ColorAnimation {
                    id: colorAnimation2
                    target: rectangle
                    property: "color"
                    to: Constants.backgroundColor
                    from: "#2294c6"
                }
            }
        }
    }

    MouseArea {
        id: mouseArea
        x: -663
        y: -264
        anchors.fill: parent
        anchors.leftMargin: 535
        anchors.rightMargin: 123
        anchors.topMargin: 43
        anchors.bottomMargin: 42
    }
    states: [
        State {
            name: "clicked"

            PropertyChanges {
                target: label
                text: qsTr("Button Checked")
            }
        }
    ]
}
