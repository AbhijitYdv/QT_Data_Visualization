# Class Specification (QML dashboard)

Status: draft for team review. This is the contract both developers code against. If a signature here changes, update this file in the same pull request.

## Layers

| Layer | Classes | Notes |
|---|---|---|
| Plugin system | `IDataParser`, `IVisualizationPlugin`, `PluginManager`, `PluginMetadata` | Interfaces are pure C++ with no UI dependency. |
| Core data | `DatasetController`, `EditableTableModel`, `FilterController` | Unchanged by the move to QML. QML views read these models directly. |
| QML bridge | `DashboardController`, `PanelModel`, `ExportManager` | Registered with `QML_ELEMENT` so `.qml` files can use them. |

Conventions: Qt 6.9, C++17, CMake. Raw pointers to Qt objects are owned through the QObject parent tree. Anything that can take longer than about 100 ms (parsing, exporting) runs off the UI thread with `QtConcurrent` and reports back through a signal.

---

## Plugin system

### PluginMetadata

Plain value type shared by both interfaces.

```cpp
struct PluginMetadata {
    QString id;           // stable and unique, e.g. "core.parser.csv"
    QString displayName;  // shown in the sidebar
    QString description;  // tooltip text
    QIcon   icon;
    QString version;
};
```

### IDataParser

One implementation per file format. Discovered at runtime through `QPluginLoader`.

```cpp
class IDataParser {
public:
    virtual ~IDataParser() = default;
    virtual PluginMetadata metadata() const = 0;
    virtual QStringList supportedExtensions() const = 0;       // {"csv", "tsv"}
    virtual bool canParse(const QString &filePath) const = 0;  // cheap check, no full parse
    virtual QAbstractItemModel *load(const QString &filePath,
                                     QObject *modelParent,
                                     QString *errorMessage) = 0;  // nullptr on failure
};
#define IDataParser_iid "com.datascope.IDataParser/1.0"
Q_DECLARE_INTERFACE(IDataParser, IDataParser_iid)
```

Rules:
- `canParse()` checks the extension first, then optionally sniffs a few bytes. It never parses the whole file.
- `load()` is synchronous and thread-safe. The caller decides which thread runs it.
- On failure return `nullptr` and fill `errorMessage` with a message a user can read.
- The UML shows `displayName()` and `icon()` as shorthand for fields inside `metadata()`.

Planned implementations: `CsvParser`, `JsonParser`, `XlsxParser`, `SqliteParser`, `GeoJsonParser`. Stretch: `PythonBridgeParser`.

### IVisualizationPlugin

One implementation per chart type. The core app never includes a concrete plugin header.

```cpp
class IVisualizationPlugin {
public:
    virtual ~IVisualizationPlugin() = default;
    virtual PluginMetadata metadata() const = 0;
    virtual bool canVisualize(QAbstractItemModel *model) const = 0;  // e.g. geo map needs lat/lon
    virtual QUrl qmlSource() const = 0;                              // file the PanelCard Loader shows
    virtual QObject *createBackend(QAbstractItemModel *model,
                                   QObject *parent) = 0;             // feeds the QML view
    virtual QVariantMap defaultConfig() const = 0;                   // axes, bins, thresholds
};
#define IVisualizationPlugin_iid "com.datascope.IVisualizationPlugin/1.0"
Q_DECLARE_INTERFACE(IVisualizationPlugin, IVisualizationPlugin_iid)
```

Rules:
- `createBackend()` returns a `QObject` whose properties the QML view binds to. It must connect to the model's `dataChanged`, `rowsInserted`, `rowsRemoved` and `modelReset` signals so views stay live during edits and filtering.
- `qmlSource()` points at a `qrc:/` path inside the plugin so the plugin ships its own view.
- Config is a flat `QVariantMap` so the layout can be saved as JSON.

Planned implementations: `LineChartPlugin`, `HeatmapPlugin`, `HistogramPlugin`, `GaugePlugin`, `GeoMapPlugin`.

### PluginManager

```cpp
class PluginManager : public QObject {
    Q_OBJECT
public:
    void loadFrom(const QString &directory);          // idempotent, skips loaded paths
    QVector<IDataParser *> parsers() const;
    QVector<IVisualizationPlugin *> visualizations() const;
    IDataParser *parserFor(const QString &filePath) const;
    IVisualizationPlugin *visualization(const QString &id) const;
signals:
    void pluginLoaded(const PluginMetadata &metadata);
    void pluginFailed(const QString &path, const QString &reason);
};
```

Rules:
- A single binary may implement both interfaces.
- Never use a hardcoded extension (`.so`, `.dll`, `.dylib`). Use `QLibrary::isLibrary()`.
- A bad plugin emits `pluginFailed` and is skipped. It must never crash startup.

---

## Core data

### DatasetController

Owns the loaded dataset and the chain raw model, filter, view.

```cpp
class DatasetController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *filtered READ filtered NOTIFY datasetChanged)
    Q_PROPERTY(QString datasetName READ datasetName NOTIFY datasetChanged)
    Q_PROPERTY(int rowCount READ rowCount NOTIFY datasetChanged)
public:
    Q_INVOKABLE void load(const QString &path);   // async, finishes with datasetChanged or loadFailed
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
signals:
    void datasetChanged();
    void loadFailed(const QString &reason);
};
```

### EditableTableModel

`QAbstractTableModel` subclass holding the parsed data.

- `data()` supports `Qt::DisplayRole`, `Qt::EditRole` and a custom `EditedRole` (true when a cell was changed, used to highlight it).
- `setData()` pushes an `EditCommand` onto the `QUndoStack` and does not mutate directly. The command's `redo()` and `undo()` call an internal setter and emit `dataChanged`.
- `flags()` returns `Qt::ItemIsEditable` for data columns.
- Column types (number, text, date, latitude, longitude) are inferred at load and exposed through `headerData()` with a custom `ColumnTypeRole`.

### FilterController

`QSortFilterProxyModel` subclass applied to the whole dashboard.

```cpp
class FilterController : public QSortFilterProxyModel {
    Q_OBJECT
public:
    Q_INVOKABLE void setRange(int column, double lo, double hi);
    Q_INVOKABLE void setRegex(int column, const QString &pattern);
    Q_INVOKABLE void clear();
protected:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const override;
};
```

Rules: all active rules combine with AND. Every plugin reads through this proxy, so one filter change updates every panel.

---

## QML bridge

### DashboardController

```cpp
class DashboardController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(PanelModel *panels READ panels CONSTANT)
public:
    Q_INVOKABLE void addPanel(const QString &pluginId);
    Q_INVOKABLE void removePanel(int index);
    Q_INVOKABLE bool saveLayout(const QString &path);   // JSON
    Q_INVOKABLE bool loadLayout(const QString &path);
};
```

Layout file (JSON): a list of panels, each with `pluginId`, `config` and grid position. It stores no raw data, only a reference to the dataset path and the active filters.

### PanelModel

`QAbstractListModel` used by the `Repeater` in `DashboardView`.

Roles: `PluginIdRole`, `TitleRole`, `ConfigRole`, `BackendRole`, `GridRectRole`.

Methods: `rowCount()`, `data()`, `setData()` for edited titles and config, `move(from, to)` for drag-reordering, `append()`, `removeAt()`.

### ExportManager

```cpp
class ExportManager : public QObject {
    Q_OBJECT
    QML_ELEMENT
public:
    Q_INVOKABLE void exportSnapshot(const QString &path);     // PNG or PDF of the dashboard
    Q_INVOKABLE void exportInteractive(const QString &path);  // standalone HTML bundle
signals:
    void exportFinished(const QString &path);
    void exportFailed(const QString &reason);
};
```

- Snapshot: `grabToImage()` on the dashboard item, then `QPdfWriter` for PDF.
- Interactive: embeds the filtered data as JSON plus a small JavaScript renderer into one `.html` file. MVP target is one visualization type (geo map).

---

## QML files and the classes behind them

| QML component | Backed by |
|---|---|
| `main.qml` | `DatasetController`, `DashboardController`, `ExportManager` |
| `Toolbar` | `DatasetController.load()`, `undo()`, `ExportManager` |
| `Sidebar` / `PluginList` | `PluginManager.visualizations()` |
| `Sidebar` / `FilterPanel` | `FilterController` |
| `DashboardView` / `Repeater` | `DashboardController.panels` (`PanelModel`) |
| `PanelCard` (`Loader`) | `qmlSource()` and `createBackend()` of the chosen plugin |
| `DataTable` | `DatasetController.filtered` (`EditableTableModel` via proxy) |

---

## Open questions for the team

1. Do we host QML through `QQmlApplicationEngine` for the whole app, or embed it with `QQuickWidget` inside a QWidgets shell? This decides `main.cpp`.
2. Qt Graphs or Qt Charts for the line chart and histogram? Test both with 100k rows in Sprint 2.
3. Map provider for `GeoMapPlugin`: Qt Location's `Map` type (may need a provider key) or a custom tile painter.
4. Target row count for "performance at scale" (10k, 100k, 1M).
