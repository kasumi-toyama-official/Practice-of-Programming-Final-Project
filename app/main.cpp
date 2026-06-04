#include "mainwindow.h"

#include <QApplication>
#include <QFontDatabase>
#include <QFont>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet(
        "QMessageBox { color: black; background-color: #f0f0f0; }"
        "QMessageBox QLabel { color: black; }"
        "QMessageBox QPushButton { color: black; background-color: #e0e0e0; }");

    int FontId = QFontDatabase::addApplicationFont(":/fonts/fzxs70/ark-pixel-12px-proportional-zh_cn.otf");
    if (FontId == -1)
    {
        qWarning("Failed to load pixel font!");
    }
    else
    {
        QString family = QFontDatabase::applicationFontFamilies(FontId).at(0);
        QFont pixelFont(family, 12);
        a.setFont(pixelFont);

        QFile styleFile(":/style/style.qss");
        if (styleFile.open(QFile::ReadOnly)) {
            QString styleSheet = QLatin1String(styleFile.readAll());
            a.setStyleSheet(styleSheet + a.styleSheet());
            styleFile.close();
        }
    }

    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
