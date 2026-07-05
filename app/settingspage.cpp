#include "settingspage.h"
#include "ui_settingspage.h"
#include "uimanager.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>

SettingsPage::SettingsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsPage)
{
    ui->setupUi(this);
    setStyleSheet("background-color: #1e1e3a;");

    ui->label->setText("游戏帮助");
    ui->label->setStyleSheet("font-size: 24px; font-weight: bold; color: #ffcc00; background: transparent;");

    // Remove spacers from UI layout
    QLayoutItem *spacer1 = ui->verticalLayout->takeAt(1);
    delete spacer1;
    QLayoutItem *spacer2 = ui->verticalLayout->takeAt(2);
    delete spacer2;

    QTextEdit* helpText = new QTextEdit;
    helpText->setReadOnly(true);
    helpText->setStyleSheet(
        "QTextEdit { color: white; background-color: #2a2a4a; border: 1px solid #555; font-size: 16px; }"
        "QScrollBar:vertical { width: 8px; background: #1a1c2c; }"
        "QScrollBar::handle:vertical { background: #7a7fba; min-height: 20px; }");
    helpText->setPlainText(
        "【学习模式】\n"
        "回合制战斗，自由选择章节。开始前需配置难度权重和题型权重。\n"
        "每回合系统抽题，答对攻击敌人并吸血回复，答错不攻击，回合结束敌人固定反击。\n"
        "每3回合触发额外回合，可选技能和难度，答对获得属性提升。\n"
        "代码补全题有3次容错机会，容错内答错不记错题本且不反击；选择题直接判对错。\n"
        "支持存档读档，通关满足阈值可获得成就。\n\n"
        "【竞技模式】\n"
        "实时无尽挑战，敌人随时间自动变强并每隔一段时间自动攻击，无法击杀。\n"
        "每轮选择技能和难度后出题，答对获得技能加成并攻击，答错容错耗尽前不挨打。\n"
        "Pass卡可跳过题目。生命归零游戏结束，记录总伤害排行。"
    );
    ui->verticalLayout->addWidget(helpText, 1);

    ui->verticalLayout->addSpacing(12);

    QPushButton* backBtn = new QPushButton("返回");
    backBtn->setFixedSize(120, 36);
    backBtn->setStyleSheet(
        "QPushButton { color: white; background-color: #3a3a6a; border: 1px solid #777; border-radius: 4px; }"
        "QPushButton:hover { background-color: #5a5a8a; }");
    connect(backBtn, &QPushButton::clicked, [](){ UIManager::instance()->goBack(); });
    ui->verticalLayout->addWidget(backBtn, 0, Qt::AlignCenter);

    ui->verticalLayout->addSpacing(16);

    ui->btn_back->hide();
}

SettingsPage::~SettingsPage()
{
    delete ui;
}
