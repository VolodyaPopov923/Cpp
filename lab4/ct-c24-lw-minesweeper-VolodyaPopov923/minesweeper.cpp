#include "minesweeper.h"

#include "newgamedialog.h"

minesweeper::minesweeper(QWidget *parent, bool debugMode) : QMainWindow(parent), firstClick(true), gridLayout(nullptr)
{
	menuBar()->clear();
	this->statusBar()->show();
	newGameAction = new QAction(tr("New Game"), this);
	newCustomGameAction = new QAction(tr("New Custom Game"), this);
	m = menuBar()->addMenu(tr("Menu"));
	m->addAction(newGameAction);
	m->addAction(newCustomGameAction);

	toolBar = addToolBar(tr("Main Toolbar"));
	toolBar->setObjectName("Main Toolbar");

	QAction *settingsAction = m->menuAction();
	toolBar->addAction(settingsAction);

	toolBar->addAction(newGameAction);
	toolBar->addAction(newCustomGameAction);
	QAction *englishAction = new QAction("English", this);
	QAction *russianAction = new QAction("Русский", this);

	toolBar->addAction(englishAction);
	toolBar->addAction(russianAction);

	connect(englishAction, &QAction::triggered, this, &minesweeper::switchToEnglish);
	connect(russianAction, &QAction::triggered, this, &minesweeper::switchToRussian);
	if (debugMode)
	{
		debugCheckBox = new QCheckBox(tr("Debug Mode"), this);
		debugCheckBox->setChecked(false);
		m->addAction(newCustomGameAction);
		connect(debugCheckBox, &QCheckBox::stateChanged, this, &minesweeper::toggleDebugMode);
		toolBar->addWidget(debugCheckBox);
	}

	connect(newGameAction, &QAction::triggered, this, &minesweeper::startLastParamsGame);
	connect(newCustomGameAction, &QAction::triggered, this, &minesweeper::startNewGame);
	gridWidget = new QWidget(this);
	gridWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	this->setCentralWidget(gridWidget);

	QSettings settings("./settings.ini", QSettings::IniFormat);
	rows = settings.value("rows", 10).toInt();
	cols = settings.value("cols", 10).toInt();
	mines = settings.value("mines", 10).toInt();
	firstClick = settings.value("firstClick", true).toBool();
	cntCurrVoids = settings.value("cntCurrVoids", rows * cols - mines).toInt();
	r_ing = settings.value("lang", true).toBool();
	board = QVector< QVector< int > >(rows, QVector< int >(cols, 0));
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			board[i][j] = settings.value(QString("board_%1_%2").arg(i).arg(j), 0).toInt();
		}
	}
	gameFinished = settings.value("gameFinished").toBool();
	if (!gameFinished)
	{
		gridLayout = new QGridLayout(this);
		gridLayout->setSpacing(0);
		for (uint16_t i = 0; i < rows; ++i)
		{
			for (uint16_t j = 0; j < cols; ++j)
			{
				QVariant value = settings.value(QString("button_%1_%2").arg(i).arg(j));
				QString buttonState = value.toString();
				CellButton *button = new CellButton(this);
				QString buttonText = settings.value(QString("buttonText_%1_%2").arg(i).arg(j)).toString();
				button->setStyleSheet("color: black; font-size: 20px; padding: 0px;");
				if (buttonState == "F")
				{
					button->setText(buttonText);
					button->flagged = true;
				}
				else if (buttonState == "N")
				{
					button->setText(buttonText);
					button->setEnabled(false);
				}
				else if (buttonState == "NO")
				{
					button->setText(buttonText);
				}

				button->setText(buttonText);
				button->setProperty("row", i);
				button->setProperty("col", j);
				gridLayout->addWidget(button, i, j);
				connect(button, &QPushButton::clicked, this, &minesweeper::handleButtonClicked);
				connect(button, &CellButton::middleClicked, this, &minesweeper::handleButtonMiddleClicked);
			}
		}
		gridWidget->setLayout(gridLayout);
		if (r_ing)
		{
			switchToEnglish();
		}
		else
		{
			switchToRussian();
		}
	}
	else
	{
		startNewGame();
	}
	gameFinished = false;
}
void minesweeper::resizeEvent(QResizeEvent *event)
{
	QSize Size = event->size();
	int32_t Side = qMin(Size.width(), Size.height());
	resize(Side, Side);
	QWidget::resizeEvent(event);
}

void minesweeper::switchToEnglish()
{
	translator.load(":/Minesweeper_ru_US.qm");
	qApp->installTranslator(&translator);
	newGameAction->setText(tr("New Game"));
	newCustomGameAction->setText(tr("New Custom Game"));
	m->setTitle(tr("Menu"));
	r_ing = true;
}

void minesweeper::switchToRussian()
{
	translator.load(":/Minesweeper_en_US.qm");
	qApp->installTranslator(&translator);
	newGameAction->setText(tr("Новая Игра"));
	newCustomGameAction->setText(tr("Новая Кастомная Игра"));
	m->setTitle(tr("Меню"));
	r_ing = false;
}

void minesweeper::createNewBoard()
{
	gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(0);
	for (uint16_t i = 0; i < rows; ++i)
	{
		for (uint16_t j = 0; j < cols; ++j)
		{
			CellButton *button = new CellButton(this);
			button->setProperty("row", i);
			button->setProperty("col", j);
			gridLayout->addWidget(button, i, j);
			connect(button, &QPushButton::clicked, this, &minesweeper::handleButtonClicked);
			connect(button, &CellButton::middleClicked, this, &minesweeper::handleButtonMiddleClicked);
		}
	}
	gridWidget->setLayout(gridLayout);
}
void minesweeper::startNewGame()
{
	int result;
	do
	{
		clearBoard();
		board.clear();
		firstClick = true;
		NewGameDialog dialog(this);
		result = dialog.exec();
		if (result == QDialog::Accepted)
		{
			rows = dialog.rows();
			cols = dialog.cols();
			mines = dialog.mines();
			cntCurrVoids = rows * cols - mines;
			createNewBoard();

			QSettings settings("YourCompany", "YourApp");
			settings.setValue("lastRows", rows);
			settings.setValue("lastCols", cols);
			settings.setValue("lastMines", mines);
		}
	} while (result != QDialog::Accepted);
}
void minesweeper::setDebugMode(bool enabled)
{
	debugMode = true;
}
minesweeper::~minesweeper()
{
	if (gameFinished)
		return;
	QSettings settings("./settings.ini", QSettings::IniFormat);
	settings.setValue("rows", rows);
	settings.setValue("cols", cols);
	settings.setValue("mines", mines);
	settings.setValue("firstClick", firstClick);
	settings.setValue("cntCurrVoids", cntCurrVoids);
	settings.setValue("lang", r_ing);

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			settings.setValue(QString("board_%1_%2").arg(i).arg(j), board[i][j]);
		}
	}
	settings.setValue("gameFinished", gameFinished);

	for (uint16_t i = 0; i < rows; ++i)
	{
		for (uint16_t j = 0; j < cols; ++j)
		{
			CellButton *button = qobject_cast< CellButton * >(gridLayout->itemAtPosition(i, j)->widget());
			if (button)
			{
				settings.setValue(QString("button_%1_%2").arg(i).arg(j), button->getState());
				settings.setValue(QString("buttonText_%1_%2").arg(i).arg(j), button->text());
			}
		}
	}
}

void minesweeper::handleButtonMiddleClicked()
{
	CellButton *button = qobject_cast< CellButton * >(sender());
	if (button)
	{
		int x = button->property("row").toInt();
		int y = button->property("col").toInt();
		if (cntFlaggedAround(x, y) == cntMineAround(x, y))
		{
			openCellsAround(x, y);
		}
		else
		{
			highlightCellsAround(x, y);
		}
	}
}

uint8_t minesweeper::cntFlaggedAround(uint16_t x, uint16_t y)
{
	uint8_t cnt = 0;
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++)
		{
			int nx = x + dx;
			int ny = y + dy;
			if (nx >= 0 && nx < rows && ny >= 0 && ny < cols)
			{
				CellButton *button = qobject_cast< CellButton * >(gridLayout->itemAtPosition(nx, ny)->widget());
				if (button && button->isFlagged())
				{
					cnt++;
				}
			}
		}
	}
	return cnt;
}

void minesweeper::openCellsAround(uint16_t x, uint16_t y)
{
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++)
		{
			int nx = x + dx;
			int ny = y + dy;
			if (nx >= 0 && nx < rows && ny >= 0 && ny < cols)
			{
				CellButton *button = qobject_cast< CellButton * >(gridLayout->itemAtPosition(nx, ny)->widget());
				if (button && button->isEnabled() && !button->isFlagged())
				{
					if (board[nx][ny] == -1)
					{
						button->setText("M");
						button->setStyleSheet("color: black; background-color: red; "
											  "font-size: 20px; padding: 0px;");
						button->setEnabled(false);
						endGame(nx, ny, 0);
					}
					else
					{
						openLake(nx, ny);
					}
				}
			}
		}
	}
}

void minesweeper::highlightCellsAround(uint16_t x, uint16_t y)
{
	QVector< CellButton * > highlightedButtons;
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++)
		{
			int nx = x + dx;
			int ny = y + dy;
			if (nx >= 0 && nx < rows && ny >= 0 && ny < cols)
			{
				CellButton *button = qobject_cast< CellButton * >(gridLayout->itemAtPosition(nx, ny)->widget());
				if (button && button->isEnabled() && button->text().isEmpty())
				{
					button->setStyleSheet("border: 2px solid lightyellow;");
					highlightedButtons.push_back(button);
				}
			}
		}
	}
	QTimer::singleShot(
		1000,
		this,
		[highlightedButtons]()
		{
			for (CellButton *button : highlightedButtons)
			{
				button->setStyleSheet("");
			}
		});
}
void minesweeper::toggleDebugMode(int state)
{
	if (board.empty())
	{
		return;
	}

	for (uint16_t i = 0; i < rows; ++i)
	{
		for (uint16_t j = 0; j < cols; ++j)
		{
			CellButton *button = qobject_cast< CellButton * >(gridLayout->itemAtPosition(i, j)->widget());
			if (button)
			{
				if (state == Qt::Checked && board[i][j] == -1)
				{
					button->oldText = button->text();
					button->setText("M");
					button->setStyleSheet("color: black; background-color: pink; "
										  "font-size: 20px; padding: 0px;");
				}
				else if (board[i][j] == -1)
				{
					if (button->text() == "M")
					{
						button->setText(button->oldText);
					}
					button->setStyleSheet("color: black; font-size: 20px; padding: 0px;");
				}
			}
		}
	}
}

uint8_t minesweeper::cntMineAround(uint16_t x, uint16_t y)
{
	uint8_t cnt = 0;
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++)
		{
			int nx = x + dx;
			int ny = y + dy;
			if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && board[nx][ny] == -1)
			{
				cnt++;
			}
		}
	}
	return cnt;
}
void minesweeper::openLake(uint16_t x, uint16_t y)
{
	if (x < 0 || x >= rows || y < 0 || y >= cols)
	{
		return;
	}

	CellButton *button = qobject_cast< CellButton * >(gridLayout->itemAtPosition(x, y)->widget());
	button->setStyleSheet("color: black; font-size: 20px; padding: 0px;");
	if (!button || button->isEnabledCustom())
	{
		return;
	}

	uint8_t value = cntMineAround(x, y);
	if (value > 0)
	{
		button->setText(QString::number(value));
		button->enabled = true;
	}
	else
	{
		button->setText("");
		button->setEnabled(false);
		button->enabled = true;
		for (int dx = -1; dx <= 1; dx++)
		{
			for (int dy = -1; dy <= 1; dy++)
			{
				if (dx != 0 || dy != 0)
				{
					openLake(x + dx, y + dy);
				}
			}
		}
	}
	cntCurrVoids -= 1;
}

void minesweeper::handleButtonClicked()
{
	QPushButton *button = qobject_cast< QPushButton * >(sender());
	button->setStyleSheet("color: black; font-size: 20px; padding: 0px;");
	if (button)
	{
		int x = button->property("row").toInt();
		int y = button->property("col").toInt();
		if (firstClick)
		{
			distributeMines(x, y);
			firstClick = false;
		}
		if (board[x][y] == -1)
		{
			button->setText("M");
			setToolTip(tr("M = MINE(BOMB)"));
			button->setStyleSheet("color: black; background-color: red; font-size: "
								  "20px; padding: 0px;");
			button->setEnabled(false);
			endGame(x, y, 0);
		}
		else
		{
			openLake(x, y);
		}
		if (cntCurrVoids == 0)
			endGame(-1, -1, 1);
	}
}

void minesweeper::clearBoard()
{
	if (gridLayout)
	{
		while (QLayoutItem *item = gridLayout->takeAt(0))
		{
			delete item->widget();
			delete item;
		}
		delete gridLayout;
		gridLayout = nullptr;
	}
}

void minesweeper::startLastParamsGame()
{
	clearBoard();
	board.clear();
	firstClick = true;

	QSettings settings("YourCompany", "YourApp");
	rows = settings.value("lastRows", 10).toInt();
	cols = settings.value("lastCols", 10).toInt();
	mines = settings.value("lastMines", 10).toInt();
	cntCurrVoids = rows * cols - mines;
	createNewBoard();
}

void minesweeper::distributeMines(uint16_t firstX, uint16_t firstY)
{
	QVector< int > positions(rows * cols);
	std::iota(positions.begin(), positions.end(), 0);

	QRandomGenerator gen = QRandomGenerator::securelySeeded();
	std::shuffle(positions.begin(), positions.end(), gen);

	board = QVector< QVector< int > >(rows, QVector< int >(cols, 0));

	for (uint16_t i = 0; i < mines; ++i)
	{
		uint16_t x = positions[i] / cols;
		uint16_t y = positions[i] % cols;

		if (x != firstX || y != firstY)
		{
			board[x][y] = -1;
		}
		else
		{
			x = positions[mines] / cols;
			y = positions[mines] % cols;
			board[x][y] = -1;
		}
	}
}

void minesweeper::endGame(uint16_t lastX, uint16_t lastY, uint8_t winOrLos)
{
	gameFinished = true;
	for (uint16_t i = 0; i < rows; ++i)
	{
		for (uint16_t j = 0; j < cols; ++j)
		{
			QPushButton *button = qobject_cast< QPushButton * >(gridLayout->itemAtPosition(i, j)->widget());
			if (button)
			{
				if (i == lastX && j == lastY)
					continue;
				if (board[i][j] == -1)
				{
					setToolTip(tr("M = MINE(BOMB)"));
					button->setText("M");
					button->setStyleSheet("color: black; background-color: "
										  "pink;font-size: 20px; padding: 0px;");
					button->setEnabled(false);
					cntCurrVoids -= 1;
				}
				else
				{
					uint8_t value = cntMineAround(i, j);
					button->setText(QString::number(value));
					button->setStyleSheet("color: black; background-color: "
										  "lightblue;font-size: 20px; padding: 0px;");
					button->setEnabled(false);
				}
			}
		}
	}
	if (winOrLos)
	{
		QMessageBox::information(this, tr("Win"), tr("You have win the game. Good game!"));
	}
	else
	{
		QMessageBox::information(this, tr("Game Over"), tr("You have lost the game. Better luck next time!"));
	}
}
