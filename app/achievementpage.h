#ifndef ACHIEVEMENTPAGE_H
#define ACHIEVEMENTPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

#include "managers/AchievementManager.h"

namespace Ui {
class AchievementPage;
}

class AchievementPage : public QWidget
{
    Q_OBJECT

public:
    explicit AchievementPage(QWidget *parent = nullptr);
    ~AchievementPage();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onBackClicked();
    void onFilterChanged(int index);

private:
    Ui::AchievementPage *ui;

    AchievementManager m_achievementManager;
    QComboBox *m_filterCombo;
    QListWidget *m_achievementList;
    QLabel *m_summaryLabel;
    QPushButton *m_backButton;

    void setupUI();
    void loadAchievements();
};

#endif // ACHIEVEMENTPAGE_H
