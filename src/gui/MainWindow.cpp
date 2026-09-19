#include "MainWindow.h"

#include "ComparisonRunner.h"
#include "ComparisonWidget.h"
#include "ControlPanelWidget.h"
#include "DecisionTraceWidget.h"
#include "GanttChartWidget.h"
#include "MetricsWidget.h"
#include "ProcessTableWidget.h"
#include "SimulationController.h"

#include <QHBoxLayout>
#include <QSizePolicy>
#include <QMessageBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  controller = new SimulationController(this);

  tabs = new QTabWidget(this);
  auto *simulationPage = new QWidget(tabs);
  auto *mainLayout = new QVBoxLayout(simulationPage);
  auto *bottomLayout = new QHBoxLayout();
  mainLayout->setContentsMargins(14, 14, 14, 14);
  mainLayout->setSpacing(12);
  bottomLayout->setSpacing(12);

  controlPanel = new ControlPanelWidget(simulationPage);
  processTable = new ProcessTableWidget(simulationPage);
  ganttChart = new GanttChartWidget(simulationPage);
  metrics = new MetricsWidget(simulationPage);
  comparison = new ComparisonWidget(tabs);
  decisionTrace = new DecisionTraceWidget(tabs);

  mainLayout->addWidget(controlPanel);
  mainLayout->addWidget(ganttChart, 1);
  bottomLayout->addWidget(processTable, 3);
  bottomLayout->addWidget(metrics, 2);
  mainLayout->addLayout(bottomLayout, 2);

  controlPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  ganttChart->setMinimumHeight(180);
  processTable->setMinimumHeight(220);
  metrics->setMinimumWidth(260);

  tabs->addTab(simulationPage, "Simulation");
  tabs->addTab(comparison, "Compare Algorithms");
  tabs->addTab(decisionTrace, "Decision Trace");

  setCentralWidget(tabs);
  setWindowTitle("CPU Scheduling Simulator");
  resize(1280, 780);

  applyStyles();
  wireSignals();
  refreshViews();
}

void MainWindow::applyStyles() {
  setStyleSheet(R"(
QMainWindow {
  background: #eef3f8;
}
QWidget {
  font-family: "Segoe UI", "Noto Sans", sans-serif;
  font-size: 13px;
  color: #111827;
}
QGroupBox {
  border: 1px solid #c9d5e2;
  border-radius: 10px;
  margin-top: 12px;
  padding: 12px;
  background: #f8fbff;
}
QGroupBox::title {
  subcontrol-origin: margin;
  left: 12px;
  padding: 0 6px;
  color: #334155;
  font-weight: 600;
}
QLabel {
  color: #1f2937;
}
QComboBox, QSpinBox {
  background: #ffffff;
  border: 1px solid #c3ceda;
  border-radius: 8px;
  padding: 5px 8px;
  min-height: 22px;
}
QComboBox:hover, QSpinBox:hover {
  border-color: #95a8bb;
}
QComboBox:focus, QSpinBox:focus {
  border: 1px solid #3b82f6;
}
QPushButton {
  border: 1px solid #b7c7d8;
  border-radius: 8px;
  padding: 8px 14px;
  background: #e8f0fa;
  color: #1e293b;
  font-weight: 600;
}
QPushButton:hover {
  background: #d6e5f7;
  border-color: #8aa3bc;
}
QPushButton:pressed {
  background: #c9dcf3;
}
QPushButton:disabled {
  background: #e6ebf0;
  border-color: #d3dbe4;
  color: #8b97a5;
}
QTableWidget {
  background: #ffffff;
  border: 1px solid #c9d5e2;
  border-radius: 8px;
  gridline-color: #e5e7eb;
  selection-background-color: #dbeafe;
  selection-color: #0f172a;
}
QHeaderView::section {
  background: #dde8f4;
  color: #111827;
  padding: 7px;
  border: 0;
  border-right: 1px solid #c3ceda;
  border-bottom: 1px solid #c3ceda;
  font-weight: 600;
}
QTabWidget::pane {
  border: 0;
}
QTabBar::tab {
  background: #dde8f4;
  border: 1px solid #c3ceda;
  padding: 9px 18px;
  margin-right: 4px;
}
QTabBar::tab:selected {
  background: #ffffff;
  border-bottom-color: #ffffff;
}
QLabel#comparisonTitle, QLabel#traceTitle {
  font-size: 18px;
  font-weight: 600;
  color: #0f172a;
}
QLabel#comparisonStatus, QLabel#traceStatus {
  color: #64748b;
}
GanttChartWidget {
  border: 1px solid #c9d5e2;
  border-radius: 10px;
  background: #f8fbff;
}
")");
}

void MainWindow::wireSignals() {
  connect(controlPanel, &ControlPanelWidget::algorithmChanged, controller,
          &SimulationController::setAlgorithm);
  connect(controlPanel, &ControlPanelWidget::preemptiveChanged, controller,
          &SimulationController::setPreemptiveMode);
  connect(controlPanel, &ControlPanelWidget::quantumChanged, controller,
          &SimulationController::setQuantum);

  connect(controlPanel, &ControlPanelWidget::startLiveClicked, controller,
          &SimulationController::startLive);
  connect(controlPanel, &ControlPanelWidget::stepClicked, controller,
          &SimulationController::stepTick);
  connect(controlPanel, &ControlPanelWidget::runOfflineClicked, controller,
          &SimulationController::runOfflineInstant);
  connect(controlPanel, &ControlPanelWidget::stopClicked, controller,
          &SimulationController::stopSimulation);
  connect(controlPanel, &ControlPanelWidget::pauseResumeClicked, controller,
          &SimulationController::pauseResumeLive);
  connect(controlPanel, &ControlPanelWidget::resetRunKeepProcessesClicked,
          controller, &SimulationController::resetRunKeepProcesses);
  connect(controlPanel, &ControlPanelWidget::clearAllClicked, controller,
          &SimulationController::clearAllProcesses);

  connect(controlPanel, &ControlPanelWidget::addProcessClicked, controller,
          &SimulationController::addProcessRequest);

  connect(processTable, &ProcessTableWidget::deleteProcessRequested, controller,
          &SimulationController::deleteProcessRequest);

  connect(comparison, &ComparisonWidget::compareRequested, this, [this]() {
    const auto workload = controller->processes();
    if (workload.empty()) {
      QMessageBox::information(this, "No Processes",
                               "Add at least one process before comparing.");
      return;
    }
    comparison->setResults(
        ComparisonRunner::run(workload, controlPanel->quantum()));
  });

  connect(controller, &SimulationController::stateUpdated, this,
          &MainWindow::refreshViews);

  connect(controller, &SimulationController::modeUpdated, this,
          [this](bool liveRunning, bool paused, bool hasScheduler) {
            controlPanel->setRuntimeState(hasScheduler, liveRunning, paused);
          });
}

void MainWindow::refreshViews() {
  processTable->setProcesses(controller->processes());
  ganttChart->setTimeline(controller->ganttChart(), controller->currentTime());
  decisionTrace->setSnapshots(controller->snapshots());
  metrics->setMetrics(controller->currentTime(), controller->currentRunningPid(),
                      controller->averageWaitingTime(),
                      controller->averageTurnaroundTime());
  controlPanel->setRuntimeState(controller->hasScheduler(),
                                controller->isLiveRunning(),
                                controller->isPaused());
}
