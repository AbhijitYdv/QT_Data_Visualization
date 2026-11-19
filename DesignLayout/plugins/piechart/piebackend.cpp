#include "piebackend.h"
#include "chartdata.h"
#include "modelaccess.h"

#include <QCoreApplication>
#include <QMetaObject>

#include <limits>

PieBackend::PieBackend(QAbstractItemModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
{
    ModelAccess::onAnyChange(m_model, this, [this] { scheduleRefresh(); });
    scheduleRefresh();
}

void PieBackend::setGroupColumn(const QString &name)
{
    if (name == m_chosenGroup)
        return;
    m_chosenGroup = name;
    scheduleRefresh();
}

void PieBackend::setSumsY(bool sums)
{
    if (sums == m_sumsY)
        return;
    m_sumsY = sums;
    scheduleRefresh();
}

void PieBackend::setYColumn(const QString &name)
{
    if (name == m_yColumn)
        return;
    m_yColumn = name;
    emit inputsChanged();
    scheduleRefresh();
}

void PieBackend::setFilterColumn(const QString &name)
{
    if (name == m_filterColumn)
        return;
    m_filterColumn = name;
    emit inputsChanged();
    scheduleRefresh();
}

void PieBackend::setFilterValue(const QString &value)
{
    if (value == m_filterValue)
        return;
    m_filterValue = value;
    emit inputsChanged();
    scheduleRefresh();
}

void PieBackend::setAllRowsModel(QAbstractItemModel *model)
{
    if (model == m_allRows)
        return;
    if (m_allRows)
        disconnect(m_allRows, nullptr, this, nullptr);
    m_allRows = model;
    ModelAccess::onAnyChange(m_allRows, this, [this] { scheduleRefresh(); });
    emit inputsChanged();
    scheduleRefresh();
}

void PieBackend::scheduleRefresh()
{
    if (m_queued)
        return;
    m_queued = true;
    QMetaObject::invokeMethod(this, [this] { refresh(); }, Qt::QueuedConnection);
}

void PieBackend::refresh()
{
    m_queued = false;
    m_labels.clear();
    m_values.clear();
    m_rows.clear();
    m_highlight = -1;
    m_hasOther = false;
    m_total = 0.0;

    // A different dataset goes back to the automatic group column.
    const QStringList headers = ModelAccess::headers(m_model);
    if (headers != m_lastHeaders) {
        if (!m_lastHeaders.isEmpty()) // not for the first dataset
            m_chosenGroup.clear();
        m_lastHeaders = headers;
    }

    m_groupOptions = ModelAccess::columnsOfType(m_model, ModelAccess::Text);
    if (m_groupOptions.contains(m_chosenGroup))
        m_effectiveGroup = m_chosenGroup;
    else if (m_groupOptions.contains(m_filterColumn))
        m_effectiveGroup = m_filterColumn;
    else
        m_effectiveGroup = m_groupOptions.value(0);

    const bool sums = m_sumsY && ModelAccess::column(m_model, m_yColumn) >= 0;
    m_valueName = sums ? tr("Sum of %1").arg(m_yColumn) : tr("Rows");

    // Grouping by the filter column: every group, with the filtered one marked.
    const bool byFilter = !m_effectiveGroup.isEmpty() && m_effectiveGroup == m_filterColumn
                          && m_allRows;
    QAbstractItemModel *source = byFilter ? m_allRows.data() : m_model.data();
    const int groupCol = ModelAccess::column(source, m_effectiveGroup);
    const int yCol = ModelAccess::column(source, m_yColumn);
    const int valueRole = ModelAccess::role(source, "value");

    if (source && groupCol >= 0) {
        const int n = source->rowCount();
        QStringList labels;
        QVector<double> values;
        labels.reserve(n);
        if (sums)
            values.reserve(n);
        for (int r = 0; r < n; ++r) {
            labels << source->index(r, groupCol).data().toString();
            if (sums) {
                const QVariant v = yCol >= 0 && valueRole >= 0
                                       ? source->index(r, yCol).data(valueRole) : QVariant();
                values << (v.isValid() ? v.toDouble() : std::numeric_limits<double>::quiet_NaN());
            }
        }

        const QString filterLabel = m_filterValue.isEmpty()
                                        ? QCoreApplication::translate("ChartData", "(blank)")
                                        : m_filterValue;
        const auto groups = ChartData::groups(labels, values);
        for (const ChartData::Group &g : groups) {
            if (byFilter && !g.isOther && g.label == filterLabel)
                m_highlight = static_cast<int>(m_labels.size());
            m_labels << g.label;
            m_values << g.value;
            m_rows << g.rows;
            m_total += g.value;
            m_hasOther = g.isOther;
        }
    }

    emit changed();
}
