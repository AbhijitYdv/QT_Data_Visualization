#include "usmapitem.h"
#include "usmapdata.h"

#include <QPainter>
#include <QQuickWindow>

#include <cmath>

namespace {

constexpr double kCell = 12.0;       // hover grid cell, px
constexpr double kHoverRadius = 6.0; // px
constexpr int kMaxDrawn = 20000;     // more points are drawn as an even sample

const QColor kStateFill("#232a2e");
const QColor kStateBorder("#3a444a");
const QColor kStateHover("#2a3236");
const QColor kDot("#4f9a86");
const QColor kDotBorder("#171b1e");
const QColor kHoverRing("#e6e8ea");

} // namespace

UsMapItem::UsMapItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setAcceptHoverEvents(true);
    setAntialiasing(true);
}

void UsMapItem::setPoints(const QVector<QPointF> &lonLat, const QVector<int> &rows)
{
    m_projected.clear();
    m_projected.reserve(lonLat.size());
    for (const QPointF &p : lonLat)
        m_projected << UsMap::project(p.x(), p.y());
    m_rows = rows;

    m_hoverIndex = -1;
    m_hoverStateIndex = -1;
    m_hoverState.clear();
    updateLayout();
    emit pointsChanged();
    emit hoverChanged();
    update();
}

void UsMapItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);
    if (newGeometry.size() != oldGeometry.size()) {
        updateLayout();
        update();
    }
}

// Fits the map into the item (keeping its shape), then moves the dots,
// rebuilds the hover grid and redraws the cached state outlines.
void UsMapItem::updateLayout()
{
    const UsMap::Outline &outline = UsMap::bundledOutline();
    const QRectF area = boundingRect().adjusted(8, 8, -8, -8);
    if (outline.states.isEmpty() || area.width() <= 0 || area.height() <= 0) {
        m_screen.clear();
        m_grid.clear();
        m_statesImage = QImage();
        return;
    }

    const QRectF b = outline.bounds;
    const double scale = std::min(area.width() / b.width(), area.height() / b.height());
    m_toScreen = QTransform();
    m_toScreen.translate(area.center().x(), area.center().y());
    m_toScreen.scale(scale, -scale); // map y grows north, screen y grows down
    m_toScreen.translate(-b.center().x(), -b.center().y());

    m_screen.resize(m_projected.size());
    m_grid.clear();
    for (int i = 0; i < m_projected.size(); ++i) {
        const QPointF s = m_toScreen.map(m_projected.at(i));
        m_screen[i] = s;
        m_grid[cellKey(int(s.x() / kCell), int(s.y() / kCell))] << i;
    }

    // States, drawn once per size at the screen's pixel ratio.
    const qreal dpr = window() ? window()->effectiveDevicePixelRatio() : 1.0;
    m_statesImage = QImage((size() * dpr).toSize(), QImage::Format_ARGB32_Premultiplied);
    m_statesImage.setDevicePixelRatio(dpr);
    m_statesImage.fill(Qt::transparent);
    QPainter p(&m_statesImage);
    p.setRenderHint(QPainter::Antialiasing);
    p.setTransform(m_toScreen);
    p.setPen(QPen(kStateBorder, 0.8 / scale)); // 0.8 px whatever the zoom
    p.setBrush(kStateFill);
    for (const UsMap::State &state : outline.states)
        p.drawPath(state.path);
}

void UsMapItem::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);
    if (!m_statesImage.isNull())
        painter->drawImage(QPointF(0, 0), m_statesImage);

    // The hovered state, slightly lighter.
    const UsMap::Outline &outline = UsMap::bundledOutline();
    if (m_hoverIndex < 0 && m_hoverStateIndex >= 0) {
        painter->save();
        painter->setTransform(m_toScreen, true);
        painter->setPen(QPen(kDot, 1.2 / m_toScreen.m11()));
        painter->setBrush(kStateHover);
        painter->drawPath(outline.states.at(m_hoverStateIndex).path);
        painter->restore();
    }

    // Dots: smaller as there are more of them; sampled past kMaxDrawn.
    const int n = static_cast<int>(m_screen.size());
    const double r = n > 10000 ? 1.6 : n > 2000 ? 2.5 : 3.5;
    const int step = std::max(1, n / kMaxDrawn);
    QColor fill = kDot;
    fill.setAlphaF(n > 2000 ? 0.6 : 0.9);
    painter->setPen(n > 2000 ? Qt::NoPen : QPen(kDotBorder, 1.0));
    painter->setBrush(fill);
    for (int i = 0; i < n; i += step)
        painter->drawEllipse(m_screen.at(i), r, r);

    if (m_hoverIndex >= 0) {
        painter->setPen(QPen(kHoverRing, 2.0));
        painter->setBrush(kDot);
        painter->drawEllipse(m_screen.at(m_hoverIndex), r + 2.5, r + 2.5);
    }
}

quint64 UsMapItem::cellKey(int cx, int cy)
{
    return (quint64(quint32(cx)) << 32) | quint32(cy);
}

int UsMapItem::nearestPoint(const QPointF &pos) const
{
    const int cx = int(pos.x() / kCell), cy = int(pos.y() / kCell);
    int best = -1;
    double bestDist = kHoverRadius * kHoverRadius;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            const auto it = m_grid.constFind(cellKey(cx + dx, cy + dy));
            if (it == m_grid.constEnd())
                continue;
            for (int i : it.value()) {
                const QPointF d = m_screen.at(i) - pos;
                const double dist = d.x() * d.x() + d.y() * d.y();
                if (dist <= bestDist) {
                    bestDist = dist;
                    best = i;
                }
            }
        }
    }
    return best;
}

void UsMapItem::hoverMoveEvent(QHoverEvent *event)
{
    const QPointF pos = event->position();
    const int point = nearestPoint(pos);
    int state = -1;
    if (point < 0)
        state = UsMap::bundledOutline().stateAt(m_toScreen.inverted().map(pos));

    const bool changed = point != m_hoverIndex || state != m_hoverStateIndex;
    m_hoverIndex = point;
    m_hoverStateIndex = state;
    m_hoverState = state >= 0 ? UsMap::bundledOutline().states.at(state).name : QString();
    m_hoverPos = pos; // the tooltip follows the cursor
    emit hoverChanged();
    if (changed)
        update();
}

void UsMapItem::hoverLeaveEvent(QHoverEvent *event)
{
    Q_UNUSED(event)
    m_hoverIndex = -1;
    m_hoverStateIndex = -1;
    m_hoverState.clear();
    emit hoverChanged();
    update();
}
