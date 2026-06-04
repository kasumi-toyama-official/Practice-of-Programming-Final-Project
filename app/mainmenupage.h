#ifndef MAINMENUPAGE_H
#define MAINMENUPAGE_H

#include <QWidget>

namespace Ui {
class MainMenuPage;
}

class MainMenuPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuPage(QWidget *parent = nullptr);
    ~MainMenuPage();

private:
    Ui::MainMenuPage *ui;
};

#endif // MAINMENUPAGE_H
