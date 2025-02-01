#include "cellbutton.h"

#include "minesweeper.h"

#include <QMouseEvent>

CellButton::CellButton(QWidget *parent) : QPushButton(parent), flagged(false), enabled(false)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setMinimumSize(31, 31);
	setStyleSheet("font-size: 20px; padding: 0px;");
}

bool CellButton::isFlagged() const
{
	return flagged;
}

void CellButton::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		if (!isEnabledCustom())
		{
			if (flagged)
			{
				setText(oldText);
				setToolTip("");
			}
			else
			{
				oldText = text();
				this->setStyleSheet("color: black; font-size: 20px; padding: 0px;");
				setText("|=");
			}
			flagged = !flagged;
		}
	}
	else if (event->button() == Qt::MiddleButton && !flagged)
	{
		emit middleClicked();
	}
	else if (!flagged)
	{
		QPushButton::mouseReleaseEvent(event);
	}
}

QString CellButton::getState() const
{
	if (this->text() == "F")
	{
		return "F";
	}
	else if (this->isEnabledCustom() && this->text() == "")
	{
		return "N";
	}
	else
	{
		return "NO";
	}
}
