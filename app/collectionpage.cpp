#include "collectionpage.h"
#include "ui_collectionpage.h"
#include "uimanager.h"

CollectionPage::CollectionPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CollectionPage)
{
    ui->setupUi(this);
    connect(ui->btn_back, &QPushButton::clicked, [](){
        UIManager::instance()->goBack();
    });
}

CollectionPage::~CollectionPage()
{
    delete ui;
}
