#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "GameData.h"

class StatusBar : public QWidget
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);

    void updatePlayer(const Stats& stats);
    void updateEnemy(const Stats& stats);
    void setRound(int round);
    void setExtraRoundHighlight(bool on);
    void showRoundLabel(bool show);
    void setArenaMode(bool on);

signals:
    void pauseClicked();

private:
    QLabel* m_playerNameLabel;
    QProgressBar* m_playerHpBar;
    QLabel* m_playerHpText;
    QLabel* m_playerAtkDefLabel;
    QLabel* m_playerLifestealLabel;
    QLabel* m_playerBuffsLabel;

    QLabel* m_roundLabel;
    QPushButton* m_pauseBtn;

    QLabel* m_enemyNameLabel;
    QProgressBar* m_enemyHpBar;
    QLabel* m_enemyHpText;
    QLabel* m_enemyAtkDefLabel;
    QLabel* m_enemyBuffsLabel;

    QLabel* m_passCardLabel;
    QLabel* m_totalDamageLabel;
};

#endif // STATUSBAR_H
