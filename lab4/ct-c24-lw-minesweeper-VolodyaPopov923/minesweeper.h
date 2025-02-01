#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include "cellbutton.h"

#include <QAction>
#include <QCheckBox>
#include <QGridLayout>
#include <QMainWindow>
#include <QMenu>
#include <QSettings>
#include <QToolBar>
#include <QTranslator>
#include <QVector>
#include <QWidget>

class minesweeper : public QMainWindow
{
  public:
	minesweeper(QWidget *parent, bool debugMode);
	void setDebugMode(bool enabled);
	~minesweeper();
	void openLake(uint16_t x, uint16_t y);

  private slots:
	void handleButtonMiddleClicked();
	uint8_t cntFlaggedAround(uint16_t x, uint16_t y);
	void openCellsAround(uint16_t x, uint16_t y);
	void highlightCellsAround(uint16_t x, uint16_t y);
	void toggleDebugMode(int state);
	uint8_t cntMineAround(uint16_t x, uint16_t y);
	void handleButtonClicked();
	void clearBoard();
	void startNewGame();
	void startLastParamsGame();
	void createNewBoard();
	void distributeMines(uint16_t firstX, uint16_t firstY);
	void endGame(uint16_t lastX, uint16_t lastY, uint8_t winOrLos);
	void switchToRussian();
	void switchToEnglish();

  private:
	bool r_ing;
	QTranslator translator;
	QMenu *m;
	QAction *newCustomGameAction;
	QAction *newGameAction;
	QToolBar *toolBar;
	bool gameFinished = false;
	bool debugMode = false;
	QCheckBox *debugCheckBox;
	QWidget *gridWidget;
	bool firstClick = false;
	uint16_t rows;
	uint16_t cols;
	uint16_t mines;
	uint32_t cntCurrVoids;
	QVector< QVector< int > > board;
	QGridLayout *gridLayout;

  protected:
	void resizeEvent(QResizeEvent *event) override;
};
#endif
