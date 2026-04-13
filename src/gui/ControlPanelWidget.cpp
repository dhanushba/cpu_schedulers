#include "ControlPanelWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QVBoxLayout>

ControlPanelWidget::ControlPanelWidget(QWidget *parent) : QWidget(parent) {
  algorithmCombo = new QComboBox(this);
  algorithmCombo->addItem("FCFS");
  algorithmCombo->addItem("SJF");
  algorithmCombo->addItem("Priority");
  algorithmCombo->addItem("Round Robin");

  preemptiveCheck = new QCheckBox("Preemptive Mode", this);
  preemptiveCheck->setChecked(true);

  quantumSpin = new QSpinBox(this);
  quantumSpin->setRange(1, 100);
  quantumSpin->setValue(2);

  burstSpin = new QSpinBox(this);
  burstSpin->setRange(1, 999);
  burstSpin->setValue(5);

  prioritySpin = new QSpinBox(this);
  prioritySpin->setRange(0, 50);
  prioritySpin->setValue(1);

  arrivalSpin = new QSpinBox(this);
  arrivalSpin->setRange(0, 9999);
  arrivalSpin->setValue(0);

  startLiveButton = new QPushButton("Start Live", this);
  stepButton = new QPushButton("Step / Next Tick", this);
  offlineButton = new QPushButton("Run Offline (Instant)", this);
  pauseResumeButton = new QPushButton("Pause", this);
  stopButton = new QPushButton("Stop", this);
  resetRunKeepButton = new QPushButton("Reset Run (Keep Processes)", this);
  clearAllButton = new QPushButton("Clear All Processes", this);
  addButton = new QPushButton("Add Process", this);

  startLiveButton->setMinimumHeight(34);
  stepButton->setMinimumHeight(34);
  offlineButton->setMinimumHeight(34);
  pauseResumeButton->setMinimumHeight(34);
  stopButton->setMinimumHeight(34);
  resetRunKeepButton->setMinimumHeight(34);
  clearAllButton->setMinimumHeight(34);
  addButton->setMinimumHeight(34);

  startLiveButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  pauseResumeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  stepButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  offlineButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  stopButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  resetRunKeepButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  clearAllButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *configBox = new QGroupBox("Scheduler Configuration", this);
  auto *configForm = new QFormLayout(configBox);
  algorithmLabel = new QLabel("Algorithm", configBox);
  preemptiveLabel = new QLabel("Preemption", configBox);
  quantumLabel = new QLabel("Time Quantum", configBox);
  configForm->addRow(algorithmLabel, algorithmCombo);
  configForm->addRow(preemptiveLabel, preemptiveCheck);
  configForm->addRow(quantumLabel, quantumSpin);

  auto *addBox = new QGroupBox("Process Input", this);
  auto *addForm = new QFormLayout(addBox);
  arrivalLabel = new QLabel("Arrival", addBox);
  burstLabel = new QLabel("Burst", addBox);
  priorityLabel = new QLabel("Priority", addBox);
  addForm->addRow(arrivalLabel, arrivalSpin);
  addForm->addRow(burstLabel, burstSpin);
  addForm->addRow(priorityLabel, prioritySpin);
  addForm->addRow(new QLabel("", addBox), addButton);

  auto *runButtons = new QHBoxLayout();
  runButtons->setSpacing(8);
  runButtons->addWidget(startLiveButton);
  runButtons->addWidget(pauseResumeButton);
  runButtons->addWidget(stepButton);
  runButtons->addWidget(offlineButton);
  runButtons->addWidget(stopButton);
  runButtons->addWidget(resetRunKeepButton);
  runButtons->addWidget(clearAllButton);

  auto *topRow = new QHBoxLayout();
  topRow->setSpacing(12);
  topRow->addWidget(configBox, 1);
  topRow->addWidget(addBox, 1);

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(10);
  root->addLayout(topRow);
  root->addLayout(runButtons);

  connect(algorithmCombo, qOverload<int>(&QComboBox::currentIndexChanged), this,
          [this](int index) {
            emit algorithmChanged(index);
            applyFieldAvailability(false);
          });

  connect(preemptiveCheck, &QCheckBox::toggled, this,
          &ControlPanelWidget::preemptiveChanged);
  connect(quantumSpin, qOverload<int>(&QSpinBox::valueChanged), this,
          &ControlPanelWidget::quantumChanged);

  connect(startLiveButton, &QPushButton::clicked, this,
          &ControlPanelWidget::startLiveClicked);
  connect(stepButton, &QPushButton::clicked, this,
          &ControlPanelWidget::stepClicked);
  connect(offlineButton, &QPushButton::clicked, this,
          &ControlPanelWidget::runOfflineClicked);
  connect(stopButton, &QPushButton::clicked, this,
          &ControlPanelWidget::stopClicked);
  connect(pauseResumeButton, &QPushButton::clicked, this,
          &ControlPanelWidget::pauseResumeClicked);
  connect(resetRunKeepButton, &QPushButton::clicked, this,
          &ControlPanelWidget::resetRunKeepProcessesClicked);
  connect(clearAllButton, &QPushButton::clicked, this,
          &ControlPanelWidget::clearAllClicked);

  connect(addButton, &QPushButton::clicked, this, [this]() {
    if (burstSpin->value() <= 0) {
      QMessageBox::warning(this, "Invalid Input",
                           "Burst Time must be greater than 0.");
      burstSpin->setFocus();
      return;
    }
    emit addProcessClicked(burstSpin->value(), prioritySpin->value(),
                           arrivalSpin->value());
  });

  applyFieldAvailability(false);
  emit algorithmChanged(algorithmCombo->currentIndex());
}

int ControlPanelWidget::selectedAlgorithmIndex() const {
  return algorithmCombo->currentIndex();
}

bool ControlPanelWidget::preemptiveEnabled() const {
  return preemptiveCheck->isChecked();
}

int ControlPanelWidget::quantum() const { return quantumSpin->value(); }

int ControlPanelWidget::burst() const { return burstSpin->value(); }

int ControlPanelWidget::priority() const { return prioritySpin->value(); }

int ControlPanelWidget::arrival() const { return arrivalSpin->value(); }

void ControlPanelWidget::setRuntimeState(bool hasScheduler, bool liveRunning,
                                         bool paused) {
  const bool lockedConfig = hasScheduler;

  algorithmCombo->setEnabled(!lockedConfig);
  applyFieldAvailability(lockedConfig);

  startLiveButton->setEnabled(!liveRunning);
  stepButton->setEnabled(!liveRunning);
  offlineButton->setEnabled(!liveRunning);

  pauseResumeButton->setEnabled(hasScheduler);
  pauseResumeButton->setText(paused ? "Resume" : "Pause");

  stopButton->setEnabled(hasScheduler || liveRunning || paused);
  resetRunKeepButton->setEnabled(true);
  clearAllButton->setEnabled(true);
}

void ControlPanelWidget::applyFieldAvailability(bool configLocked) {
  const int algorithm = selectedAlgorithmIndex();
  const bool usesPreemptionToggle = (algorithm == 1 || algorithm == 2);
  const bool usesQuantum = (algorithm == 3);
  const bool usesPriority = (algorithm == 2);

  preemptiveLabel->setVisible(usesPreemptionToggle);
  preemptiveCheck->setVisible(usesPreemptionToggle);
  preemptiveCheck->setEnabled(usesPreemptionToggle && !configLocked);

  quantumLabel->setVisible(usesQuantum);
  quantumSpin->setVisible(usesQuantum);
  quantumSpin->setEnabled(usesQuantum && !configLocked);

  priorityLabel->setVisible(usesPriority);
  prioritySpin->setVisible(usesPriority);
  prioritySpin->setEnabled(usesPriority);
}
