#include "ProcessTableWidget.h"

#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

ProcessTableWidget::ProcessTableWidget(QWidget *parent) : QWidget(parent) {
  table = new QTableWidget(this);
  table->setColumnCount(8);
  table->setHorizontalHeaderLabels({"PID", "Arrival", "Burst", "Priority",
                                    "Remaining", "Waiting", "Turnaround",
                                    "Status"});
  table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::SingleSelection);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table->setAlternatingRowColors(true);
  table->verticalHeader()->setVisible(false);

  deleteSelectedButton = new QPushButton("Delete Selected", this);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->addWidget(table);
  layout->addWidget(deleteSelectedButton);

  deleteSelectedButton->setMinimumHeight(34);

  connect(deleteSelectedButton, &QPushButton::clicked, this, [this]() {
    const auto selected = table->selectionModel()->selectedRows();
    if (selected.isEmpty())
      return;

    const int row = selected.first().row();
    bool ok = false;
    const int pid = table->item(row, 0)->text().toInt(&ok);
    if (ok)
      emit deleteProcessRequested(pid);
  });
}

void ProcessTableWidget::setProcesses(const std::vector<Process> &processes) {
  table->setRowCount(static_cast<int>(processes.size()));
  for (int i = 0; i < static_cast<int>(processes.size()); ++i) {
    const Process &p = processes[i];
    table->setItem(i, 0, new QTableWidgetItem(QString::number(p.pid)));
    table->setItem(i, 1, new QTableWidgetItem(QString::number(p.arrivalTime)));
    table->setItem(i, 2, new QTableWidgetItem(QString::number(p.burstTime)));
    table->setItem(i, 3, new QTableWidgetItem(QString::number(p.priority)));
    table->setItem(i, 4,
                   new QTableWidgetItem(QString::number(p.remainingTime)));
    table->setItem(i, 5, new QTableWidgetItem(QString::number(p.waitingTime)));
    table->setItem(i, 6,
                   new QTableWidgetItem(QString::number(p.turnaroundTime)));
    table->setItem(i, 7,
                   new QTableWidgetItem(p.isFinished ? "Finished" : "Active"));
  }
}
