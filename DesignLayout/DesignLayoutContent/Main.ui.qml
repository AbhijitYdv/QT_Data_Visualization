

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
        x: 0
        y: 0
        width: parent.width * .25
        height: parent.height

        Frame {
            id: lineChartSwitchFrame
            x: 0
            y: parent.height * .1
            width: parent.width
            height: parent.height * .05

            opacity: 1
            visible: true

            SwitchDelegate {
                id: lineChartSwitch
                checked: false
                x: parent.width * .65
                y: 0
                width: parent.width * .3
                height: parent.height

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
                x: parent.width * .1
                y: 0
                width: parent.width * .5
                height: parent.height
                text: qsTr("Line Chart")
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                transformOrigin: Item.Center
                font.styleName: "Bold"
                font.family: "Segoe UI"
                font.pointSize: 30
                color: "white"
            }
        }

        Text {
            id: label
            x: parent.width
            y: 0
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

    Rectangle {
        id: lineChart
        x: parent.width * 0.4
        y: parent.height * 0.2
        width: parent.width * .5
        height: parent.height * 0.6
        color: "#ffffff"
        visible: lineChartSwitch.checked
        DragHandler {
                id: graphDragHandler
                target: lineGraph
            }
        LineGraph {
                anchors.fill: parent
            }

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
