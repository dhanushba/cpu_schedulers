#pragma once

#include <QMainWindow>

class ControlPanelWidget;
class ProcessTableWidget;
class GanttChartWidget;
class MetricsWidget;
class SimulationController;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  void applyStyles();
  void wireSignals();
  void refreshViews();

  ControlPanelWidget *controlPanel;
  ProcessTableWidget *processTable;
  GanttChartWidget *ganttChart;
  MetricsWidget *metrics;
  SimulationController *controller;
};
