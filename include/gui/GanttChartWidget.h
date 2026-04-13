#pragma once

#include <QColor>
#include <QHash>
#include <QWidget>
#include <vector>

#include "ExecutionRecord.h"

class GanttChartWidget : public QWidget {
  Q_OBJECT

public:
  explicit GanttChartWidget(QWidget *parent = nullptr);

  void setTimeline(const std::vector<ExecutionRecord> &records, int currentTime);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  QColor colorForPid(int pid);

  std::vector<ExecutionRecord> timeline;
  int now;
  QHash<int, QColor> colorMap;
};
