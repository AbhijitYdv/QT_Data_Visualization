#include "chartdata.h"
#include "parserutils.h"

#include <QCoreApplication>
#include <QHash>

#include <algorithm>
#include <cmath>
#include <numeric>

namespace ChartData {

Summary summarize(QVector<double> values)
{
    Summary s;
    s.count = static_cast<int>(values.size());
    if (values.isEmpty())
        return s;

    std::sort(values.begin(), values.end());
    s.min = values.first();
    s.max = values.last();
    s.mean = std::accumulate(values.cbegin(), values.cend(), 0.0) / s.count;
    const int mid = s.count / 2;
    s.median = (s.count % 2) ? values.at(mid) : (values.at(mid - 1) + values.at(mid)) / 2.0;
    return s;
}

// ------------------------------------------------------------ histogram

int defaultBinCount(int valueCount)
{
    if (valueCount <= 0)
        return 0;
    const int sturges = 1 + static_cast<int>(std::ceil(std::log2(valueCount)));
    return std::clamp(sturges, 5, 50);
}

Histogram histogram(const QVector<double> &values, int bins)
{
    Histogram h;
    if (values.isEmpty() || bins < 1)
        return h;

    const auto [lowIt, highIt] = std::minmax_element(values.cbegin(), values.cend());
    double low = *lowIt;
    double high = *highIt;
    if (low == high) { // one value only: a single bin around it
        bins = 1;
        low -= 0.5;
        high += 0.5;
    }

    const double width = (high - low) / bins;
    h.edges.reserve(bins + 1);
    for (int i = 0; i < bins; ++i)
        h.edges << low + i * width;
    h.edges << high; // exact, no rounding drift on the last edge

    h.counts = QVector<int>(bins, 0);
    for (double v : values) {
        const int bin = std::min(static_cast<int>((v - low) / width), bins - 1);
        ++h.counts[bin];
    }
    h.maxCount = *std::max_element(h.counts.cbegin(), h.counts.cend());
    return h;
}

QString binLabel(const Histogram &histogram, int i)
{
    if (i < 0 || i + 1 >= histogram.edges.size())
        return {};
    auto fmt = [](double v) { return QString::number(v, 'g', 4); };
    return QStringLiteral("%1–%2").arg(fmt(histogram.edges.at(i)), fmt(histogram.edges.at(i + 1)));
}

// --------------------------------------------------------------- groups

QVector<Group> groups(const GraphTable &table, const QVector<int> &rows,
                      int groupColumn, int valueColumn, int maxGroups)
{
    if (groupColumn < 0 || groupColumn >= table.headers.size())
        return {};

    const QString blank = QCoreApplication::translate("ChartData", "(blank)");
    QHash<QString, int> indexOf;
    QVector<Group> all;

    for (int r : rows) {
        if (r < 0 || r >= table.rows.size())
            continue;
        const QStringList &row = table.rows.at(r);
        QString label = row.value(groupColumn).trimmed();
        if (label.isEmpty())
            label = blank;

        auto it = indexOf.constFind(label);
        int g;
        if (it == indexOf.constEnd()) {
            g = static_cast<int>(all.size());
            indexOf.insert(label, g);
            all.append(Group{label, 0.0, 0, false});
        } else {
            g = it.value();
        }

        Group &group = all[g];
        ++group.rows;
        if (valueColumn < 0) {
            group.value += 1.0;
        } else {
            double v = 0.0;
            if (Parsers::parseNumber(row.value(valueColumn), &v))
                group.value += v;
        }
    }

    all.removeIf([](const Group &g) { return !(g.value > 0.0); });
    std::stable_sort(all.begin(), all.end(), [](const Group &a, const Group &b) {
        return a.value != b.value ? a.value > b.value : a.label < b.label;
    });

    if (maxGroups > 0 && all.size() > maxGroups) {
        Group other{QCoreApplication::translate("ChartData", "Other"), 0.0, 0, true};
        for (qsizetype i = maxGroups; i < all.size(); ++i) {
            other.value += all.at(i).value;
            other.rows += all.at(i).rows;
        }
        all.resize(maxGroups);
        all.append(other);
    }
    return all;
}

} // namespace ChartData
