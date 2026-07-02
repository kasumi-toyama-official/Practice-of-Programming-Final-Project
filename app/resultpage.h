#ifndef RESULTPAGE_H
#define RESULTPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "managers/RankingManager.h"

namespace Ui {
class ResultPage;
}

class ResultPage : public QWidget
{
    Q_OBJECT

public:
    explicit ResultPage(QWidget *parent = nullptr);
    ~ResultPage();

    void setResult(bool victory, int chapterId, int totalDamage, int rounds, int correctCount, bool trophyEarned);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onBackClicked();

private:
    Ui::ResultPage *ui;

    RankingManager m_rankingManager;

    bool m_victory = false;
    int m_chapterId = 0;
    int m_totalDamage = 0;
    int m_rounds = 0;
    int m_correctCount = 0;
    bool m_trophyEarned = false;

    QLabel *m_titleLabel;
    QLabel *m_statsLabel;
    QLabel *m_bestLabel;
    QPushButton *m_backButton;

    void setupUI();
    void refreshDisplay();
};

#endif // RESULTPAGE_H
