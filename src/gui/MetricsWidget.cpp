#include "MetricsWidget.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

MetricsWidget::MetricsWidget(QWidget *parent) : QWidget(parent) {
  auto *box = new QGroupBox("Live Metrics", this);
  auto *form = new QFormLayout(box);

  timeValue = new QLabel("0", box);
  runningValue = new QLabel("CPU Idle", box);
  avgWaitValue = new QLabel("0.00", box);
  avgTurnaroundValue = new QLabel("0.00", box);

  form->addRow("Simulation Time", timeValue);
  form->addRow("Current PID", runningValue);
  form->addRow("Avg Waiting", avgWaitValue);
  form->addRow("Avg Turnaround", avgTurnaroundValue);

  auto *layout = new QVBoxLayout(this);
  layout->addWidget(box);
}

void MetricsWidget::setMetrics(int time, int runningPid, double avgWaiting,
                               double avgTurnaround) {
  timeValue->setText(QString::number(time));
  runningValue->setText(runningPid == -1 ? "CPU Idle"
                                          : QString("P%1").arg(runningPid));
  avgWaitValue->setText(QString::number(avgWaiting, 'f', 2));
  avgTurnaroundValue->setText(QString::number(avgTurnaround, 'f', 2));
}
