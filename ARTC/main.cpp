#include "mainwindow.h"
#include "uitheme.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(QStringLiteral("ARTC"));
    a.setOrganizationName(QStringLiteral("ARTC"));
    a.setStyleSheet(UiTheme::styleSheet());

    MainWindow w;
    w.show();
    return a.exec();
}
