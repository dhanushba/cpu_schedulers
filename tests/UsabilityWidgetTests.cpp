#include "ControlPanelWidget.h"
#include "ProcessTableWidget.h"

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>

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
  auto *deleteButton = processTable.findChild<QPushButton *>("dangerAction");
  if (!table || !emptyState || !deleteButton || !emptyState->isVisibleTo(&processTable) ||
      deleteButton->isEnabled()) {
    std::cerr << "FAIL: empty process state was not configured correctly\n";
    return 1;
  }

  processTable.setProcesses({Process(1, 0, 3)});
  if (!table->isVisibleTo(&processTable) || emptyState->isVisibleTo(&processTable)) {
    std::cerr << "FAIL: process table did not replace its empty state\n";
    return 1;
  }

  std::cout << "Usability widget tests passed.\n";
  return 0;
}