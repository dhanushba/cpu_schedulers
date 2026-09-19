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
#include <QMessageBox>
#include <QSizePolicy>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  controller = new SimulationController(this);

  tabs = new QTabWidget(this);
  auto *simulationPage = new QWidget(tabs);
  auto *mainLayout = new QVBoxLayout(simulationPage);
  mainLayout->setContentsMargins(14, 14, 14, 14);
  mainLayout->setSpacing(12);

  controlPanel = new ControlPanelWidget(simulationPage);
  processTable = new ProcessTableWidget(simulationPage);
  ganttChart = new GanttChartWidget(simulationPage);
  metrics = new MetricsWidget(simulationPage);
  comparison = new ComparisonWidget(tabs);
  decisionTrace = new DecisionTraceWidget(tabs);

  mainLayout->addWidget(controlPanel);
  mainLayout->addWidget(ganttChart, 1);
  auto *detailsSplitter = new QSplitter(Qt::Horizontal, simulationPage);
  detailsSplitter->setObjectName("detailsSplitter");
  detailsSplitter->addWidget(processTable);
  detailsSplitter->addWidget(metrics);
  detailsSplitter->setStretchFactor(0, 3);
  detailsSplitter->setStretchFactor(1, 2);
  detailsSplitter->setCollapsible(0, false);
  detailsSplitter->setCollapsible(1, false);
  mainLayout->addWidget(detailsSplitter, 2);

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
  setMinimumSize(900, 640);

  applyStyles();
  wireSignals();
  refreshViews();
}

void MainWindow::applyStyles() {
  setStyleSheet(R"(
QMainWindow {
  background: #f5f5f7;
}
QWidget {
  font-family: "Segoe UI", "Noto Sans", sans-serif;
  font-size: 13px;
  color: #1d1d1f;
}
QGroupBox {
  border: 1px solid #d7d7da;
  border-radius: 6px;
  margin-top: 12px;
  padding: 12px;
  background: #ffffff;
}
QGroupBox::title {
  subcontrol-origin: margin;
  left: 12px;
  padding: 0 6px;
  color: #3a3a3c;
  font-weight: 600;
}
QLabel {
  color: #1d1d1f;
}
QComboBox, QSpinBox {
  background: #ffffff;
  border: 1px solid #c7c7cc;
  border-radius: 6px;
  padding: 5px 8px;
  min-height: 22px;
}
QComboBox:hover, QSpinBox:hover {
  border-color: #8e8e93;
}
QComboBox:focus, QSpinBox:focus {
  border: 1px solid #6e6e73;
}
QPushButton {
  border: 1px solid #c7c7cc;
  border-radius: 6px;
  padding: 7px 14px;
  background: #ffffff;
  color: #1d1d1f;
}
QPushButton:hover {
  background: #f2f2f4;
  border-color: #a1a1a6;
}
QPushButton:pressed {
  background: #e5e5e7;
}
QPushButton:disabled {
  background: #f2f2f4;
  border-color: #e0e0e3;
  color: #a1a1a6;
}
QPushButton#primaryAction, QPushButton#compareAllButton {
  background: #1d1d1f;
  border-color: #1d1d1f;
  color: #ffffff;
  font-weight: 600;
}
QPushButton#primaryAction:hover, QPushButton#compareAllButton:hover {
  background: #3a3a3c;
}
QPushButton#dangerAction {
  background: #ffffff;
  border-color: #c7c7cc;
  color: #c9342f;
}
QPushButton#dangerAction:hover {
  background: #fff5f5;
  border-color: #d97a76;
}
QToolButton#processActionsButton {
  border: 1px solid transparent;
  border-radius: 4px;
  background: transparent;
  color: #6e6e73;
  font-weight: 600;
}
QToolButton#processActionsButton:hover,
QToolButton#processActionsButton:pressed {
  background: #e8e8ed;
  color: #1d1d1f;
}
QToolButton#processActionsButton:focus {
  border-color: #8e8e93;
  background: #f2f2f4;
  color: #1d1d1f;
}
QMenu {
  background: #ffffff;
  border: 1px solid #d7d7da;
  padding: 4px;
}
QMenu::item {
  border-radius: 4px;
  padding: 6px 28px 6px 10px;
}
QMenu::item:selected {
  background: #eeeeF0;
}
QMenu::item:disabled {
  color: #a1a1a6;
}
QLabel#runtimeStatus {
  font-weight: 600;
  color: #6e6e73;
}
QLabel#runtimeStatus[state="running"] {
  color: #248a3d;
}
QLabel#runtimeStatus[state="paused"] {
  color: #9a6700;
}
QLabel#processEmptyState {
  color: #6e6e73;
  padding: 28px;
}
QSplitter::handle {
  background: #e5e5e7;
  width: 1px;
}
QTableWidget {
  background: #ffffff;
  alternate-background-color: #fafafa;
  border: 1px solid #d7d7da;
  border-radius: 6px;
  gridline-color: #ececef;
  selection-background-color: #e8e8ed;
  selection-color: #1d1d1f;
}
QHeaderView::section {
  background: #f5f5f7;
  color: #3a3a3c;
  padding: 7px;
  border: 0;
  border-right: 1px solid #e0e0e3;
  border-bottom: 1px solid #d7d7da;
  font-weight: 600;
}
QTabWidget::pane {
  border: 0;
}
QTabBar::tab {
  background: transparent;
  border: 0;
  border-bottom: 2px solid transparent;
  color: #6e6e73;
  padding: 10px 16px;
}
QTabBar::tab:selected {
  border-bottom-color: #1d1d1f;
  color: #1d1d1f;
  font-weight: 600;
}
QTabBar::tab:hover:!selected {
  color: #1d1d1f;
}
QLabel#comparisonTitle, QLabel#traceTitle {
  font-size: 18px;
  font-weight: 600;
  color: #1d1d1f;
}
QLabel#comparisonStatus, QLabel#traceStatus {
  color: #6e6e73;
}
GanttChartWidget {
  border: 1px solid #d7d7da;
  border-radius: 6px;
  background: #ffffff;
}
)");
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
    connect(processTable, &ProcessTableWidget::editProcessRequested, controller,
      &SimulationController::editProcessRequest);
    connect(processTable, &ProcessTableWidget::duplicateProcessRequested,
      controller, &SimulationController::addProcessRequest);

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
  processTable->setEditingEnabled(!controller->hasScheduler());
  processTable->setProcesses(controller->processes());
  ganttChart->setTimeline(controller->ganttChart(), controller->currentTime());
  decisionTrace->setSnapshots(controller->snapshots());
  metrics->setMetrics(controller->currentTime(), controller->currentRunningPid(),
                      controller->averageWaitingTime(),
                      controller->averageTurnaroundTime());
  controlPanel->setRuntimeState(controller->hasScheduler(),
                                controller->isLiveRunning(),
                                controller->isPaused());
  const QString state = controller->isLiveRunning()
                            ? "Running"
                            : (controller->isPaused() ? "Paused" : "Ready");
  statusBar()->showMessage(
      QString("%1 | %2 | %3 processes | Time %4")
          .arg(controlPanel->selectedAlgorithmName(), state)
          .arg(controller->processes().size())
          .arg(controller->currentTime()));
}