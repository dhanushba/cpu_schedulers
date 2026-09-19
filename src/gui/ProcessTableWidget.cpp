#include "ProcessTableWidget.h"

#include <QAction>
#include <QDialog>
#include <QDialogButtonBox>
#include <QEvent>
#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QSpinBox>
#include <QTableWidget>
#include <QToolButton>
#include <QVBoxLayout>

ProcessTableWidget::ProcessTableWidget(QWidget *parent) : QWidget(parent) {
  table = new QTableWidget(this);
  table->setColumnCount(9);
  table->setHorizontalHeaderLabels({"PID", "Arrival", "Burst", "Priority",
                                    "Remaining", "Waiting", "Turnaround",
                                    "Status", "Actions"});
  table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  table->horizontalHeader()->setSectionResizeMode(8,
                                                   QHeaderView::ResizeToContents);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::SingleSelection);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table->setAlternatingRowColors(true);
  table->setMouseTracking(true);
  table->installEventFilter(this);
  table->verticalHeader()->setVisible(false);

  emptyStateLabel = new QLabel("Add a process to build a workload", this);
  emptyStateLabel->setObjectName("processEmptyState");
  emptyStateLabel->setAlignment(Qt::AlignCenter);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->addWidget(table);
  layout->addWidget(emptyStateLabel);

  connect(table, &QTableWidget::cellEntered, this,
          [this](int row, int) { showActionsForRow(row); });
    connect(table, &QTableWidget::currentCellChanged, this,
      [this](int row, int, int, int) { showActionsForRow(row); });
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

    auto *actionsButton = new QToolButton(table);
    actionsButton->setObjectName("processActionsButton");
    actionsButton->setText("...");
    actionsButton->setToolTip(QString("Actions for P%1").arg(p.pid));
    actionsButton->setPopupMode(QToolButton::InstantPopup);
    actionsButton->setAutoRaise(true);
    actionsButton->setFixedSize(32, 28);
    actionsButton->setVisible(false);

    auto *menu = new QMenu(actionsButton);
    auto *editAction = menu->addAction("Edit");
    editAction->setObjectName("editProcessAction");
    editAction->setEnabled(editingEnabled);
    editAction->setToolTip(editingEnabled
                               ? "Change this process before simulation starts"
                               : "Reset the simulation before editing");
    auto *duplicateAction = menu->addAction("Duplicate");
    duplicateAction->setObjectName("duplicateProcessAction");
    menu->addSeparator();
    auto *deleteAction = menu->addAction("Delete");
    deleteAction->setObjectName("deleteProcessAction");

    connect(editAction, &QAction::triggered, this, [this, p]() {
      QDialog dialog(this);
      dialog.setWindowTitle(QString("Edit P%1").arg(p.pid));
      auto *form = new QFormLayout(&dialog);

      auto *arrival = new QSpinBox(&dialog);
      arrival->setRange(0, 9999);
      arrival->setValue(p.arrivalTime);
      auto *burst = new QSpinBox(&dialog);
      burst->setRange(1, 999);
      burst->setValue(p.burstTime);
      auto *priority = new QSpinBox(&dialog);
      priority->setRange(0, 50);
      priority->setValue(p.priority);

      form->addRow("Arrival", arrival);
      form->addRow("Burst", burst);
      form->addRow("Priority", priority);
      auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save |
                                               QDialogButtonBox::Cancel,
                                           &dialog);
      form->addRow(buttons);
      connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
      connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

      if (dialog.exec() == QDialog::Accepted)
        emit editProcessRequested(p.pid, burst->value(), priority->value(),
                                  arrival->value());
    });
    connect(duplicateAction, &QAction::triggered, this, [this, p]() {
      emit duplicateProcessRequested(p.burstTime, p.priority, p.arrivalTime);
    });
    connect(deleteAction, &QAction::triggered, this,
            [this, p]() { emit deleteProcessRequested(p.pid); });

    actionsButton->setMenu(menu);
    table->setCellWidget(i, 8, actionsButton);
  }
}

void ProcessTableWidget::setEditingEnabled(bool enabled) {
  editingEnabled = enabled;
  const auto editActions = findChildren<QAction *>("editProcessAction");
  for (auto *action : editActions) {
    action->setEnabled(enabled);
    action->setToolTip(enabled ? "Change this process before simulation starts"
                               : "Reset the simulation before editing");
  }
}

bool ProcessTableWidget::eventFilter(QObject *watched, QEvent *event) {
  if (watched == table && event->type() == QEvent::Leave)
    showActionsForRow(-1);
  return QWidget::eventFilter(watched, event);
}

void ProcessTableWidget::showActionsForRow(int row) {
  for (int index = 0; index < table->rowCount(); ++index) {
    auto *button = qobject_cast<QToolButton *>(table->cellWidget(index, 8));
    if (button)
      button->setVisible(index == row);
  }
}
