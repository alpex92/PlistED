#include <QtGui/QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator1;
    QTranslator qtTranslator2;

    if (!qtTranslator1.load("qt_" + QLocale::system().name(),
        QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    qtTranslator1.load("qt_ru.qm");


    if (!qtTranslator2.load(":/translation_ru"))
    qtTranslator2.load("translation_ru.qm");

    a.installTranslator(&qtTranslator1);
    a.installTranslator(&qtTranslator2);

    MainWindow w;
    w.show();
    
    return a.exec();
}
