#ifndef CELLBUTTON_H
#define CELLBUTTON_H

#include "minesweeper.h"

#include <QPushButton>

class CellButton : public QPushButton
{
	Q_OBJECT

  public:
	QString oldText;
	bool enabled = false;
	bool flagged = false;
	explicit CellButton(QWidget *parent = nullptr);
	bool isFlagged() const;
	bool isEnabledCustom() const { return enabled; }
	QString getState() const;

  protected:
	void mouseReleaseEvent(QMouseEvent *event) override;

  signals:
	void middleClicked();
};

#endif
