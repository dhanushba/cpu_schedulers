#include "DecisionTraceWidget.h"

#include <QApplication>
#include <QTableWidget>

#include <iostream>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  DecisionTraceWidget widget;

  auto *table = widget.findChild<QTableWidget *>("decisionTraceTable");
  if (!table || table->rowCount() != 0) {
    std::cerr << "FAIL: decision trace did not start empty\n";
    return 1;
  }

  widget.setSnapshots({{0, 1, {}, "P1 ran: earliest arrival time"},
                       {1, 2, {1, 3},
                        "P2 ran: shortest remaining time"},
                       {2, -1, {}, "CPU idle: no process was ready"}});

  if (table->rowCount() != 3 || table->item(0, 0)->text() != "0-1" ||
      table->item(0, 1)->text() != "P1" ||
      table->item(1, 2)->text() != "P1, P3" ||
      table->item(2, 1)->text() != "CPU Idle") {
    std::cerr << "FAIL: decision snapshots were not rendered correctly\n";
    return 1;
  }

  widget.setSnapshots({});
  if (table->rowCount() != 0) {
    std::cerr << "FAIL: clearing snapshots did not clear the trace\n";
    return 1;
  }

  std::cout << "Decision trace widget tests passed.\n";
  return 0;
}