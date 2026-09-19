#pragma once

#include <QWidget>
#include <vector>

#include "SimulationSnapshot.h"

class QLabel;
class QTableWidget;

class DecisionTraceWidget : public QWidget {
  Q_OBJECT

public:
  explicit DecisionTraceWidget(QWidget *parent = nullptr);

  void setSnapshots(const std::vector<SimulationSnapshot> &snapshots);

private:
  QLabel *statusLabel;
  QTableWidget *traceTable;
};