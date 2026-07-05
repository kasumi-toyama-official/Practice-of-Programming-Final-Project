#include "mainwindow.h"

#include <QApplication>
#include <QFontDatabase>
#include <QFont>
#include <QFile>
#include <QMessageBox>
#include <QDir>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

static bool isAsciiPath()
{
#ifdef Q_OS_WIN
    wchar_t buf[MAX_PATH];
    DWORD len = GetModuleFileNameW(NULL, buf, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) return false;
    for (DWORD i = 0; i < len; ++i) {
        if (buf[i] > 127) return false;
    }
    return true;
#else
    return true;
#endif
}

int main(int argc, char *argv[])
{
    if (!isAsciiPath()) {
        MessageBoxW(NULL,
            L"检测到游戏路径包含非英文字符！\n\n"
            L"由于编译器限制，请将整个游戏文件夹移动到纯英文路径下再运行。\n"
            L"例如：D:\\Game",
            L"路径错误", MB_OK | MB_ICONERROR);
        return 1;
    }

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
