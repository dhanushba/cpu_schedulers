#include "ControlPanelWidget.h"
#include "ProcessTableWidget.h"

#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QTableWidget>
#include <QToolButton>

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

  ProcessTableWidget processTable;
  auto *table = processTable.findChild<QTableWidget *>();
  auto *emptyState = processTable.findChild<QLabel *>("processEmptyState");
  if (!table || !emptyState || !emptyState->isVisibleTo(&processTable)) {
    std::cerr << "FAIL: empty process state was not configured correctly\n";
    return 1;
  }

  processTable.setProcesses({Process(1, 0, 3)});
  if (!table->isVisibleTo(&processTable) || emptyState->isVisibleTo(&processTable)) {
    std::cerr << "FAIL: process table did not replace its empty state\n";
    return 1;
  }

  auto *actionsButton =
      processTable.findChild<QToolButton *>("processActionsButton");
  auto *editAction = processTable.findChild<QAction *>("editProcessAction");
  auto *duplicateAction =
      processTable.findChild<QAction *>("duplicateProcessAction");
  auto *deleteAction =
      processTable.findChild<QAction *>("deleteProcessAction");
  if (!actionsButton || !actionsButton->menu() || !editAction ||
      !duplicateAction || !deleteAction) {
    std::cerr << "FAIL: process row actions were not configured correctly\n";
    return 1;
  }
  table->setCurrentCell(0, 0);
  if (actionsButton->isHidden()) {
    std::cerr << "FAIL: selecting a process did not reveal its actions\n";
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
  duplicateAction->trigger();
  deleteAction->trigger();
  if (duplicatedBurst != 3 || deletedPid != 1) {
    std::cerr << "FAIL: process row actions emitted incorrect values\n";
    return 1;
  }

  processTable.setEditingEnabled(false);
  if (editAction->isEnabled()) {
    std::cerr << "FAIL: editing remained enabled after simulation start\n";
    return 1;
  }

  std::cout << "Usability widget tests passed.\n";
  return 0;
}