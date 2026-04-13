#pragma once

#include <QWidget>

class QLabel;

class MetricsWidget : public QWidget {
  Q_OBJECT

public:
  explicit MetricsWidget(QWidget *parent = nullptr);

  void setMetrics(int time, int runningPid, double avgWaiting,
                  double avgTurnaround);

private:
  QLabel *timeValue;
  QLabel *runningValue;
  QLabel *avgWaitValue;
  QLabel *avgTurnaroundValue;
};
