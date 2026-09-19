#pragma once

#include <QWidget>
#include <vector>

#include "ComparisonRunner.h"

class QLabel;
class QPushButton;
class QTableWidget;

class ComparisonWidget : public QWidget {
  Q_OBJECT

public:
  explicit ComparisonWidget(QWidget *parent = nullptr);

  void setResults(const std::vector<ComparisonResult> &results);

signals:
  void compareRequested();

private:
  QLabel *statusLabel;
  QPushButton *compareButton;
  QTableWidget *resultsTable;
};