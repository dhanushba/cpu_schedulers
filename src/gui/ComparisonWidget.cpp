#include "ComparisonWidget.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

ComparisonWidget::ComparisonWidget(QWidget *parent) : QWidget(parent) {
  auto *title = new QLabel("Algorithm Comparison", this);
  title->setObjectName("comparisonTitle");

  statusLabel = new QLabel("No comparison results", this);
  statusLabel->setObjectName("comparisonStatus");

  compareButton = new QPushButton("Compare All", this);
  compareButton->setObjectName("compareAllButton");
  compareButton->setMinimumHeight(34);

  auto *toolbar = new QHBoxLayout();
  toolbar->addWidget(title);
  toolbar->addStretch();
  toolbar->addWidget(statusLabel);
  toolbar->addWidget(compareButton);

  resultsTable = new QTableWidget(0, 5, this);
  resultsTable->setObjectName("comparisonTable");
  resultsTable->setHorizontalHeaderLabels(
      {"Algorithm", "Avg Waiting", "Avg Turnaround", "CPU Utilization",
       "Context Switches"});
  resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  resultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
  resultsTable->setAlternatingRowColors(true);
  resultsTable->verticalHeader()->setVisible(false);
  resultsTable->horizontalHeader()->setSectionResizeMode(0,
                                                         QHeaderView::Stretch);
  for (int column = 1; column < resultsTable->columnCount(); column++) {
    resultsTable->horizontalHeader()->setSectionResizeMode(
        column, QHeaderView::ResizeToContents);
  }

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(14, 14, 14, 14);
  layout->setSpacing(12);
  layout->addLayout(toolbar);
  layout->addWidget(resultsTable, 1);

  connect(compareButton, &QPushButton::clicked, this,
          &ComparisonWidget::compareRequested);
}

void ComparisonWidget::setResults(
    const std::vector<ComparisonResult> &results) {
  resultsTable->setRowCount(static_cast<int>(results.size()));

  for (int row = 0; row < static_cast<int>(results.size()); row++) {
    const auto &result = results[row];
    resultsTable->setItem(
        row, 0, new QTableWidgetItem(QString::fromStdString(result.name)));
    resultsTable->setItem(
        row, 1,
        new QTableWidgetItem(QString::number(result.averageWaitingTime, 'f', 2)));
    resultsTable->setItem(
        row, 2, new QTableWidgetItem(
                    QString::number(result.averageTurnaroundTime, 'f', 2)));
    resultsTable->setItem(
        row, 3,
        new QTableWidgetItem(
            QString("%1%").arg(result.cpuUtilization, 0, 'f', 1)));
    resultsTable->setItem(
        row, 4, new QTableWidgetItem(QString::number(result.contextSwitches)));
  }

  statusLabel->setText(
      results.empty() ? "No comparison results"
                      : QString("%1 modes compared").arg(results.size()));
}