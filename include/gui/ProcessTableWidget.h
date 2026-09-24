#pragma once

#include <QString>
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

  void setAlgorithmName(const QString &name);
  void setProcesses(const std::vector<Process> &processes);
  void setEditingEnabled(bool enabled);

signals:
  void editProcessRequested(int pid, int burst, int priority, int arrival);
  void duplicateProcessRequested(int burst, int priority, int arrival);
  void deleteProcessRequested(int pid);
  void deleteProcessesRequested(const std::vector<int> &pids);

private:
  std::vector<int> selectedRows() const;
  void updateActionState();
  void editSelectedProcess();
  void deleteSelectedProcesses();

  QTableWidget *table;
  QLabel *emptyStateLabel;
  QLabel *selectionLabel;
  QPushButton *editButton;
  QPushButton *duplicateButton;
  QPushButton *deleteButton;
  QString algorithmName;
  bool editingEnabled = true;
};
