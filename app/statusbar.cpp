#include "statusbar.h"

StatusBar::StatusBar(QWidget *parent)
    : QWidget{parent}
{
    setStyleSheet("color: white;");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    QVBoxLayout* playerLayout = new QVBoxLayout;
    m_playerNameLabel = new QLabel("玩家");
    playerLayout->addWidget(m_playerNameLabel);

    m_playerHpBar = new QProgressBar;
    m_playerHpBar->setRange(0, 100);
    m_playerHpBar->setTextVisible(false);
    m_playerHpBar->setStyleSheet(
        "QProgressBar { border: 1px solid #555; background: #2a2a4a; }"
        "QProgressBar::chunk { background: #4caf50; }");
    playerLayout->addWidget(m_playerHpBar);

    m_playerHpText = new QLabel("HP: ?/?");
    playerLayout->addWidget(m_playerHpText);

    m_playerAtkDefLabel = new QLabel("攻:? 防:?");
    playerLayout->addWidget(m_playerAtkDefLabel);

    m_playerLifestealLabel = new QLabel("吸血:?%");
    playerLayout->addWidget(m_playerLifestealLabel);

    m_playerBuffsLabel = new QLabel("");
    m_playerBuffsLabel->setWordWrap(true);
    playerLayout->addWidget(m_playerBuffsLabel);

    mainLayout->addLayout(playerLayout);

    QVBoxLayout* centerLayout = new QVBoxLayout;
    centerLayout->addStretch();

    m_pauseBtn = new QPushButton("⏸ 暂停");
    m_pauseBtn->setFixedSize(100, 34);
    m_pauseBtn->setStyleSheet(
        "QPushButton { background-color: #3a3a6a; color: white; border: 1px solid #777; border-radius: 4px; }"
        "QPushButton:hover { background-color: #5a5a8a; }");
    connect(m_pauseBtn, &QPushButton::clicked, this, &StatusBar::pauseClicked);

    QHBoxLayout* pauseRow = new QHBoxLayout;
    pauseRow->addStretch();
    pauseRow->addWidget(m_pauseBtn);
    pauseRow->addStretch();
    centerLayout->addLayout(pauseRow);

    m_roundLabel = new QLabel("回合: 0");
    m_roundLabel->setAlignment(Qt::AlignCenter);
    centerLayout->addWidget(m_roundLabel);

    m_passCardLabel = new QLabel("Pass卡: 0");
    m_passCardLabel->setAlignment(Qt::AlignCenter);
    m_passCardLabel->hide();
    centerLayout->addWidget(m_passCardLabel);

    m_totalDamageLabel = new QLabel("总伤害: 0");
    m_totalDamageLabel->setAlignment(Qt::AlignCenter);
    m_totalDamageLabel->hide();
    centerLayout->addWidget(m_totalDamageLabel);

    centerLayout->addStretch();
    mainLayout->addLayout(centerLayout);

    QVBoxLayout* enemyLayout = new QVBoxLayout;
    m_enemyNameLabel = new QLabel("敌人");
    m_enemyNameLabel->setAlignment(Qt::AlignRight);
    enemyLayout->addWidget(m_enemyNameLabel);

    m_enemyHpBar = new QProgressBar;
    m_enemyHpBar->setRange(0, 9999);
    m_enemyHpBar->setTextVisible(false);
    m_enemyHpBar->setStyleSheet(
        "QProgressBar { border: 1px solid #555; background: #2a2a4a; }"
        "QProgressBar::chunk { background: #e53935; }");
    enemyLayout->addWidget(m_enemyHpBar);

    m_enemyHpText = new QLabel("HP: ?/?");
    m_enemyHpText->setAlignment(Qt::AlignRight);
    enemyLayout->addWidget(m_enemyHpText);

    m_enemyAtkDefLabel = new QLabel("攻:? 防:?");
    m_enemyAtkDefLabel->setAlignment(Qt::AlignRight);
    enemyLayout->addWidget(m_enemyAtkDefLabel);

    m_enemyBuffsLabel = new QLabel("");
    m_enemyBuffsLabel->setWordWrap(true);
    m_enemyBuffsLabel->setAlignment(Qt::AlignRight);
    enemyLayout->addWidget(m_enemyBuffsLabel);

    mainLayout->addLayout(enemyLayout);
}

void StatusBar::updatePlayer(const Stats &stats)
{
    m_playerHpBar->setMaximum(stats.maxHp);
    m_playerHpBar->setValue(stats.hp);
    m_playerHpText->setText(QString("HP: %1/%2").arg(stats.hp).arg(stats.maxHp));
    m_playerAtkDefLabel->setText(QString("攻:%1 防:%2").arg(stats.attack).arg(stats.defence));
    m_playerLifestealLabel->setText(QString("吸血:%1%").arg(stats.lifesteal * 100, 0, 'f', 1));
    m_playerBuffsLabel->setText(stats.buffs.join(", "));

    if (m_passCardLabel->isVisible())
        m_passCardLabel->setText(QString("Pass卡: %1").arg(stats.passCards));
    if (m_totalDamageLabel->isVisible())
        m_totalDamageLabel->setText(QString("总伤害: %1").arg(stats.totalDamage));

    m_roundLabel->setText(QString("回合: %1").arg(stats.round));
}

void StatusBar::updateEnemy(const Stats &stats)
{
    m_enemyHpBar->setMaximum(stats.maxHp);
    m_enemyHpBar->setValue(stats.hp);
    m_enemyHpText->setText(QString("HP: %1/%2").arg(stats.hp).arg(stats.maxHp));
    m_enemyAtkDefLabel->setText(QString("攻:%1 防:%2").arg(stats.attack).arg(stats.defence));
    m_enemyBuffsLabel->setText(stats.buffs.join(", "));
}

void StatusBar::setRound(int round)
{
    m_roundLabel->setText(QString("回合: %1").arg(round));
}

void StatusBar::setExtraRoundHighlight(bool on)
{
    if (on)
        setStyleSheet("background-color: #332244;");
    else
        setStyleSheet("");
}

void StatusBar::showRoundLabel(bool show)
{
    m_roundLabel->setVisible(show);
}
void StatusBar::setArenaMode(bool on)
{
    m_passCardLabel->setVisible(on);
    m_totalDamageLabel->setVisible(on);
    m_pauseBtn->setVisible(!on);
    m_roundLabel->setVisible(!on);
}