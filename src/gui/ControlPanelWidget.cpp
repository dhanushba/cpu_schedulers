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
#include <QStyle>
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

        startLiveButton->setObjectName("primaryAction");
        offlineButton->setObjectName("primaryAction");
        addButton->setObjectName("primaryAction");
        clearAllButton->setObjectName("dangerAction");
        stopButton->setObjectName("dangerAction");

        startLiveButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        pauseResumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        stepButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
        offlineButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
        stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
        resetRunKeepButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
        clearAllButton->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
        addButton->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));

        startLiveButton->setToolTip("Run the simulation one time unit per second");
        pauseResumeButton->setToolTip("Pause or resume live execution");
        stepButton->setToolTip("Advance the simulation by one time unit");
        offlineButton->setToolTip("Complete the current workload immediately");
        stopButton->setToolTip("Stop live execution at the current time");
        resetRunKeepButton->setToolTip("Reset results and keep the process list");
        clearAllButton->setToolTip("Remove every process and reset the simulation");
        addButton->setToolTip("Add the configured process to the workload");

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
        runtimeStatus = new QLabel("Ready", configBox);
        runtimeStatus->setObjectName("runtimeStatus");
        configForm->addRow("Status", runtimeStatus);

  auto *addBox = new QGroupBox("Process Input", this);
  auto *addForm = new QFormLayout(addBox);
  arrivalLabel = new QLabel("Arrival", addBox);
  burstLabel = new QLabel("Burst", addBox);
  priorityLabel = new QLabel("Priority", addBox);
  addForm->addRow(arrivalLabel, arrivalSpin);
  addForm->addRow(burstLabel, burstSpin);
  addForm->addRow(priorityLabel, prioritySpin);
  addForm->addRow(new QLabel("", addBox), addButton);

        auto *executionButtons = new QHBoxLayout();
        executionButtons->setSpacing(8);
        executionButtons->addWidget(startLiveButton);
        executionButtons->addWidget(pauseResumeButton);
        executionButtons->addWidget(stepButton);
        executionButtons->addWidget(offlineButton);
        executionButtons->addWidget(stopButton);

        auto *dataButtons = new QHBoxLayout();
        dataButtons->setSpacing(8);
        dataButtons->addWidget(resetRunKeepButton);
        dataButtons->addWidget(clearAllButton);

        auto *runButtons = new QHBoxLayout();
        runButtons->setSpacing(16);
        runButtons->addLayout(executionButtons, 3);
        runButtons->addStretch();
        runButtons->addLayout(dataButtons, 2);

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

QString ControlPanelWidget::selectedAlgorithmName() const {
        return algorithmCombo->currentText();
}

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
        pauseResumeButton->setIcon(style()->standardIcon(
                        paused ? QStyle::SP_MediaPlay : QStyle::SP_MediaPause));

  stopButton->setEnabled(hasScheduler || liveRunning || paused);
  resetRunKeepButton->setEnabled(true);
  clearAllButton->setEnabled(true);

        if (liveRunning)
                runtimeStatus->setText("Running");
        else if (paused)
                runtimeStatus->setText("Paused");
        else if (hasScheduler)
                runtimeStatus->setText("Stopped");
        else
                runtimeStatus->setText("Ready");

        runtimeStatus->setProperty("state", runtimeStatus->text().toLower());
        runtimeStatus->style()->unpolish(runtimeStatus);
        runtimeStatus->style()->polish(runtimeStatus);
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
