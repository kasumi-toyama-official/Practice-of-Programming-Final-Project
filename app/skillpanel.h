#ifndef SKILLPANEL_H
#define SKILLPANEL_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QMenu>
#include "GameData.h"

class SkillPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SkillPanel(QWidget *parent = nullptr);

    // 设置技能列表（清空旧按钮，根据数据创建新按钮）
    void setSkills(const QList<SkillData>& skills, bool isArenaMode);

    // 显示/隐藏带动画
    void showWithAnimation();
    void hideWithAnimation();

signals:
    // 用户最终选择了某个技能和难度
    void skillSelected(int skillId, Difficulty difficulty);

private slots:
    void onSkillButtonClicked();

private:
    QGridLayout* m_layout;
    QList<QPushButton*> m_skillButtons;
    bool m_isArenaMode;  // 当前是否竞技模式
};

#endif // SKILLPANEL_H