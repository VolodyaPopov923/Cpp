#include "minesweeper.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	bool debugMode = (argc > 1 && std::string(argv[1]) == "dbg");
	debugMode = true;
	minesweeper game(nullptr, debugMode);
	game.show();
	return app.exec();
}
