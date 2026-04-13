#pragma once

#include <QWidget>

class QComboBox;
class QCheckBox;
class QLabel;
class QPushButton;
class QSpinBox;

class ControlPanelWidget : public QWidget {
  Q_OBJECT

public:
  explicit ControlPanelWidget(QWidget *parent = nullptr);

  int selectedAlgorithmIndex() const;
  bool preemptiveEnabled() const;
  int quantum() const;
  int burst() const;
  int priority() const;
  int arrival() const;

  void setRuntimeState(bool hasScheduler, bool liveRunning, bool paused);

signals:
  void algorithmChanged(int index);
  void preemptiveChanged(bool enabled);
  void quantumChanged(int quantum);

  void startLiveClicked();
  void stepClicked();
  void runOfflineClicked();
  void stopClicked();
  void pauseResumeClicked();
  void resetRunKeepProcessesClicked();
  void clearAllClicked();
  void addProcessClicked(int burst, int priority, int arrival);

private:
  void applyFieldAvailability(bool configLocked);

  QComboBox *algorithmCombo;
  QCheckBox *preemptiveCheck;
  QSpinBox *quantumSpin;
  QSpinBox *burstSpin;
  QSpinBox *prioritySpin;
  QSpinBox *arrivalSpin;

  QLabel *algorithmLabel;
  QLabel *preemptiveLabel;
  QLabel *quantumLabel;
  QLabel *burstLabel;
  QLabel *priorityLabel;
  QLabel *arrivalLabel;

  QPushButton *startLiveButton;
  QPushButton *stepButton;
  QPushButton *offlineButton;
  QPushButton *stopButton;
  QPushButton *pauseResumeButton;
  QPushButton *resetRunKeepButton;
  QPushButton *clearAllButton;
  QPushButton *addButton;
};
