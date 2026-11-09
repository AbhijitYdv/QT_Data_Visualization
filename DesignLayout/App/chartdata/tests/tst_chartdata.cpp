// Tests for the summary, histogram and grouping helpers and the table model.

#include "chartdata.h"
#include "datasettablemodel.h"

#include <QAbstractItemModelTester>
#include <QTest>

#include <limits>
#include <numeric>

using namespace ChartData;

namespace {

constexpr int Text = static_cast<int>(CellType::Text);
constexpr int Number = static_cast<int>(CellType::Number);
constexpr int Date = static_cast<int>(CellType::Date);

// region,date,units
GraphTable salesTable()
{
    GraphTable t;
    t.headers = {"region", "date", "units"};
    t.types = {Text, Date, Number};
    t.rows = {
        {"North", "2026-01-03", "10"},
        {"South", "2026-01-01", "40"},
        {"North", "2026-01-02", "5"},
        {"East", "2026-01-05", ""},
        {"", "2026-01-04", "7"},
        {"West", "2026-01-06", "-3"},
    };
    return t;
}

QVector<int> allRows(const GraphTable &t)
{
    QVector<int> rows(t.rows.size());
    std::iota(rows.begin(), rows.end(), 0);
    return rows;
}

QStringList column(const DatasetTableModel &model, int col)
{
    QStringList out;
    for (int r = 0; r < model.rowCount(); ++r)
        out << model.data(model.index(r, col)).toString();
    return out;
}

} // namespace

class TestChartData : public QObject
{
    Q_OBJECT

private slots:
    // ------------------------------------------------------------ summary

    void summaryOddAndEven()
    {
        const Summary odd = summarize({5, 1, 3});
        QCOMPARE(odd.count, 3);
        QCOMPARE(odd.min, 1.0);
        QCOMPARE(odd.max, 5.0);
        QCOMPARE(odd.mean, 3.0);
        QCOMPARE(odd.median, 3.0);

        QCOMPARE(summarize({4, 1, 3, 2}).median, 2.5);
    }

    void summaryEmpty()
    {
        const Summary s = summarize({});
        QCOMPARE(s.count, 0);
        QCOMPARE(s.mean, 0.0);
    }

    // ---------------------------------------------------------- histogram

    void defaultBins()
    {
        QCOMPARE(defaultBinCount(0), 0);
        QCOMPARE(defaultBinCount(3), 5);     // floor of 5
        QCOMPARE(defaultBinCount(1000), 11); // 1 + ceil(log2 1000) = 11
        QCOMPARE(defaultBinCount(1 << 30), 31);
        QCOMPARE(defaultBinCount(std::numeric_limits<int>::max()), 32);
    }

    void histogramCountsEveryValue()
    {
        const Histogram h = histogram({0, 1, 2, 3, 4, 5, 6, 7, 8, 10}, 5);
        QCOMPARE(h.counts.size(), 5);
        QCOMPARE(h.edges.size(), 6);
        QCOMPARE(h.edges.first(), 0.0);
        QCOMPARE(h.edges.last(), 10.0);
        QCOMPARE(h.counts, (QVector<int>{2, 2, 2, 2, 2})); // 10 lands in the last bin
        QCOMPARE(h.maxCount, 2);
        QCOMPARE(binLabel(h, 0), QStringLiteral("0–2"));
        QCOMPARE(binLabel(h, 9), QString());
    }

    void histogramSingleValue()
    {
        const Histogram h = histogram({7, 7, 7}, 10);
        QCOMPARE(h.counts, QVector<int>{3});
        QCOMPARE(h.edges, (QVector<double>{6.5, 7.5}));
    }

    void histogramEmpty()
    {
        QVERIFY(histogram({}, 10).counts.isEmpty());
        QVERIFY(histogram({1, 2}, 0).counts.isEmpty());
    }

    void histogramNegativeAndFractional()
    {
        // Bins include their lower edge: -0.5 starts the second bin.
        const Histogram h = histogram({-1.5, -0.5, 0.25, 0.5}, 2);
        QCOMPARE(h.counts, (QVector<int>{1, 3}));
        QCOMPARE(h.edges.at(1), -0.5);
    }

    // ------------------------------------------------------------- groups

    void groupsByRowCount()
    {
        const GraphTable t = salesTable();
        const QVector<Group> g = groups(t, allRows(t), 0, -1);
        QCOMPARE(g.size(), 5);
        QCOMPARE(g.at(0).label, QStringLiteral("North"));
        QCOMPARE(g.at(0).value, 2.0);
        QCOMPARE(g.at(1).label, QStringLiteral("(blank)")); // ties sorted by name
    }

    void groupsBySumDropsNonPositive()
    {
        const GraphTable t = salesTable();
        const QVector<Group> g = groups(t, allRows(t), 0, 2);
        // South 40, North 15, (blank) 7; East has no number, West is negative.
        QCOMPARE(g.size(), 3);
        QCOMPARE(g.at(0).label, QStringLiteral("South"));
        QCOMPARE(g.at(1).value, 15.0);
        QCOMPARE(g.at(1).rows, 2);
    }

    void groupsMergeTheRestIntoOther()
    {
        const GraphTable t = salesTable();
        const QVector<Group> g = groups(t, allRows(t), 0, -1, 2);
        QCOMPARE(g.size(), 3);
        QVERIFY(g.last().isOther);
        QCOMPARE(g.last().label, QStringLiteral("Other"));
        QCOMPARE(g.last().value, 3.0); // East, South and West
    }

    void groupsOnlyGivenRows()
    {
        const GraphTable t = salesTable();
        const QVector<Group> g = groups(t, {0, 2}, 0, 2);
        QCOMPARE(g.size(), 1);
        QCOMPARE(g.first().value, 15.0);
        QVERIFY(groups(t, allRows(t), 9, -1).isEmpty());
    }

    // -------------------------------------------------------- table model

    void modelPassesQtModelTester()
    {
        DatasetTableModel model;
        QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::QtTest);
        const GraphTable t = salesTable();
        model.setSource(t, allRows(t));
        model.sortByColumn(2);
        model.setSource(t, {0, 1});
        model.clear();
    }

    void modelShowsFilteredRows()
    {
        DatasetTableModel model;
        const GraphTable t = salesTable();
        model.setSource(t, {1, 3});
        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.columnCount(), 3);
        QCOMPARE(model.count(), 2);
        QCOMPARE(model.headerData(2, Qt::Horizontal).toString(), QStringLiteral("units"));
        QCOMPARE(column(model, 0), (QStringList{"South", "East"}));
        QCOMPARE(model.data(model.index(0, 2), DatasetTableModel::NumericRole).toBool(), true);
        QCOMPARE(model.data(model.index(0, 0), DatasetTableModel::NumericRole).toBool(), false);
        QVERIFY(model.isNumericColumn(1));  // dates count as numeric
        QVERIFY(!model.isNumericColumn(0));
        QVERIFY(!model.isNumericColumn(7)); // out of range
    }

    void modelSortsNumbersWithBlanksLast()
    {
        DatasetTableModel model;
        const GraphTable t = salesTable();
        model.setSource(t, allRows(t));

        model.sortByColumn(2);
        QCOMPARE(column(model, 2), (QStringList{"-3", "5", "7", "10", "40", ""}));
        QVERIFY(model.sortAscending());

        model.sortByColumn(2); // same column again: descending
        QCOMPARE(column(model, 2), (QStringList{"40", "10", "7", "5", "-3", ""}));
        QVERIFY(!model.sortAscending());
    }

    void modelSortsDatesAndText()
    {
        DatasetTableModel model;
        const GraphTable t = salesTable();
        model.setSource(t, allRows(t));

        model.sortByColumn(1);
        QCOMPARE(column(model, 1).first(), QStringLiteral("2026-01-01"));
        QCOMPARE(column(model, 1).last(), QStringLiteral("2026-01-06"));

        model.sortByColumn(0);
        QCOMPARE(column(model, 0), (QStringList{"East", "North", "North", "South", "West", ""}));
    }

    void modelKeepsSortOnRefilterButResetsOnNewData()
    {
        DatasetTableModel model;
        const GraphTable t = salesTable();
        model.setSource(t, allRows(t));
        model.sortByColumn(2);
        model.setSource(t, {0, 1, 2}); // a filter change: same columns
        QCOMPARE(column(model, 2), (QStringList{"5", "10", "40"}));

        GraphTable other;
        other.headers = {"x"};
        other.types = {Number};
        other.rows = {{"2"}, {"1"}};
        model.setSource(other, {0, 1});
        QCOMPARE(model.sortColumn(), -1);
        QCOMPARE(column(model, 0), (QStringList{"2", "1"}));
    }
};

QTEST_GUILESS_MAIN(TestChartData)
#include "tst_chartdata.moc"
