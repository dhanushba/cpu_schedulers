#pragma once

#include <QWidget>
#include <vector>

#include "Process.h"

class QTableWidget;
class QPushButton;
class QLabel;

class ProcessTableWidget : public QWidget {
  Q_OBJECT

public:
  explicit ProcessTableWidget(QWidget *parent = nullptr);

  void setProcesses(const std::vector<Process> &processes);

signals:
  void deleteProcessRequested(int pid);

private:
  QTableWidget *table;
  QPushButton *deleteSelectedButton;
  QLabel *emptyStateLabel;
};
