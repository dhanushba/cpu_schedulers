#pragma once

#include <QWidget>
#include <vector>

#include "Process.h"

class QTableWidget;
class QLabel;
class QPushButton;

class ProcessTableWidget : public QWidget {
  Q_OBJECT

public:
  explicit ProcessTableWidget(QWidget *parent = nullptr);

  void setProcesses(const std::vector<Process> &processes);
  void setEditingEnabled(bool enabled);

signals:
  void editProcessRequested(int pid, int burst, int priority, int arrival);
  void duplicateProcessRequested(int burst, int priority, int arrival);
  void deleteProcessRequested(int pid);

private:
  int selectedRow() const;
  void updateActionState();
  void editSelectedProcess();

  QTableWidget *table;
  QLabel *emptyStateLabel;
  QPushButton *editButton;
  QPushButton *duplicateButton;
  QPushButton *deleteButton;
  bool editingEnabled = true;
};
