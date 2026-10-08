import QtQuick
import QtQuick.Shapes

Rectangle {
    id: magicPath_DataScope_Dashboard

    height: 1024
    width: 1440

    color: "#ffffff"

    Rectangle {
        id: body

        height: 1024
        width: 1440

        anchors.left: parent.left
        anchors.top: parent.top

        color: "#ffffff"

        Rectangle {
            id: div

            height: 1024
            width: 1440

            anchors.left: parent.left
            anchors.top: parent.top

            color: "#111516"

            Image {
                id: header

                anchors.left: parent.left
                anchors.top: parent.top

                source: Qt.resolvedUrl("assets/header.png")

                Text {
                    id: dataScope

                    height: 24
                    width: 85

                    anchors.left: parent.left
                    anchors.leftMargin: 24
                    anchors.top: parent.top
                    anchors.topMargin: 18.50

                    color: "#e8eded"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                    horizontalAlignment: Text.AlignLeft
                    lineHeight: 24
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("DataScope")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.Wrap
                }
                Rectangle {
                    id: live_waveform

                    height: 43
                    width: 235

                    anchors.left: parent.left
                    anchors.leftMargin: 772.77
                    anchors.top: parent.top
                    anchors.topMargin: 9

                    clip: true
                    color: "transparent"

                    Shape {
                        id: path

                        height: 43
                        width: 229.66

                        anchors.left: parent.left
                        anchors.leftMargin: 2.67
                        anchors.top: parent.top
                        anchors.topMargin: 2.93

                        ShapePath {
                            id: path_ShapePath0

                            fillColor: "#00000000"
                            strokeColor: "#287b70"
                            strokeWidth: 1.50

                            PathSvg {
                                id: path_ShapePath0_PathSvg0

                                path: "M 0 22.477272033691406 L 56.68180847167969 22.477272033691406 L 75.24999237060547 21.5 L 89.9090805053711 0 L 110.43180084228516 43 L 132.90907287597656 22.477272033691406 L 229.6590576171875 22.477272033691406"
                            }
                        }
                    }
                }
                Rectangle {
                    id: button

                    height: 33.50
                    width: 105.21

                    anchors.left: parent.left
                    anchors.leftMargin: 1035.77
                    anchors.top: parent.top
                    anchors.topMargin: 13.75

                    color: "transparent"

                    Rectangle {
                        id: button_surface

                        height: 33.50
                        width: 105.21

                        anchors.left: parent.left
                        anchors.top: parent.top

                        border.color: "#3b4548"
                        border.width: 1
                        color: "#191d21"
                        radius: 3
                    }
                    Text {
                        id: load_dataset

                        height: 14.40
                        width: 78

                        anchors.left: parent.left
                        anchors.leftMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: -0.30

                        color: "#d9dfdf"
                        font.family: "Space Grotesk"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignHCenter
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("Load dataset")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Rectangle {
                    id: button_1

                    height: 33.50
                    width: 124.88

                    anchors.left: parent.left
                    anchors.leftMargin: 1149.98
                    anchors.top: parent.top
                    anchors.topMargin: 13.75

                    color: "transparent"

                    Rectangle {
                        id: button_surface_1

                        height: 33.50
                        width: 124.88

                        anchors.left: parent.left
                        anchors.top: parent.top

                        border.color: "#3b4548"
                        border.width: 1
                        color: "#191d21"
                        radius: 3
                    }
                    Text {
                        id: export_snapshot

                        height: 14.40
                        width: 98

                        anchors.left: parent.left
                        anchors.leftMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: -0.30

                        color: "#d9dfdf"
                        font.family: "Space Grotesk"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignHCenter
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("Export snapshot")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Rectangle {
                    id: button_2

                    height: 33.50
                    width: 132.13

                    anchors.left: parent.left
                    anchors.leftMargin: 1283.87
                    anchors.top: parent.top
                    anchors.topMargin: 13.75

                    color: "transparent"

                    Rectangle {
                        id: button_surface_2

                        height: 33.50
                        width: 132.13

                        anchors.left: parent.left
                        anchors.top: parent.top

                        border.color: "#287c70"
                        border.width: 1
                        color: "#191d21"
                        radius: 3
                    }
                    Text {
                        id: export_interactive

                        height: 14.40
                        width: 105

                        anchors.left: parent.left
                        anchors.leftMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: -0.30

                        color: "#55b8a7"
                        font.family: "Space Grotesk"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignHCenter
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("Export interactive")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                    }
                }
            }
            Image {
                id: aside

                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: 62

                source: Qt.resolvedUrl("assets/aside.png")

                Text {
                    id: plugins

                    height: 18
                    width: 42

                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.top: parent.top
                    anchors.topMargin: 24.75

                    color: "#92a09f"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 12
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignLeft
                    lineHeight: 18
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("Plugins")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.Wrap
                }
                Rectangle {
                    id: li

                    height: 39.50
                    width: 197

                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.top: parent.top
                    anchors.topMargin: 53

                    border.color: "#30393c"
                    border.width: 1
                    color: "#191d20"
                    radius: 2

                    Rectangle {
                        id: span

                        height: 7
                        width: 7

                        anchors.left: parent.left
                        anchors.leftMargin: 11
                        anchors.top: parent.top
                        anchors.topMargin: 16.25

                        color: "#3ba191"
                        radius: 3.50
                    }
                    Text {
                        id: line_chart

                        height: 19.50
                        width: 64

                        anchors.left: parent.left
                        anchors.leftMargin: 28
                        anchors.top: parent.top
                        anchors.topMargin: 9.75

                        color: "#d5dada"
                        font.family: "Space Grotesk"
                        font.letterSpacing: -0.13
                        font.pixelSize: 13
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 19.50
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("Line chart")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                        wrapMode: Text.Wrap
                    }
                }
                Rectangle {
                    id: span_1

                    height: 7
                    width: 7

                    anchors.left: parent.left
                    anchors.leftMargin: 27
                    anchors.top: parent.top
                    anchors.topMargin: 108.75

                    color: "#9ba4a3"
                    radius: 3.50
                }
                Text {
                    id: heatmap

                    height: 19.50
                    width: 57

                    anchors.left: parent.left
                    anchors.leftMargin: 44
                    anchors.top: parent.top
                    anchors.topMargin: 102.25

                    color: "#d5dada"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 13
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignLeft
                    lineHeight: 19.50
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("Heatmap")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                }
                Rectangle {
                    id: span_2

                    height: 7
                    width: 7

                    anchors.left: parent.left
                    anchors.leftMargin: 27
                    anchors.top: parent.top
                    anchors.topMargin: 148.25

                    color: "#9ba4a3"
                    radius: 3.50
                }
                Text {
                    id: histogram

                    height: 19.50
                    width: 65

                    anchors.left: parent.left
                    anchors.leftMargin: 44
                    anchors.top: parent.top
                    anchors.topMargin: 141.75

                    color: "#d5dada"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 13
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignLeft
                    lineHeight: 19.50
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("Histogram")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.Wrap
                }
                Rectangle {
                    id: span_3

                    height: 7
                    width: 7

                    anchors.left: parent.left
                    anchors.leftMargin: 27
                    anchors.top: parent.top
                    anchors.topMargin: 187.75

                    color: "#9ba4a3"
                    radius: 3.50
                }
                Text {
                    id: gauge

                    height: 19.50
                    width: 41

                    anchors.left: parent.left
                    anchors.leftMargin: 44
                    anchors.top: parent.top
                    anchors.topMargin: 181.25

                    color: "#d5dada"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 13
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignLeft
                    lineHeight: 19.50
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("Gauge")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                }
                Rectangle {
                    id: span_4

                    height: 7
                    width: 7

                    anchors.left: parent.left
                    anchors.leftMargin: 27
                    anchors.top: parent.top
                    anchors.topMargin: 227.25

                    color: "#9ba4a3"
                    radius: 3.50
                }
                Text {
                    id: geo_map

                    height: 19.50
                    width: 55

                    anchors.left: parent.left
                    anchors.leftMargin: 44
                    anchors.top: parent.top
                    anchors.topMargin: 220.75

                    color: "#d5dada"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 13
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignLeft
                    lineHeight: 19.50
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("Geo map")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                }
                Text {
                    id: load_plugin_

                    height: 18
                    width: 76

                    anchors.left: parent.left
                    anchors.leftMargin: 44
                    anchors.top: parent.top
                    anchors.topMargin: 257.25

                    color: "#83908f"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 12
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignLeft
                    lineHeight: 18
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("Load plugin…")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.Wrap
                }
                Text {
                    id: filters

                    height: 18
                    width: 36

                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.top: parent.top
                    anchors.topMargin: 309.25

                    color: "#92a09f"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 12
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignLeft
                    lineHeight: 18
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("Filters")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.Wrap
                }
                Text {
                    id: date_range

                    height: 16.50
                    width: 59

                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.top: parent.top
                    anchors.topMargin: 337.25

                    color: "#8e9a99"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 11
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignLeft
                    lineHeight: 16.50
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("Date range")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.Wrap
                }
                Rectangle {
                    id: input_date_range

                    height: 32
                    width: 197

                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.top: parent.top
                    anchors.topMargin: 359

                    border.color: "#3c4549"
                    border.width: 1
                    clip: true
                    color: "#191d21"
                    radius: 3

                    Text {
                        id: jan_Sep_2026

                        height: 12
                        width: 92

                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#e2e7e7"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        text: qsTr("Jan – Sep 2026")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                    }
                }
                Text {
                    id: magnitude_4_0

                    height: 16.50
                    width: 86

                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.top: parent.top
                    anchors.topMargin: 405.75

                    color: "#8e9a99"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 11
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignLeft
                    lineHeight: 16.50
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("Magnitude ≥ 4.0")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.Wrap
                }
                Text {
                    id: region

                    height: 16.50
                    width: 37

                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.top: parent.top
                    anchors.topMargin: 458.25

                    color: "#8e9a99"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 11
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignLeft
                    lineHeight: 16.50
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("Region")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.Wrap
                }
                Rectangle {
                    id: select_region

                    height: 34
                    width: 197

                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.top: parent.top
                    anchors.topMargin: 480

                    border.color: "#3c4549"
                    border.width: 1
                    clip: true
                    color: "#191d21"
                    radius: 3
                }
                Text {
                    id: dataset

                    height: 18
                    width: 47

                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.top: parent.top
                    anchors.topMargin: 540.75

                    color: "#92a09f"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 12
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignLeft
                    lineHeight: 18
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("Dataset")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.Wrap
                }
                Rectangle {
                    id: div_1

                    height: 70
                    width: 197

                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.top: parent.top
                    anchors.topMargin: 569

                    color: "transparent"

                    Rectangle {
                        id: div_surface

                        height: 70
                        width: 197

                        anchors.left: parent.left
                        anchors.top: parent.top

                        border.color: "#3a4447"
                        border.width: 1
                        color: "#191d21"
                        radius: 3
                    }
                    Text {
                        id: quakes_2024_csv

                        height: 19.50
                        width: 117

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.top: parent.top
                        anchors.topMargin: 12.75

                        color: "#e0e7e6"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 13
                        font.weight: Font.Medium
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 19.50
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("quakes_2024.csv")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                        wrapMode: Text.Wrap
                    }
                    Text {
                        id: _18204_rows_3112_shown

                        height: 15
                        width: 137

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.top: parent.top
                        anchors.topMargin: 40.75

                        color: "#899393"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 10
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 15
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("18204 rows · 3112 shown")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                        wrapMode: Text.Wrap
                    }
                }
            }
            Rectangle {
                id: section

                height: 190
                width: 380.66

                anchors.left: parent.left
                anchors.leftMargin: 250
                anchors.top: parent.top
                anchors.topMargin: 83

                border.color: "#30383b"
                border.width: 1
                clip: true
                color: "#191d20"
                radius: 3

                Rectangle {
                    id: header_1

                    height: 36
                    width: 378.66

                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    anchors.top: parent.top
                    anchors.topMargin: 1

                    color: "transparent"

                    Image {
                        id: header_surface

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/header_surface.png")
                    }
                    Text {
                        id: magnitude_by_region

                        height: 19.50
                        width: 130

                        anchors.left: parent.left
                        anchors.leftMargin: 11
                        anchors.top: parent.top
                        anchors.topMargin: 7.50

                        color: "#e5ebeb"
                        font.family: "Space Grotesk"
                        font.letterSpacing: -0.13
                        font.pixelSize: 13
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 19.50
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("Magnitude by region")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                        wrapMode: Text.Wrap
                    }
                    Text {
                        id: geo_map_1

                        height: 14.50
                        width: 47

                        anchors.left: parent.left
                        anchors.leftMargin: 322.37
                        anchors.top: parent.top
                        anchors.topMargin: 10.25

                        color: "#758180"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 11
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 13.20
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("geo map")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                    }
                }
                Shape {
                    id: div_2

                    height: 128
                    width: 354.66

                    anchors.left: parent.left
                    anchors.leftMargin: 13
                    anchors.top: parent.top
                    anchors.topMargin: 49

                    Rectangle {
                        id: span_5

                        height: 7
                        width: 7

                        anchors.left: parent.left
                        anchors.leftMargin: 106.80
                        anchors.top: parent.top
                        anchors.topMargin: 45.09

                        color: "#40aa9a"
                        radius: 3.50
                    }
                    Rectangle {
                        id: span_6

                        height: 7
                        width: 7

                        anchors.left: parent.left
                        anchors.leftMargin: 237.28
                        anchors.top: parent.top
                        anchors.topMargin: 27.45

                        color: "#40aa9a"
                        radius: 3.50
                    }
                    Rectangle {
                        id: span_7

                        height: 11
                        width: 11

                        anchors.left: parent.left
                        anchors.leftMargin: 205.54
                        anchors.top: parent.top
                        anchors.topMargin: 65.26

                        color: "#df5d4f"
                        radius: 5.50
                    }
                    Rectangle {
                        id: span_8

                        height: 7
                        width: 7

                        anchors.left: parent.left
                        anchors.leftMargin: 276.07
                        anchors.top: parent.top
                        anchors.topMargin: 48.88

                        color: "#40aa9a"
                        radius: 3.50
                    }
                    ShapePath {
                        id: div_2ShapePath

                        strokeColor: "#293537"
                        strokeWidth: 1

                        fillGradient: LinearGradient {
                            id: div_2ShapePath_LinearGradient
                        
                            x1: div_2.width * 0.5
                            x2: div_2.width * 0.5
                            y1: div_2.height * 0
                            y2: div_2.height * 1
                        
                            GradientStop {
                                color: "#ff293234"
                                position: 0
                            }
                            GradientStop {
                                color: "#00293234"
                                position: 1
                            }
                        }

                        PathRectangle {
                            id: div_2PathRectangle

                            x: 0
                            y: 0

                            height: div_2.height
                            width: div_2.width
                        }
                    }
                }
            }
            Rectangle {
                id: section_1

                height: 190
                width: 380.66

                anchors.left: parent.left
                anchors.leftMargin: 644.66
                anchors.top: parent.top
                anchors.topMargin: 83

                border.color: "#30383b"
                border.width: 1
                clip: true
                color: "#191d20"
                radius: 3

                Rectangle {
                    id: header_2

                    height: 36
                    width: 378.66

                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    anchors.top: parent.top
                    anchors.topMargin: 1

                    color: "transparent"

                    Image {
                        id: header_surface_1

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/header_surface_1.png")
                    }
                    Text {
                        id: events_over_time

                        height: 19.50
                        width: 105

                        anchors.left: parent.left
                        anchors.leftMargin: 11
                        anchors.top: parent.top
                        anchors.topMargin: 7.50

                        color: "#e5ebeb"
                        font.family: "Space Grotesk"
                        font.letterSpacing: -0.13
                        font.pixelSize: 13
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 19.50
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("Events over time")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                        wrapMode: Text.Wrap
                    }
                    Text {
                        id: line

                        height: 14.50
                        width: 27

                        anchors.left: parent.left
                        anchors.leftMargin: 341.78
                        anchors.top: parent.top
                        anchors.topMargin: 10.25

                        color: "#758180"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 11
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 13.20
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("line")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                    }
                }
                Rectangle {
                    id: span_9

                    height: 48.63
                    width: 47.23

                    anchors.left: parent.left
                    anchors.leftMargin: 13
                    anchors.top: parent.top
                    anchors.topMargin: 128.37

                    color: "#3c988a"
                }
                Rectangle {
                    id: span_10

                    height: 78.08
                    width: 47.24

                    anchors.left: parent.left
                    anchors.leftMargin: 64.23
                    anchors.top: parent.top
                    anchors.topMargin: 98.92

                    color: "#3c988a"
                }
                Rectangle {
                    id: span_11

                    height: 60.16
                    width: 47.23

                    anchors.left: parent.left
                    anchors.leftMargin: 115.48
                    anchors.top: parent.top
                    anchors.topMargin: 116.84

                    color: "#3c988a"
                }
                Rectangle {
                    id: span_12

                    height: 99.84
                    width: 47.24

                    anchors.left: parent.left
                    anchors.leftMargin: 166.71
                    anchors.top: parent.top
                    anchors.topMargin: 77.16

                    color: "#3c988a"
                }
                Rectangle {
                    id: span_13

                    height: 72.95
                    width: 47.23

                    anchors.left: parent.left
                    anchors.leftMargin: 217.95
                    anchors.top: parent.top
                    anchors.topMargin: 104.05

                    color: "#3c988a"
                }
                Rectangle {
                    id: span_14

                    height: 116.48
                    width: 47.24

                    anchors.left: parent.left
                    anchors.leftMargin: 269.19
                    anchors.top: parent.top
                    anchors.topMargin: 60.52

                    color: "#3c988a"
                }
                Rectangle {
                    id: span_15

                    height: 87.04
                    width: 47.23

                    anchors.left: parent.left
                    anchors.leftMargin: 320.43
                    anchors.top: parent.top
                    anchors.topMargin: 89.96

                    color: "#3c988a"
                }
            }
            Rectangle {
                id: section_2

                height: 190
                width: 380.66

                anchors.left: parent.left
                anchors.leftMargin: 1039.33
                anchors.top: parent.top
                anchors.topMargin: 83

                border.color: "#30383b"
                border.width: 1
                clip: true
                color: "#191d20"
                radius: 3

                Rectangle {
                    id: header_3

                    height: 36
                    width: 378.66

                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    anchors.top: parent.top
                    anchors.topMargin: 1

                    color: "transparent"

                    Image {
                        id: header_surface_2

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/header_surface_2.png")
                    }
                    Text {
                        id: depth_distribution

                        height: 19.50
                        width: 115

                        anchors.left: parent.left
                        anchors.leftMargin: 11
                        anchors.top: parent.top
                        anchors.topMargin: 7.50

                        color: "#e5ebeb"
                        font.family: "Space Grotesk"
                        font.letterSpacing: -0.13
                        font.pixelSize: 13
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 19.50
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("Depth distribution")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                        wrapMode: Text.Wrap
                    }
                    Text {
                        id: histogram_1

                        height: 14.50
                        width: 60

                        anchors.left: parent.left
                        anchors.leftMargin: 309.43
                        anchors.top: parent.top
                        anchors.topMargin: 10.25

                        color: "#758180"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 11
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 13.20
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("histogram")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                    }
                }
                Rectangle {
                    id: span_16

                    height: 40.95
                    width: 55.77

                    anchors.left: parent.left
                    anchors.leftMargin: 13
                    anchors.top: parent.top
                    anchors.topMargin: 136.05

                    color: "#c18446"
                }
                Rectangle {
                    id: span_17

                    height: 70.40
                    width: 55.78

                    anchors.left: parent.left
                    anchors.leftMargin: 72.77
                    anchors.top: parent.top
                    anchors.topMargin: 106.60

                    color: "#c18446"
                }
                Rectangle {
                    id: span_18

                    height: 104.95
                    width: 55.77

                    anchors.left: parent.left
                    anchors.leftMargin: 132.55
                    anchors.top: parent.top
                    anchors.topMargin: 72.05

                    color: "#c18446"
                }
                Rectangle {
                    id: span_19

                    height: 89.59
                    width: 55.78

                    anchors.left: parent.left
                    anchors.leftMargin: 192.33
                    anchors.top: parent.top
                    anchors.topMargin: 87.41

                    color: "#c18446"
                }
                Rectangle {
                    id: span_20

                    height: 56.31
                    width: 55.77

                    anchors.left: parent.left
                    anchors.leftMargin: 252.11
                    anchors.top: parent.top
                    anchors.topMargin: 120.69

                    color: "#c18446"
                }
                Rectangle {
                    id: span_21

                    height: 32
                    width: 55.78

                    anchors.left: parent.left
                    anchors.leftMargin: 311.88
                    anchors.top: parent.top
                    anchors.topMargin: 145

                    color: "#c18446"
                }
            }
            Rectangle {
                id: section_3

                height: 190
                width: 380.66

                anchors.left: parent.left
                anchors.leftMargin: 250
                anchors.top: parent.top
                anchors.topMargin: 287

                border.color: "#30383b"
                border.width: 1
                clip: true
                color: "#191d20"
                radius: 3

                Rectangle {
                    id: header_4

                    height: 36
                    width: 378.66

                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    anchors.top: parent.top
                    anchors.topMargin: 1

                    color: "transparent"

                    Image {
                        id: header_surface_3

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/header_surface_3.png")
                    }
                    Text {
                        id: live_risk_index

                        height: 19.50
                        width: 88

                        anchors.left: parent.left
                        anchors.leftMargin: 11
                        anchors.top: parent.top
                        anchors.topMargin: 7.50

                        color: "#e5ebeb"
                        font.family: "Space Grotesk"
                        font.letterSpacing: -0.13
                        font.pixelSize: 13
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 19.50
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("Live risk index")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                        wrapMode: Text.Wrap
                    }
                    Text {
                        id: gauge_1

                        height: 14.50
                        width: 34

                        anchors.left: parent.left
                        anchors.leftMargin: 335.31
                        anchors.top: parent.top
                        anchors.topMargin: 10.25

                        color: "#758180"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 11
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 13.20
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("gauge")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                    }
                }
                Rectangle {
                    id: div_3

                    height: 82
                    width: 82

                    anchors.left: parent.left
                    anchors.leftMargin: 149.33
                    anchors.top: parent.top
                    anchors.topMargin: 72

                    color: "transparent"

                    Shape {
                        id: div_surface_1

                        height: 82
                        width: 82

                        anchors.left: parent.left
                        anchors.top: parent.top

                        ShapePath {
                            id: div_surface_1ShapePath

                            strokeColor: "#000"
                            strokeWidth: 0

                            fillGradient: ConicalGradient {
                                id: div_surface_1ShapePath_ConicalGradient
                            
                                angle: 0
                                centerX: div_surface_1.width * 0.75
                                centerY: div_surface_1.height * 0.5
                            
                                GradientStop {
                                    color: "#ffdf5d4f"
                                    position: 1
                                }
                                GradientStop {
                                    color: "#ffdf5d4f"
                                    position: 0.67
                                }
                                GradientStop {
                                    color: "#ff30373a"
                                    position: 0.33
                                }
                                GradientStop {
                                    color: "#ff30373a"
                                    position: 0
                                }
                            }

                            PathRectangle {
                                id: div_surface_1PathRectangle

                                x: 0
                                y: 0

                                height: div_surface_1.height
                                width: div_surface_1.width

                                radius: 41
                            }
                        }
                    }
                    Rectangle {
                        id: div_after

                        height: 66
                        width: 66

                        anchors.left: parent.left
                        anchors.leftMargin: 8
                        anchors.top: parent.top
                        anchors.topMargin: 8

                        color: "#191d20"
                        radius: 33
                    }
                    Text {
                        id: _62_

                        height: 27
                        width: 37

                        anchors.left: parent.left
                        anchors.leftMargin: 23.34
                        anchors.top: parent.top
                        anchors.topMargin: 27.50

                        color: "#e8eded"
                        font.family: "Space Grotesk"
                        font.letterSpacing: -0.13
                        font.pixelSize: 18
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 27
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("62%")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                        wrapMode: Text.Wrap
                    }
                }
            }
            Rectangle {
                id: section_4

                height: 190
                width: 380.66

                anchors.left: parent.left
                anchors.leftMargin: 644.66
                anchors.top: parent.top
                anchors.topMargin: 287

                border.color: "#30383b"
                border.width: 1
                clip: true
                color: "#191d20"
                radius: 3

                Rectangle {
                    id: header_5

                    height: 36
                    width: 378.66

                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    anchors.top: parent.top
                    anchors.topMargin: 1

                    color: "transparent"

                    Image {
                        id: header_surface_4

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/header_surface_4.png")
                    }
                    Text {
                        id: correlation_matrix

                        height: 19.50
                        width: 113

                        anchors.left: parent.left
                        anchors.leftMargin: 11
                        anchors.top: parent.top
                        anchors.topMargin: 7.50

                        color: "#e5ebeb"
                        font.family: "Space Grotesk"
                        font.letterSpacing: -0.13
                        font.pixelSize: 13
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 19.50
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("Correlation matrix")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                        wrapMode: Text.Wrap
                    }
                    Text {
                        id: heatmap_1

                        height: 14.50
                        width: 47

                        anchors.left: parent.left
                        anchors.leftMargin: 322.37
                        anchors.top: parent.top
                        anchors.topMargin: 10.25

                        color: "#758180"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 11
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 13.20
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("heatmap")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                    }
                }
                Rectangle {
                    id: i

                    height: 40.66
                    width: 68.53

                    anchors.left: parent.left
                    anchors.leftMargin: 13
                    anchors.top: parent.top
                    anchors.topMargin: 49

                    color: "#31534e"
                }
                Rectangle {
                    id: i_1

                    height: 40.66
                    width: 68.53

                    anchors.left: parent.left
                    anchors.leftMargin: 84.53
                    anchors.top: parent.top
                    anchors.topMargin: 49

                    color: "#42a595"
                }
                Rectangle {
                    id: i_2

                    height: 40.66
                    width: 68.53

                    anchors.left: parent.left
                    anchors.leftMargin: 156.06
                    anchors.top: parent.top
                    anchors.topMargin: 49

                    color: "#233330"
                }
                Rectangle {
                    id: i_3

                    height: 40.66
                    width: 68.53

                    anchors.left: parent.left
                    anchors.leftMargin: 227.59
                    anchors.top: parent.top
                    anchors.topMargin: 49

                    color: "#1e6358"
                }
                Rectangle {
                    id: i_4

                    height: 40.66
                    width: 68.54

                    anchors.left: parent.left
                    anchors.leftMargin: 299.13
                    anchors.top: parent.top
                    anchors.topMargin: 49

                    color: "#61bcae"
                }
                Rectangle {
                    id: i_5

                    height: 40.66
                    width: 68.53

                    anchors.left: parent.left
                    anchors.leftMargin: 13
                    anchors.top: parent.top
                    anchors.topMargin: 92.66

                    color: "#1d665a"
                }
                Rectangle {
                    id: i_6

                    height: 40.66
                    width: 68.53

                    anchors.left: parent.left
                    anchors.leftMargin: 84.53
                    anchors.top: parent.top
                    anchors.topMargin: 92.66

                    color: "#2d4642"
                }
                Rectangle {
                    id: i_7

                    height: 40.66
                    width: 68.53

                    anchors.left: parent.left
                    anchors.leftMargin: 156.06
                    anchors.top: parent.top
                    anchors.topMargin: 92.66

                    color: "#62beae"
                }
                Rectangle {
                    id: i_8

                    height: 40.66
                    width: 68.53

                    anchors.left: parent.left
                    anchors.leftMargin: 227.59
                    anchors.top: parent.top
                    anchors.topMargin: 92.66

                    color: "#31534e"
                }
                Rectangle {
                    id: i_9

                    height: 40.66
                    width: 68.54

                    anchors.left: parent.left
                    anchors.leftMargin: 299.13
                    anchors.top: parent.top
                    anchors.topMargin: 92.66

                    color: "#3fa090"
                }
                Rectangle {
                    id: i_10

                    height: 40.67
                    width: 68.53

                    anchors.left: parent.left
                    anchors.leftMargin: 13
                    anchors.top: parent.top
                    anchors.topMargin: 136.33

                    color: "#61bcae"
                }
                Rectangle {
                    id: i_11

                    height: 40.67
                    width: 68.53

                    anchors.left: parent.left
                    anchors.leftMargin: 84.53
                    anchors.top: parent.top
                    anchors.topMargin: 136.33

                    color: "#31534e"
                }
                Rectangle {
                    id: i_12

                    height: 40.67
                    width: 68.53

                    anchors.left: parent.left
                    anchors.leftMargin: 156.06
                    anchors.top: parent.top
                    anchors.topMargin: 136.33

                    color: "#1e6358"
                }
                Rectangle {
                    id: i_13

                    height: 40.67
                    width: 68.53

                    anchors.left: parent.left
                    anchors.leftMargin: 227.59
                    anchors.top: parent.top
                    anchors.topMargin: 136.33

                    color: "#233330"
                }
                Rectangle {
                    id: i_14

                    height: 40.67
                    width: 68.54

                    anchors.left: parent.left
                    anchors.leftMargin: 299.13
                    anchors.top: parent.top
                    anchors.topMargin: 136.33

                    color: "#42a595"
                }
            }
            Rectangle {
                id: section_5

                height: 190
                width: 380.66

                anchors.left: parent.left
                anchors.leftMargin: 1039.33
                anchors.top: parent.top
                anchors.topMargin: 287

                border.color: "#30383b"
                border.width: 1
                clip: true
                color: "transparent"
                radius: 3

                Rectangle {
                    id: svg

                    height: 18
                    width: 18

                    anchors.left: parent.left
                    anchors.leftMargin: 181.33
                    anchors.top: parent.top
                    anchors.topMargin: 73.50

                    clip: true
                    color: "transparent"

                    Shape {
                        id: path_1

                        height: 0
                        width: 10.50

                        anchors.left: parent.left
                        anchors.leftMargin: 3.75
                        anchors.top: parent.top
                        anchors.topMargin: 9

                        ShapePath {
                            id: path_1_ShapePath0

                            fillColor: "#00000000"
                            strokeColor: "#98a6a4"
                            strokeWidth: 2

                            PathSvg {
                                id: path_1_ShapePath0_PathSvg0

                                path: "M 0 0 L 10.5 0"
                            }
                        }
                    }
                    Shape {
                        id: path_2

                        height: 10.50
                        width: 0

                        anchors.left: parent.left
                        anchors.leftMargin: 9
                        anchors.top: parent.top
                        anchors.topMargin: 3.75

                        ShapePath {
                            id: path_2_ShapePath0

                            fillColor: "#00000000"
                            strokeColor: "#98a6a4"
                            strokeWidth: 2

                            PathSvg {
                                id: path_2_ShapePath0_PathSvg0

                                path: "M 0 0 L 0 10.5"
                            }
                        }
                    }
                }
                Text {
                    id: add_visualization

                    height: 17
                    width: 106

                    anchors.left: parent.left
                    anchors.leftMargin: 138.40
                    anchors.top: parent.top
                    anchors.topMargin: 99.50

                    color: "#8f9b9a"
                    font.family: "Space Grotesk"
                    font.letterSpacing: -0.13
                    font.pixelSize: 13
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignHCenter
                    lineHeight: 15.60
                    lineHeightMode: Text.FixedHeight
                    text: qsTr("Add visualization")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignTop
                }
            }
            Rectangle {
                id: section_6

                height: 180
                width: 1170

                anchors.left: parent.left
                anchors.leftMargin: 250
                anchors.top: parent.top
                anchors.topMargin: 493

                border.color: "#30383b"
                border.width: 1
                clip: true
                color: "#191d20"
                radius: 3

                Image {
                    id: header_6

                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    anchors.top: parent.top
                    anchors.topMargin: 1

                    source: Qt.resolvedUrl("assets/header_1.png")

                    Text {
                        id: data_table_editable

                        height: 19.50
                        width: 135

                        anchors.left: parent.left
                        anchors.leftMargin: 11
                        anchors.top: parent.top
                        anchors.topMargin: 12.50

                        color: "#e5ebeb"
                        font.family: "Space Grotesk"
                        font.letterSpacing: -0.13
                        font.pixelSize: 13
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 19.50
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("Data table — editable")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignTop
                        wrapMode: Text.Wrap
                    }
                    Rectangle {
                        id: button_3

                        height: 39.50
                        width: 51.98

                        anchors.left: parent.left
                        anchors.leftMargin: 1003.73
                        anchors.top: parent.top
                        anchors.topMargin: 2.75

                        border.color: "#3b4548"
                        border.width: 1
                        color: "#191d21"
                        radius: 3

                        Rectangle {
                            id: svg_1

                            height: 12
                            width: 12

                            anchors.left: parent.left
                            anchors.leftMargin: 11
                            anchors.top: parent.top
                            anchors.topMargin: 6

                            clip: true
                            color: "transparent"

                            Shape {
                                id: path_3

                                height: 9
                                width: 9

                                anchors.left: parent.left
                                anchors.leftMargin: 1.50
                                anchors.top: parent.top
                                anchors.topMargin: 1.50

                                ShapePath {
                                    id: path_3_ShapePath0

                                    fillColor: "#00000000"
                                    strokeColor: "#d9dfdf"
                                    strokeWidth: 2

                                    PathSvg {
                                        id: path_3_ShapePath0_PathSvg0

                                        path: "M 0 4.5 C 8.881784197001252e-16 6.985281467437744 2.014718532562256 9 4.5 9 C 6.985281467437744 9 9 6.985281467437744 9 4.5 C 9 2.014718532562256 6.985281467437744 0 4.5 0 C 3.241976022720337 0.004732550121843815 2.0344845056533813 0.49561184644699097 1.1299999952316284 1.3700000047683716 L 0 2.5"
                                    }
                                }
                            }
                            Shape {
                                id: path_4

                                height: 2.50
                                width: 2.50

                                anchors.left: parent.left
                                anchors.leftMargin: 1.50
                                anchors.top: parent.top
                                anchors.topMargin: 1.50

                                ShapePath {
                                    id: path_4_ShapePath0

                                    fillColor: "#00000000"
                                    strokeColor: "#d9dfdf"
                                    strokeWidth: 2

                                    PathSvg {
                                        id: path_4_ShapePath0_PathSvg0

                                        path: "M 0 0 L 0 2.5 L 2.5 2.5"
                                    }
                                }
                            }
                        }
                        Text {
                            id: undo

                            height: 15.50
                            width: 33

                            anchors.left: parent.left
                            anchors.leftMargin: 11
                            anchors.top: parent.top
                            anchors.topMargin: 18.25

                            color: "#d9dfdf"
                            font.family: "Space Grotesk"
                            font.letterSpacing: -0.13
                            font.pixelSize: 12
                            font.weight: Font.Normal
                            horizontalAlignment: Text.AlignHCenter
                            lineHeight: 14.40
                            lineHeightMode: Text.FixedHeight
                            text: qsTr("Undo")
                            textFormat: Text.PlainText
                            verticalAlignment: Text.AlignTop
                        }
                    }
                    Rectangle {
                        id: button_4

                        height: 39.50
                        width: 93.30

                        anchors.left: parent.left
                        anchors.leftMargin: 1063.70
                        anchors.top: parent.top
                        anchors.topMargin: 2.75

                        border.color: "#3b4548"
                        border.width: 1
                        color: "#191d21"
                        radius: 3

                        Rectangle {
                            id: svg_2

                            height: 12
                            width: 12

                            anchors.left: parent.left
                            anchors.leftMargin: 11
                            anchors.top: parent.top
                            anchors.topMargin: 6

                            clip: true
                            color: "transparent"

                            Shape {
                                id: path_5

                                height: 10
                                width: 10

                                anchors.left: parent.left
                                anchors.leftMargin: 1
                                anchors.top: parent.top
                                anchors.topMargin: 1

                                ShapePath {
                                    id: path_5_ShapePath0

                                    fillColor: "#00000000"
                                    strokeColor: "#d9dfdf"
                                    strokeWidth: 2

                                    PathSvg {
                                        id: path_5_ShapePath0_PathSvg0

                                        path: "M 10 5 L 8.760000228881836 5 C 8.310906559228897 4.999039370042738 7.9162546172738075 5.297584235668182 7.795000076293945 5.730000019073486 L 6.619999885559082 9.910000801086426 C 6.604444329626858 9.963334139436483 6.555555555969477 10.000000953674316 6.5 10.000000953674316 C 6.444444444030523 10.000000953674316 6.395555670373142 9.963334139436483 6.380000114440918 9.910000801086426 L 3.619999885559082 0.08999965339899063 C 3.6044443296268582 0.03666631504893303 3.5555555559694767 -3.576278970740532e-7 3.5 -3.576278970740532e-7 C 3.4444444440305233 -3.576278970740532e-7 3.3955556703731418 0.03666631504893303 3.380000114440918 0.08999965339899063 L 2.2049999237060547 4.269999980926514 C 2.0842468962073326 4.700630396604538 1.6922383904457092 4.998720235307701 1.2450000047683716 5 L 0 5"
                                    }
                                }
                            }
                        }
                        Text {
                            id: filtered_view

                            height: 15.50
                            width: 74

                            anchors.left: parent.left
                            anchors.leftMargin: 11
                            anchors.top: parent.top
                            anchors.topMargin: 18.25

                            color: "#d9dfdf"
                            font.family: "Space Grotesk"
                            font.letterSpacing: -0.13
                            font.pixelSize: 12
                            font.weight: Font.Normal
                            horizontalAlignment: Text.AlignHCenter
                            lineHeight: 14.40
                            lineHeightMode: Text.FixedHeight
                            text: qsTr("Filtered view")
                            textFormat: Text.PlainText
                            verticalAlignment: Text.AlignTop
                            wrapMode: Text.Wrap
                        }
                    }
                }
                Rectangle {
                    id: th

                    height: 33
                    width: 304.95

                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    anchors.top: parent.top
                    anchors.topMargin: 47.50

                    color: "transparent"

                    Image {
                        id: th_surface

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/th_surface.png")
                    }
                    Text {
                        id: _time

                        height: 12
                        width: 30

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#81908e"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("time")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                    }
                }
                Rectangle {
                    id: th_1

                    height: 33
                    width: 193.46

                    anchors.left: parent.left
                    anchors.leftMargin: 305.95
                    anchors.top: parent.top
                    anchors.topMargin: 47.50

                    color: "transparent"

                    Image {
                        id: th_surface_1

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/th_surface_1.png")
                    }
                    Text {
                        id: lat

                        height: 12
                        width: 23

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#81908e"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("lat")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Rectangle {
                    id: th_2

                    height: 33
                    width: 238.05

                    anchors.left: parent.left
                    anchors.leftMargin: 499.41
                    anchors.top: parent.top
                    anchors.topMargin: 47.50

                    color: "transparent"

                    Image {
                        id: th_surface_2

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/th_surface_2.png")
                    }
                    Text {
                        id: lon

                        height: 12
                        width: 23

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#81908e"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("lon")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Rectangle {
                    id: th_3

                    height: 33
                    width: 171.17

                    anchors.left: parent.left
                    anchors.leftMargin: 737.46
                    anchors.top: parent.top
                    anchors.topMargin: 47.50

                    color: "transparent"

                    Image {
                        id: th_surface_3

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/th_surface_3.png")
                    }
                    Text {
                        id: mag

                        height: 12
                        width: 23

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#81908e"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("mag")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Rectangle {
                    id: th_4

                    height: 33
                    width: 260.37

                    anchors.left: parent.left
                    anchors.leftMargin: 908.63
                    anchors.top: parent.top
                    anchors.topMargin: 47.50

                    color: "transparent"

                    Image {
                        id: th_surface_4

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/th_surface_4.png")
                    }
                    Text {
                        id: region_1

                        height: 12
                        width: 44

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#81908e"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("region")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                    }
                }
                Rectangle {
                    id: td

                    height: 33
                    width: 304.95

                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    anchors.top: parent.top
                    anchors.topMargin: 80.50

                    color: "transparent"

                    Image {
                        id: td_surface

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface.png")
                    }
                    Text {
                        id: _2026_09_12

                        height: 12
                        width: 72

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("2026-09-12")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Rectangle {
                    id: td_1

                    height: 33
                    width: 193.46

                    anchors.left: parent.left
                    anchors.leftMargin: 305.95
                    anchors.top: parent.top
                    anchors.topMargin: 80.50

                    color: "transparent"

                    Image {
                        id: td_surface_1

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_1.png")
                    }
                    Text {
                        id: _34_05

                        height: 12
                        width: 37

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("34.05")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                    }
                }
                Rectangle {
                    id: td_2

                    height: 33
                    width: 238.05

                    anchors.left: parent.left
                    anchors.leftMargin: 499.41
                    anchors.top: parent.top
                    anchors.topMargin: 80.50

                    color: "transparent"

                    Image {
                        id: td_surface_2

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_2.png")
                    }
                    Text {
                        id: _118_24

                        height: 12
                        width: 51

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("-118.24")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                    }
                }
                Rectangle {
                    id: td_3

                    height: 33
                    width: 171.17

                    anchors.left: parent.left
                    anchors.leftMargin: 737.46
                    anchors.top: parent.top
                    anchors.topMargin: 80.50

                    color: "transparent"

                    Image {
                        id: td_surface_3

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_3.png")
                    }
                    Text {
                        id: _5_10

                        height: 12
                        width: 30

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#e2a24b"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("5.10")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                    }
                }
                Rectangle {
                    id: td_4

                    height: 33
                    width: 260.37

                    anchors.left: parent.left
                    anchors.leftMargin: 908.63
                    anchors.top: parent.top
                    anchors.topMargin: 80.50

                    color: "transparent"

                    Image {
                        id: td_surface_4

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_4.png")
                    }
                    Text {
                        id: soCal

                        height: 12
                        width: 37

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("SoCal")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Rectangle {
                    id: td_5

                    height: 33
                    width: 304.95

                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    anchors.top: parent.top
                    anchors.topMargin: 113.50

                    color: "transparent"

                    Image {
                        id: td_surface_5

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_5.png")
                    }
                    Text {
                        id: _2026_09_13

                        height: 12
                        width: 72

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("2026-09-13")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Rectangle {
                    id: td_6

                    height: 33
                    width: 193.46

                    anchors.left: parent.left
                    anchors.leftMargin: 305.95
                    anchors.top: parent.top
                    anchors.topMargin: 113.50

                    color: "transparent"

                    Image {
                        id: td_surface_6

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_6.png")
                    }
                    Text {
                        id: _37_77

                        height: 12
                        width: 37

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("37.77")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Rectangle {
                    id: td_7

                    height: 33
                    width: 238.05

                    anchors.left: parent.left
                    anchors.leftMargin: 499.41
                    anchors.top: parent.top
                    anchors.topMargin: 113.50

                    color: "transparent"

                    Image {
                        id: td_surface_7

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_7.png")
                    }
                    Text {
                        id: _122_42

                        height: 12
                        width: 51

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("-122.42")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                    }
                }
                Rectangle {
                    id: td_8

                    height: 33
                    width: 171.17

                    anchors.left: parent.left
                    anchors.leftMargin: 737.46
                    anchors.top: parent.top
                    anchors.topMargin: 113.50

                    color: "transparent"

                    Image {
                        id: td_surface_8

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_8.png")
                    }
                    Text {
                        id: _3_80

                        height: 12
                        width: 30

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("3.80")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Rectangle {
                    id: td_9

                    height: 33
                    width: 260.37

                    anchors.left: parent.left
                    anchors.leftMargin: 908.63
                    anchors.top: parent.top
                    anchors.topMargin: 113.50

                    color: "transparent"

                    Image {
                        id: td_surface_9

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_9.png")
                    }
                    Text {
                        id: bay_Area

                        height: 12
                        width: 58

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("Bay Area")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Rectangle {
                    id: td_10

                    height: 32.50
                    width: 304.95

                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    anchors.top: parent.top
                    anchors.topMargin: 146.50

                    color: "transparent"

                    Image {
                        id: td_surface_10

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_10.png")
                    }
                    Text {
                        id: _2026_09_14

                        height: 12
                        width: 72

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: 0.25

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("2026-09-13")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Rectangle {
                    id: td_11

                    height: 32.50
                    width: 193.46

                    anchors.left: parent.left
                    anchors.leftMargin: 305.95
                    anchors.top: parent.top
                    anchors.topMargin: 146.50

                    color: "transparent"

                    Image {
                        id: td_surface_11

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_11.png")
                    }
                    Text {
                        id: _19_43

                        height: 12
                        width: 37

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: 0.25

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("19.43")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                    }
                }
                Rectangle {
                    id: td_12

                    height: 32.50
                    width: 238.05

                    anchors.left: parent.left
                    anchors.leftMargin: 499.41
                    anchors.top: parent.top
                    anchors.topMargin: 146.50

                    color: "transparent"

                    Image {
                        id: td_surface_12

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_12.png")
                    }
                    Text {
                        id: _155_29

                        height: 12
                        width: 51

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: 0.25

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("-155.29")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                    }
                }
                Rectangle {
                    id: td_13

                    height: 32.50
                    width: 171.17

                    anchors.left: parent.left
                    anchors.leftMargin: 737.46
                    anchors.top: parent.top
                    anchors.topMargin: 146.50

                    color: "transparent"

                    Image {
                        id: td_surface_13

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_13.png")
                    }
                    Text {
                        id: _4_60

                        height: 12
                        width: 30

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: 0.25

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("4.60")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                    }
                }
                Rectangle {
                    id: td_14

                    height: 32.50
                    width: 260.37

                    anchors.left: parent.left
                    anchors.leftMargin: 908.63
                    anchors.top: parent.top
                    anchors.topMargin: 146.50

                    color: "transparent"

                    Image {
                        id: td_surface_14

                        anchors.left: parent.left
                        anchors.top: parent.top

                        source: Qt.resolvedUrl("assets/td_surface_14.png")
                    }
                    Text {
                        id: hawaii

                        height: 12
                        width: 44

                        anchors.left: parent.left
                        anchors.leftMargin: 13
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: 0.25

                        color: "#dce4e3"
                        font.family: "IBM Plex Mono"
                        font.letterSpacing: -0.13
                        font.pixelSize: 12
                        font.weight: Font.Normal
                        horizontalAlignment: Text.AlignLeft
                        lineHeight: 14.40
                        lineHeightMode: Text.FixedHeight
                        text: qsTr("Hawaii")
                        textFormat: Text.PlainText
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                    }
                }
            }
        }
    }
}