#include "achievementpage.h"
#include "ui_achievementpage.h"
#include "uimanager.h"

AchievementPage::AchievementPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AchievementPage)
{
    ui->setupUi(this);
    connect(ui->btn_back, &QPushButton::clicked, [](){
        UIManager::instance()->goBack();
    });
}

AchievementPage::~AchievementPage()
{
    delete ui;
}
