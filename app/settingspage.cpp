#include "settingspage.h"
#include "ui_settingspage.h"
#include "uimanager.h"

SettingsPage::SettingsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsPage)
{
    ui->setupUi(this);
    connect(ui->btn_back, &QPushButton::clicked, [](){
        UIManager::instance()->goBack();
    });
}

SettingsPage::~SettingsPage()
{
    delete ui;
}
