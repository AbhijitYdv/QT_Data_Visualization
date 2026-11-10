#ifndef CHARTDATA_H
#define CHARTDATA_H

#include "graphtable.h"

#include <QList>
#include <QString>
#include <QVector>

// Plain calculations behind the summary cards, histogram and pie chart.
// No QObject or UI here, so every function is unit-tested on its own
// (tests/tst_chartdata.cpp).
namespace ChartData {

// ------------------------------------------------------------- summary

struct Summary
{
    int count = 0;       // values summarised
    double min = 0.0;
    double max = 0.0;
    double mean = 0.0;
    double median = 0.0;
};

// Takes the values by copy because the median needs them sorted.
Summary summarize(QVector<double> values);

// ----------------------------------------------------------- histogram

struct Histogram
{
    QVector<double> edges; // bins + 1 edges, lowest first
    QVector<int> counts;   // one count per bin
    int maxCount = 0;
};

// Sturges' rule (1 + log2 n), kept between 5 and 50. 0 for no values.
int defaultBinCount(int valueCount);

// Equal-width bins from min to max; the last bin includes the max.
// If every value is the same there is one bin, one unit wide.
Histogram histogram(const QVector<double> &values, int bins);

// "120–135" style label for bin `i`.
QString binLabel(const Histogram &histogram, int i);

// --------------------------------------------------------------- groups

struct Group
{
    QString label;
    double value = 0.0; // row count, or the sum of the value column
    int rows = 0;
    bool isOther = false;
};

// Groups `rows` of `table` by the text in `groupColumn`.
//  - valueColumn < 0: each group's value is its row count;
//  - otherwise: the sum of the numbers in valueColumn (non-numbers ignored).
// Groups are sorted largest first; past `maxGroups` the rest are merged into
// one "Other" group. Groups whose value is not above 0 are left out, because
// a pie slice cannot be negative. Empty text becomes "(blank)".
QVector<Group> groups(const GraphTable &table, const QVector<int> &rows,
                      int groupColumn, int valueColumn, int maxGroups = 7);

} // namespace ChartData

#endif // CHARTDATA_H
