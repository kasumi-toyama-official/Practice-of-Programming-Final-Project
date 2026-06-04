#ifndef ACHIEVEMENTPAGE_H
#define ACHIEVEMENTPAGE_H

#include <QWidget>

namespace Ui {
class AchievementPage;
}

class AchievementPage : public QWidget
{
    Q_OBJECT

public:
    explicit AchievementPage(QWidget *parent = nullptr);
    ~AchievementPage();

private:
    Ui::AchievementPage *ui;
};

#endif // ACHIEVEMENTPAGE_H
