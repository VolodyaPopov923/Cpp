#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include "cellbutton.h"

#include <QApplication>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QSettings>
#include <QSpinBox>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>

class NewGameDialog : public QDialog
{
  public:
	NewGameDialog(QWidget *parent = nullptr);

	uint16_t rows() const;
	uint16_t cols() const;
	uint16_t mines() const;
	void setT(bool mode);

  private:
	QSpinBox *rowsSpinBox;
	QSpinBox *colsSpinBox;
	QSpinBox *minesSpinBox;
	QLabel *m;
	QLabel *n1;
	QLabel *n2;
};

#endif
