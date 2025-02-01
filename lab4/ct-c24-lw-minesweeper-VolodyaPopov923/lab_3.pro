QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    cellbutton.cpp \
    main.cpp \
    minesweeper.cpp \
    newgamedialog.cpp

HEADERS += \
    cellbutton.h \
    minesweeper.h \
    newgamedialog.h

TRANSLATIONS += \
    Minesweeper_ru_RU.qm \
    Minesweeper_en_US.qm
CONFIG += lrelease
CONFIG += embed_translations

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    translate.qrc
