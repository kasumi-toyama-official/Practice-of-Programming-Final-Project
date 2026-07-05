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
    for (int c = 0; c < 3; ++c)
        m_layout->setColumnStretch(c, 0);

    int row = 0, col = 0;
    const int maxCols = 3;
    for (const SkillData& skill : skills) {
        QPushButton* btn = new QPushButton;
        btn->setIcon(QIcon(skill.iconPath));
        btn->setIconSize(QSize(32, 32));
        btn->setText(skill.name);
        btn->setProperty("skillId", skill.id);
        btn->setProperty("easyBonus", skill.easyBonus);
        btn->setProperty("mediumBonus", skill.mediumBonus);
        btn->setProperty("hardBonus", skill.hardBonus);
        btn->setProperty("attribute", skill.attribute);
        btn->setFixedSize(150, 70);
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        connect(btn, &QPushButton::clicked, this, &SkillPanel::onSkillButtonClicked);
        m_layout->addWidget(btn, row, col);
        m_skillButtons.append(btn);
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
    m_layout->activate();
}

void SkillPanel::onSkillButtonClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int skillId = btn->property("skillId").toInt();

    if (skillId == 105) {
        emit skillSelected(skillId, Difficulty::Easy);
        hideWithAnimation();
        return;
    }

    if (m_isArenaMode) {
        Difficulty diff = static_cast<Difficulty>(btn->property("difficulty").toInt());
        emit skillSelected(skillId, diff);
        hideWithAnimation();
    } else {
        int easyBonus = btn->property("easyBonus").toInt();
        int medBonus = btn->property("mediumBonus").toInt();
        int hardBonus = btn->property("hardBonus").toInt();
        QString attr = btn->property("attribute").toString();
        QString unit = (attr == "吸血比例") ? "%" : (attr == "Pass卡") ? "张" : "";

        QMenu menu;
        menu.addAction(QString("简单 (+%1%2)").arg(easyBonus).arg(unit));
        menu.addAction(QString("中等 (+%1%2)").arg(medBonus).arg(unit));
        menu.addAction(QString("困难 (+%1%2)").arg(hardBonus).arg(unit));
        QAction* chosen = menu.exec(btn->mapToGlobal(QPoint(0, btn->height())));
        if (!chosen) return;
        Difficulty diff;
        if (chosen->text().startsWith("简单")) diff = Difficulty::Easy;
        else if (chosen->text().startsWith("中等")) diff = Difficulty::Medium;
        else diff = Difficulty::Hard;
        emit skillSelected(skillId, diff);
        hideWithAnimation();
    }
}

void SkillPanel::showWithAnimation()
{
    if (graphicsEffect()) {
        graphicsEffect()->setEnabled(false);
        delete graphicsEffect();
    }
    hide();
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(0.0);
    setGraphicsEffect(effect);
    show();
    QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(250);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void SkillPanel::hideWithAnimation()
{
    delete graphicsEffect();
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);
    QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(250);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    connect(anim, &QPropertyAnimation::finished, this, &QWidget::hide);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}