#include "resultpage.h"
#include "ui_resultpage.h"
#include "uimanager.h"

ResultPage::ResultPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ResultPage)
{
    ui->setupUi(this);
    connect(ui->btn_back, &QPushButton::clicked, [](){
        UIManager* mgr = UIManager::instance();
        mgr->clearHistory();
        mgr->goTo(UIManager::MainMenu);
    });
}

ResultPage::~ResultPage()
{
    delete ui;
}
