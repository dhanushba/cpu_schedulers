#include "DecisionTraceWidget.h"

#include <QAbstractItemView>
#include <QColor>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {

QString formatReadyProcesses(const std::vector<int> &readyPids) {
  if (readyPids.empty())
    return "None";

  QStringList processNames;
  for (const int pid : readyPids)
    processNames.append(QString("P%1").arg(pid));
  return processNames.join(", ");
}

} // namespace

DecisionTraceWidget::DecisionTraceWidget(QWidget *parent) : QWidget(parent) {
  auto *title = new QLabel("Scheduler Decision Trace", this);
  title->setObjectName("traceTitle");

  statusLabel = new QLabel("No scheduling decisions yet", this);
  statusLabel->setObjectName("traceStatus");

  auto *header = new QHBoxLayout();
  header->addWidget(title);
  header->addStretch();
  header->addWidget(statusLabel);

  traceTable = new QTableWidget(0, 4, this);
  traceTable->setObjectName("decisionTraceTable");
  traceTable->setHorizontalHeaderLabels(
      {"Time Slice", "Running", "Ready Processes", "Decision"});
  traceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  traceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  traceTable->setSelectionMode(QAbstractItemView::SingleSelection);
  traceTable->setAlternatingRowColors(true);
  traceTable->verticalHeader()->setVisible(false);
  traceTable->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);
  traceTable->horizontalHeader()->setSectionResizeMode(
      1, QHeaderView::ResizeToContents);
  traceTable->horizontalHeader()->setSectionResizeMode(
      2, QHeaderView::ResizeToContents);
  traceTable->horizontalHeader()->setSectionResizeMode(3,
                                                       QHeaderView::Stretch);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(14, 14, 14, 14);
  layout->setSpacing(12);
  layout->addLayout(header);
  layout->addWidget(traceTable, 1);
}

void DecisionTraceWidget::setSnapshots(
    const std::vector<SimulationSnapshot> &snapshots) {
  traceTable->setRowCount(static_cast<int>(snapshots.size()));

  for (int row = 0; row < static_cast<int>(snapshots.size()); row++) {
    const auto &snapshot = snapshots[row];
    auto *timeItem = new QTableWidgetItem(
        QString("%1-%2").arg(snapshot.time).arg(snapshot.time + 1));
    auto *runningItem = new QTableWidgetItem(
        snapshot.runningPid < 0 ? "CPU Idle"
                                : QString("P%1").arg(snapshot.runningPid));
    auto *readyItem =
        new QTableWidgetItem(formatReadyProcesses(snapshot.readyPids));
    auto *decisionItem =
        new QTableWidgetItem(QString::fromStdString(snapshot.decision));

    runningItem->setBackground(snapshot.runningPid < 0 ? QColor("#e5e7eb")
                                                        : QColor("#d1fae5"));
    if (!snapshot.readyPids.empty())
      readyItem->setBackground(QColor("#ffedd5"));

    traceTable->setItem(row, 0, timeItem);
    traceTable->setItem(row, 1, runningItem);
    traceTable->setItem(row, 2, readyItem);
    traceTable->setItem(row, 3, decisionItem);
  }

  statusLabel->setText(
      snapshots.empty() ? "No scheduling decisions yet"
                        : QString("%1 time slices").arg(snapshots.size()));
  if (!snapshots.empty())
    traceTable->scrollToBottom();
}