import QtQuick

// Shows one chart plugin inside a DashboardCard.
//   PluginView { pluginId: "core.viz.bar" }
//
// Loads the plugin's own QML (qrc:/plugins/...), creates its backend on
// graphController.tableModel (the filtered rows), and hands it to the view
// as `backend`. Backends that have xColumn / yColumn follow the sidebar's
// column pickers; allRowsModel / filterColumn / filterValue are filled in
// too when a backend has them. If the plugin isn't installed, says so instead.
Item {
    id: root
    property string pluginId
    property QtObject backend: null

    // Reading `count` makes this re-check whenever plugins are (re)loaded.
    readonly property bool available: pluginManager.count >= 0 && pluginManager.has(pluginId)

    anchors.fill: parent

    Loader {
        id: loader
        anchors.fill: parent
        active: root.available
        visible: !graphController.loading
        source: root.available ? pluginManager.qmlSource(root.pluginId) : ""

        onLoaded: {
            const b = pluginManager.createBackend(root.pluginId, graphController.tableModel, item)
            if (b) {
                if ("xColumn" in b)
                    b.xColumn = Qt.binding(() => graphController.xColumnName)
                if ("yColumn" in b)
                    b.yColumn = Qt.binding(() => graphController.yColumnName)
                // Optional extras, for plugins that want them (the pie):
                if ("allRowsModel" in b)
                    b.allRowsModel = graphController.allRowsModel
                if ("filterColumn" in b)
                    b.filterColumn = Qt.binding(() => graphController.filterColumnName)
                if ("filterValue" in b)
                    b.filterValue = Qt.binding(() => graphController.filterValue)
            }
            root.backend = b
            item.backend = b
        }
        onStatusChanged: {
            if (status === Loader.Error)
                console.warn("PluginView: could not load the view of", root.pluginId)
        }
    }

    Text {
        anchors.centerIn: parent
        visible: !root.available || loader.status === Loader.Error || graphController.loading
        text: graphController.loading ? qsTr("Loading…")
              : qsTr("Plugin \"%1\" is not installed").arg(root.pluginId)
        color: "#5a6369"
        font.pixelSize: 13
    }
}
