#include "ControlPanelWidget.h"
#include "ProcessTableWidget.h"

#include <QAbstractButton>
#include <QApplication>
#include <QItemSelectionModel>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTimer>

#include <iostream>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  ControlPanelWidget controls;
  auto *status = controls.findChild<QLabel *>("runtimeStatus");
  if (!status || status->text() != "Ready") {
    std::cerr << "FAIL: control panel did not start ready\n";
    return 1;
  }
  controls.setRuntimeState(true, true, false);
  if (status->text() != "Running") {
    std::cerr << "FAIL: running state was not displayed\n";
    return 1;
  }
  controls.setRuntimeState(true, false, true);
  if (status->text() != "Paused") {
    std::cerr << "FAIL: paused state was not displayed\n";
    return 1;
  }
  auto *priority = controls.findChild<QSpinBox *>("prioritySpin");
  if (!priority || !priority->isVisibleTo(&controls) ||
      !priority->toolTip().contains("Lower")) {
    std::cerr << "FAIL: workload priority was not clearly available\n";
    return 1;
  }

  ProcessTableWidget processTable;
  auto *table = processTable.findChild<QTableWidget *>();
  auto *emptyState = processTable.findChild<QLabel *>("processEmptyState");
  if (!table || !emptyState || !emptyState->isVisibleTo(&processTable)) {
    std::cerr << "FAIL: empty process state was not configured correctly\n";
    return 1;
  }

  processTable.setProcesses({Process(1, 0, 3), Process(2, 1, 5)});
  if (!table->isVisibleTo(&processTable) || emptyState->isVisibleTo(&processTable)) {
    std::cerr << "FAIL: process table did not replace its empty state\n";
    return 1;
  }

  auto *editButton =
      processTable.findChild<QPushButton *>("editProcessButton");
  auto *duplicateButton =
      processTable.findChild<QPushButton *>("duplicateProcessButton");
  auto *deleteButton =
      processTable.findChild<QPushButton *>("deleteProcessButton");
    auto *selectionStatus =
      processTable.findChild<QLabel *>("processSelectionStatus");
  if (!editButton || !duplicateButton || !deleteButton ||
      !selectionStatus ||
      editButton->isEnabled() || duplicateButton->isEnabled() ||
      deleteButton->isEnabled()) {
    std::cerr << "FAIL: process action toolbar was not configured correctly\n";
    return 1;
  }
  table->selectRow(0);
  if (!editButton->isEnabled() || !duplicateButton->isEnabled() ||
      !deleteButton->isEnabled()) {
    std::cerr << "FAIL: selecting a process did not enable its actions\n";
    return 1;
  }

  int duplicatedBurst = 0;
  int deletedPid = 0;
  QObject::connect(&processTable, &ProcessTableWidget::duplicateProcessRequested,
                   [&duplicatedBurst](int burst, int, int) {
                     duplicatedBurst = burst;
                   });
  QObject::connect(&processTable, &ProcessTableWidget::deleteProcessRequested,
                   [&deletedPid](int pid) { deletedPid = pid; });
  duplicateButton->click();
  deleteButton->click();
  if (duplicatedBurst != 3 || deletedPid != 1) {
    std::cerr << "FAIL: process row actions emitted incorrect values\n";
    return 1;
  }

  processTable.setEditingEnabled(false);
  if (editButton->isEnabled() || !duplicateButton->isEnabled() ||
      !deleteButton->isEnabled()) {
    std::cerr << "FAIL: editing remained enabled after simulation start\n";
    return 1;
  }

  processTable.setEditingEnabled(true);
  table->clearSelection();
  table->selectionModel()->select(
      table->model()->index(0, 0),
      QItemSelectionModel::Select | QItemSelectionModel::Rows);
  table->selectionModel()->select(
      table->model()->index(1, 0),
      QItemSelectionModel::Select | QItemSelectionModel::Rows);
  if (editButton->isEnabled() || duplicateButton->isEnabled() ||
      !deleteButton->isEnabled() || deleteButton->text() != "Delete 2" ||
      selectionStatus->text() != "2 selected") {
    std::cerr << "FAIL: multi-selection action state was incorrect\n";
    return 1;
  }

  std::vector<int> deletedPids;
  QObject::connect(&processTable,
                   &ProcessTableWidget::deleteProcessesRequested,
                   [&deletedPids](const std::vector<int> &pids) {
                     deletedPids = pids;
                   });
  QTimer::singleShot(0, []() {
    auto *dialog = qobject_cast<QMessageBox *>(QApplication::activeModalWidget());
    if (dialog)
      dialog->button(QMessageBox::Yes)->click();
  });
  deleteButton->click();
  if (deletedPids != std::vector<int>({1, 2})) {
    std::cerr << "FAIL: bulk delete emitted incorrect process IDs\n";
    return 1;
  }

  std::cout << "Usability widget tests passed.\n";
  return 0;
}