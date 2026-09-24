#include "ProcessTableWidget.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

ProcessTableWidget::ProcessTableWidget(QWidget *parent) : QWidget(parent) {
  table = new QTableWidget(this);
  table->setColumnCount(8);
  table->setHorizontalHeaderLabels(
      {"PID", "Arrival", "Burst", "Priority", "Remaining", "Waiting",
       "Turnaround", "Status"});
  table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::ExtendedSelection);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table->setAlternatingRowColors(true);
  table->verticalHeader()->setVisible(false);

  emptyStateLabel = new QLabel("Add a process to build a workload", this);
  emptyStateLabel->setObjectName("processEmptyState");
  emptyStateLabel->setAlignment(Qt::AlignCenter);

  auto *title = new QLabel("Processes", this);
  title->setObjectName("processTableTitle");
  selectionLabel = new QLabel(this);
  selectionLabel->setObjectName("processSelectionStatus");
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
  toolbar->addWidget(selectionLabel);
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
    const auto rows = selectedRows();
    if (rows.size() != 1)
      return;
    const int row = rows.front();
    emit duplicateProcessRequested(table->item(row, 2)->text().toInt(),
                     table->item(row, 3)
                       ->data(Qt::UserRole)
                       .toInt(),
                                   table->item(row, 1)->text().toInt());
  });
  connect(deleteButton, &QPushButton::clicked, this,
          &ProcessTableWidget::deleteSelectedProcesses);

  updateActionState();
}

void ProcessTableWidget::setAlgorithmName(const QString &name) {
  algorithmName = name;
  table->horizontalHeaderItem(0)->setText(QString("PID (%1)").arg(name));
}

void ProcessTableWidget::setProcesses(const std::vector<Process> &processes) {
  table->setRowCount(static_cast<int>(processes.size()));
  table->setVisible(!processes.empty());
  emptyStateLabel->setVisible(processes.empty());
  for (int i = 0; i < static_cast<int>(processes.size()); ++i) {
    const Process &p = processes[i];
    auto *pidItem = new QTableWidgetItem(QString::number(p.pid));
    pidItem->setData(Qt::UserRole, p.pid);
    table->setItem(i, 0, pidItem);
    table->setItem(i, 1, new QTableWidgetItem(QString::number(p.arrivalTime)));
    table->setItem(i, 2, new QTableWidgetItem(QString::number(p.burstTime)));
    auto *priorityItem = new QTableWidgetItem(
      algorithmName == "Priority" ? QString::number(p.priority) : "-");
    priorityItem->setData(Qt::UserRole, p.priority);
    table->setItem(i, 3, priorityItem);
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
  duplicateButton->setToolTip(
      enabled ? "Duplicate the selected process"
              : "Reset the simulation before duplicating");
  deleteButton->setToolTip(enabled ? "Delete the selected process"
                                   : "Reset the simulation before deleting");
  updateActionState();
}

std::vector<int> ProcessTableWidget::selectedRows() const {
  const auto rows = table->selectionModel()->selectedRows();
  std::vector<int> selected;
  selected.reserve(rows.size());
  for (const auto &index : rows)
    selected.push_back(index.row());
  return selected;
}

void ProcessTableWidget::updateActionState() {
  const int selectedCount = static_cast<int>(selectedRows().size());
  const bool hasSingleSelection = selectedCount == 1;
  editButton->setEnabled(hasSingleSelection && editingEnabled);
  duplicateButton->setEnabled(hasSingleSelection && editingEnabled);
  deleteButton->setEnabled(selectedCount > 0 && editingEnabled);
  deleteButton->setText(selectedCount > 1
                            ? QString("Delete %1").arg(selectedCount)
                            : "Delete");
  selectionLabel->setText(selectedCount > 0
                              ? QString("%1 selected").arg(selectedCount)
                              : QString());
  selectionLabel->setVisible(selectedCount > 0);
}

void ProcessTableWidget::editSelectedProcess() {
  const auto rows = selectedRows();
  if (rows.size() != 1 || !editingEnabled)
    return;
  const int row = rows.front();

  const int pid = table->item(row, 0)->data(Qt::UserRole).toInt();
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
  priority->setValue(table->item(row, 3)->data(Qt::UserRole).toInt());
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

void ProcessTableWidget::deleteSelectedProcesses() {
  const auto rows = selectedRows();
  if (rows.empty())
    return;

  std::vector<int> pids;
  pids.reserve(rows.size());
  for (const int row : rows)
    pids.push_back(table->item(row, 0)->data(Qt::UserRole).toInt());

  if (pids.size() > 1) {
    const auto answer = QMessageBox::question(
        this, "Delete Processes",
        QString("Delete %1 selected processes? This cannot be undone.")
            .arg(pids.size()),
        QMessageBox::Cancel | QMessageBox::Yes, QMessageBox::Cancel);
    if (answer != QMessageBox::Yes)
      return;
  }

  if (pids.size() == 1)
    emit deleteProcessRequested(pids.front());
  else
    emit deleteProcessesRequested(pids);
}
