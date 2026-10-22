import QtQuick

// X / Y / filter pickers wired to GraphController.
Column {
    id: root
    width: 200
    spacing: 10

    ColumnPicker {
        width: root.width
        label: qsTr("X axis")
        options: graphController.xOptions
        current: graphController.xColumnName
        onPicked: function(value) { graphController.xColumnName = value }
    }

    ColumnPicker {
        width: root.width
        label: qsTr("Y axis")
        options: graphController.yOptions
        current: graphController.yColumnName
        onPicked: function(value) { graphController.yColumnName = value }
    }

    ColumnPicker {
        width: root.width
        label: qsTr("Filter by")
        options: graphController.filterColumnOptions
        current: graphController.filterColumnName
        onPicked: function(value) { graphController.filterColumnName = value }
    }

    ColumnPicker {
        width: root.width
        visible: graphController.filterValues.length > 0
        label: qsTr("Show only")
        options: graphController.filterValues
        current: graphController.filterValue
        onPicked: function(value) { graphController.filterValue = value }
    }
}
