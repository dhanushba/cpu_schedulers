#include "ControlPanelWidget.h"
#include "ProcessTableWidget.h"

#include <QAbstractButton>
#include <QApplication>
#include <QComboBox>
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
  auto *algorithm = controls.findChild<QComboBox *>("algorithmCombo");
  auto *priority = controls.findChild<QSpinBox *>("prioritySpin");
  if (!algorithm || !priority || !priority->isHidden()) {
    std::cerr << "FAIL: priority input was visible for FCFS\n";
    return 1;
  }
  priority->setValue(4);
  algorithm->setCurrentIndex(2);
    if (!priority->isVisibleTo(&controls) || !priority->isEnabled() ||
      priority->value() != 4 ||
      !priority->toolTip().contains("Lower")) {
    std::cerr << "FAIL: Priority mode did not enable the preserved value\n";
    return 1;
  }
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
  if (!priority || !priority->isVisibleTo(&controls) ||
      !priority->toolTip().contains("Lower")) {
    std::cerr << "FAIL: workload priority was not clearly available\n";
    return 1;
  }
  auto *burst = controls.findChild<QSpinBox *>("burstSpin");
  auto *arrival = controls.findChild<QSpinBox *>("arrivalSpin");
  QPushButton *addProcess = nullptr;
  for (auto *button : controls.findChildren<QPushButton *>()) {
    if (button->text() == "Add Process") {
      addProcess = button;
      break;
    }
  }
  if (!burst || !arrival || !addProcess) {
    std::cerr << "FAIL: process input controls were not available\n";
    return 1;
  }
  int emittedPriority = -1;
  QObject::connect(&controls, &ControlPanelWidget::addProcessClicked,
                   [&emittedPriority](int, int processPriority, int) {
                     emittedPriority = processPriority;
                   });
  priority->setValue(4);
  burst->setValue(3);
  arrival->setValue(0);
  addProcess->click();
  if (emittedPriority != 4) {
    std::cerr << "FAIL: process input did not emit the selected priority\n";
    return 1;
  }

  ProcessTableWidget processTable;
  processTable.setAlgorithmName("Priority");
  auto *table = processTable.findChild<QTableWidget *>();
  auto *emptyState = processTable.findChild<QLabel *>("processEmptyState");
  if (!table || !emptyState || !emptyState->isVisibleTo(&processTable)) {
    std::cerr << "FAIL: empty process state was not configured correctly\n";
    return 1;
  }

  std::vector<Process> displayedProcesses{Process(1, 0, 3, 4),
                                          Process(2, 1, 5, 2)};
  processTable.setProcesses(displayedProcesses);
  if (!table->isVisibleTo(&processTable) || emptyState->isVisibleTo(&processTable)) {
    std::cerr << "FAIL: process table did not replace its empty state\n";
    return 1;
  }
      if (table->horizontalHeaderItem(0)->text() != "PID (Priority)" ||
        table->item(0, 0)->text() != "1" ||
        table->item(0, 0)->data(Qt::UserRole).toInt() != 1 ||
      table->item(0, 3)->text() != "4" ||
      table->item(1, 3)->text() != "2") {
    std::cerr << "FAIL: process table did not display process priority\n";
    return 1;
  }

  processTable.setAlgorithmName("Round Robin");
  displayedProcesses.emplace_back(3, 2, 4, 1);
  processTable.setProcesses(displayedProcesses);
  if (table->horizontalHeaderItem(0)->text() != "PID (Round Robin)" ||
      table->item(0, 0)->text() != "1" ||
      table->item(1, 0)->text() != "2" ||
      table->item(2, 0)->text() != "3" ||
      table->item(0, 3)->text() != "-" ||
      table->item(2, 3)->text() != "-" ||
      table->item(2, 3)->data(Qt::UserRole).toInt() != 1) {
    std::cerr << "FAIL: process table did not preserve process algorithms\n";
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
  if (editButton->isEnabled() || duplicateButton->isEnabled() ||
      deleteButton->isEnabled()) {
    std::cerr << "FAIL: process actions remained enabled after simulation start\n";
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