#include "skillpanel.h"
#include <QApplication>
#include <QClipboard> // 没用上，但保留
#include <QDebug>

SkillPanel::SkillPanel(QWidget *parent)
    : QWidget(parent), m_isArenaMode(false)
{
    setStyleSheet(
        "color: white;"
        "QPushButton { color: white; background-color: #3a3a6a; border: 1px solid #777; border-radius: 4px; }"
        "QPushButton:hover { background-color: #5a5a8a; }");

    m_layout = new QGridLayout(this);
    setLayout(m_layout);
    hide(); // 初始隐藏
}

void SkillPanel::setSkills(const QList<SkillData>& skills, bool isArenaMode)
{
    m_isArenaMode = isArenaMode;
    // 清除旧按钮
    QLayoutItem* item;
    while ((item = m_layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    m_skillButtons.clear();

    int row = 0, col = 0;
    const int maxCols = 3;
    for (const SkillData& skill : skills) {
        QPushButton* btn = new QPushButton;
        btn->setIcon(QIcon(skill.iconPath));
        btn->setIconSize(QSize(32, 32));
        if (isArenaMode) {
            btn->setText(skill.name);
            btn->setProperty("skillId", skill.id);
            btn->setProperty("difficulty", static_cast<int>(skill.difficulty));
        } else {
            // 学习模式：显示属性名和可选的难度范围
            btn->setText(QString("%1\n(简单/中等/困难)").arg(skill.name));
            btn->setProperty("skillId", skill.id);
            // 存储各难度加成数值，用于弹出菜单后发送
            btn->setProperty("easyBonus", skill.easyBonus);
            btn->setProperty("mediumBonus", skill.mediumBonus);
            btn->setProperty("hardBonus", skill.hardBonus);
        }
        btn->setMinimumSize(100, 80);
        connect(btn, &QPushButton::clicked, this, &SkillPanel::onSkillButtonClicked);
        m_layout->addWidget(btn, row, col);
        m_skillButtons.append(btn);
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

void SkillPanel::onSkillButtonClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int skillId = btn->property("skillId").toInt();
    if (m_isArenaMode) {
        // 竞技模式：直接取出难度，发送信号
        Difficulty diff = static_cast<Difficulty>(btn->property("difficulty").toInt());
        emit skillSelected(skillId, diff);
        hideWithAnimation();
    } else {
        // 学习模式：弹出难度菜单
        QMenu menu;
        QAction* easyAct = menu.addAction("简单");
        QAction* mediumAct = menu.addAction("中等");
        QAction* hardAct = menu.addAction("困难");
        // 菜单弹出位置在按钮下方
        QAction* chosen = menu.exec(btn->mapToGlobal(QPoint(0, btn->height())));
        if (chosen == easyAct)
            emit skillSelected(skillId, Difficulty::Easy);
        else if (chosen == mediumAct)
            emit skillSelected(skillId, Difficulty::Medium);
        else if (chosen == hardAct)
            emit skillSelected(skillId, Difficulty::Hard);
        else
            return; // 取消
        hideWithAnimation();
    }
}

// 动画部分保持不变
void SkillPanel::showWithAnimation()
{
    setWindowOpacity(0.0);
    show();
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);
    QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(250);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void SkillPanel::hideWithAnimation()
{
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);
    QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(250);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    connect(anim, &QPropertyAnimation::finished, this, &QWidget::hide);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}