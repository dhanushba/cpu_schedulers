#include "ProcessTableWidget.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
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

  emptyStateLabel = new QLabel("Add a process to build a workload", this);
  emptyStateLabel->setObjectName("processEmptyState");
  emptyStateLabel->setAlignment(Qt::AlignCenter);

  auto *title = new QLabel("Processes", this);
  title->setObjectName("processTableTitle");
  editButton = new QPushButton("Edit", this);
  editButton->setObjectName("editProcessButton");
  duplicateButton = new QPushButton("Duplicate", this);
  duplicateButton->setObjectName("duplicateProcessButton");
  deleteButton = new QPushButton("Delete", this);
  deleteButton->setObjectName("deleteProcessButton");
  deleteButton->setProperty("destructive", true);

  auto *toolbar = new QHBoxLayout();
  toolbar->setSpacing(6);
  toolbar->addWidget(title);
  toolbar->addStretch();
  toolbar->addWidget(editButton);
  toolbar->addWidget(duplicateButton);
  toolbar->addWidget(deleteButton);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->addLayout(toolbar);
  layout->addWidget(table);
  layout->addWidget(emptyStateLabel);

  connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this,
          [this]() { updateActionState(); });
  connect(editButton, &QPushButton::clicked, this,
          &ProcessTableWidget::editSelectedProcess);
  connect(duplicateButton, &QPushButton::clicked, this, [this]() {
    const int row = selectedRow();
    if (row < 0)
      return;
    emit duplicateProcessRequested(table->item(row, 2)->text().toInt(),
                                   table->item(row, 3)->text().toInt(),
                                   table->item(row, 1)->text().toInt());
  });
  connect(deleteButton, &QPushButton::clicked, this, [this]() {
    const int row = selectedRow();
    if (row >= 0)
      emit deleteProcessRequested(table->item(row, 0)->text().toInt());
  });

  updateActionState();
}

void ProcessTableWidget::setProcesses(const std::vector<Process> &processes) {
  table->setRowCount(static_cast<int>(processes.size()));
  table->setVisible(!processes.empty());
  emptyStateLabel->setVisible(processes.empty());
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
  updateActionState();
}

void ProcessTableWidget::setEditingEnabled(bool enabled) {
  editingEnabled = enabled;
  editButton->setToolTip(enabled ? "Edit the selected process"
                                 : "Reset the simulation before editing");
  updateActionState();
}

int ProcessTableWidget::selectedRow() const {
  const auto rows = table->selectionModel()->selectedRows();
  return rows.isEmpty() ? -1 : rows.first().row();
}

void ProcessTableWidget::updateActionState() {
  const bool hasSelection = selectedRow() >= 0;
  editButton->setEnabled(hasSelection && editingEnabled);
  duplicateButton->setEnabled(hasSelection);
  deleteButton->setEnabled(hasSelection);
}

void ProcessTableWidget::editSelectedProcess() {
  const int row = selectedRow();
  if (row < 0 || !editingEnabled)
    return;

  const int pid = table->item(row, 0)->text().toInt();
  QDialog dialog(this);
  dialog.setWindowTitle(QString("Edit P%1").arg(pid));
  auto *form = new QFormLayout(&dialog);

  auto *arrival = new QSpinBox(&dialog);
  arrival->setRange(0, 9999);
  arrival->setValue(table->item(row, 1)->text().toInt());
  auto *burst = new QSpinBox(&dialog);
  burst->setRange(1, 999);
  burst->setValue(table->item(row, 2)->text().toInt());
  auto *priority = new QSpinBox(&dialog);
  priority->setRange(0, 50);
  priority->setValue(table->item(row, 3)->text().toInt());
  priority->setToolTip("Lower values run first");

  form->addRow("Arrival", arrival);
  form->addRow("Burst", burst);
  form->addRow("Priority (lower first)", priority);
  auto *buttons = new QDialogButtonBox(
      QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
  form->addRow(buttons);
  connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  if (dialog.exec() == QDialog::Accepted)
    emit editProcessRequested(pid, burst->value(), priority->value(),
                              arrival->value());
}
