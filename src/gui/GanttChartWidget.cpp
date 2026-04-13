#include "GanttChartWidget.h"

#include <QPainter>
#include <QPaintEvent>

GanttChartWidget::GanttChartWidget(QWidget *parent) : QWidget(parent), now(0) {
  setMinimumHeight(180);
  setContentsMargins(8, 8, 8, 8);
}

void GanttChartWidget::setTimeline(const std::vector<ExecutionRecord> &records,
                                   int currentTime) {
  timeline = records;
  now = currentTime;
  update();
}

void GanttChartWidget::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);

  const QRect bounds = rect().adjusted(12, 12, -12, -12);
  p.fillRect(rect(), QColor("#f8fbff"));

  p.setPen(QPen(QColor("#c9d5e2"), 1));
  p.setBrush(Qt::NoBrush);
  p.drawRoundedRect(bounds, 10, 10);

  if (timeline.empty()) {
    p.setPen(QColor("#6b7280"));
    p.drawText(bounds, Qt::AlignCenter,
               "No execution yet. Start simulation and add processes.");
    return;
  }

  const int totalTime = now > 0 ? now : timeline.back().endTime;
  const double unitW = totalTime > 0 ? (bounds.width() - 20.0) / totalTime : 1.0;

  const int y = bounds.top() + 20;
  const int h = 54;

  for (const auto &r : timeline) {
    const int x = bounds.left() + 10 + static_cast<int>(r.startTime * unitW);
    const int w =
        qMax(2, static_cast<int>((r.endTime - r.startTime) * unitW) - 1);

    const QColor c = colorForPid(r.pid);
    p.setBrush(c);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(QRect(x, y, w, h), 6, 6);

    p.setPen(QColor("#111827"));
    const QString label = r.pid == -1 ? "Idle" : QString("P%1").arg(r.pid);
    p.drawText(QRect(x, y, w, h), Qt::AlignCenter, label);

    p.setPen(QColor("#374151"));
    p.drawText(x, y + h + 18, QString::number(r.startTime));
  }

  p.setPen(QColor("#374151"));
  p.drawText(bounds.right() - 30, y + h + 18, QString::number(totalTime));
}

QColor GanttChartWidget::colorForPid(int pid) {
  if (pid == -1)
    return QColor("#cbd5e1");

  if (colorMap.contains(pid))
    return colorMap[pid];

  static const QList<QColor> palette = {
      QColor("#60a5fa"), QColor("#34d399"), QColor("#f59e0b"),
      QColor("#f87171"), QColor("#a78bfa"), QColor("#22d3ee"),
      QColor("#fb7185"), QColor("#4ade80")};

  const QColor c = palette[pid % palette.size()];
  colorMap.insert(pid, c);
  return c;
}
