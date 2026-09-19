#include "ComparisonWidget.h"

#include <QApplication>
#include <QPushButton>
#include <QTableWidget>

#include <iostream>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  ComparisonWidget widget;

  bool requested = false;
  QObject::connect(&widget, &ComparisonWidget::compareRequested,
                   [&requested]() { requested = true; });

  auto *button = widget.findChild<QPushButton *>("compareAllButton");
  auto *table = widget.findChild<QTableWidget *>("comparisonTable");
  if (!button || !table) {
    std::cerr << "FAIL: comparison controls were not created\n";
    return 1;
  }

  button->click();
  if (!requested) {
    std::cerr << "FAIL: Compare All did not emit compareRequested\n";
    return 1;
  }

  ComparisonResult result{SchedulerMode::FCFS, "FCFS", {}, {}, 2.0, 6.0,
                          100.0, 1};
  ComparisonResult slower{SchedulerMode::RoundRobin, "Round Robin", {}, {},
                          3.0, 7.0, 80.0, 4};
  widget.setResults({result, slower});

  if (table->rowCount() != 2 || table->item(0, 0)->text() != "FCFS" ||
      table->item(0, 3)->text() != "100.0%" ||
      table->item(0, 4)->text() != "1") {
    std::cerr << "FAIL: comparison result was not rendered correctly\n";
    return 1;
  }

  if (!table->item(0, 1)->font().bold() ||
      table->item(1, 1)->font().bold()) {
    std::cerr << "FAIL: best comparison metric was not highlighted\n";
    return 1;
  }

  std::cout << "Comparison widget tests passed.\n";
  return 0;
}