#include "errorbookpage.h"
#include "ui_errorbookpage.h"
#include "uimanager.h"

ErrorBookPage::ErrorBookPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ErrorBookPage)
{
    ui->setupUi(this);
    connect(ui->btn_back, &QPushButton::clicked, [](){
        UIManager::instance()->goBack();
    });
}

ErrorBookPage::~ErrorBookPage()
{
    delete ui;
}
