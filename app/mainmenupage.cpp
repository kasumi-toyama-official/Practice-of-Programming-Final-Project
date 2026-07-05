#include "mainmenupage.h"
#include "ui_mainmenupage.h"
#include "uimanager.h"
#include <QApplication>
#include "battlepage.h"

MainMenuPage::MainMenuPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainMenuPage)
{
    ui->setupUi(this);

    connect(ui->btn_learn, &QPushButton::clicked, [](){
        UIManager::instance()->goTo(UIManager::ChapterSelect);
    });
    connect(ui->btn_arena, &QPushButton::clicked, []() {
        UIManager *mgr = UIManager::instance();
        BattlePage *bp = qobject_cast<BattlePage*>(mgr->getPage(UIManager::Battle));
        if (bp) bp->setArenaMode(true);
        mgr->goTo(UIManager::Battle);
    });
    connect(ui->btn_errorbook, &QPushButton::clicked, [](){
        UIManager::instance()->goTo(UIManager::ErrorBook);
    });
    connect(ui->btn_collection, &QPushButton::clicked, [](){
        UIManager::instance()->goTo(UIManager::Collection);
    });
    connect(ui->btn_achievement, &QPushButton::clicked, [](){
        UIManager::instance()->goTo(UIManager::Achievement);
    });
    connect(ui->btn_ranking, &QPushButton::clicked, [](){
        UIManager::instance()->goTo(UIManager::Ranking);
    });
    connect(ui->btn_settings, &QPushButton::clicked, [](){
        UIManager::instance()->goTo(UIManager::Settings);
    });
    connect(ui->btn_exit, &QPushButton::clicked, [](){
        QApplication::quit();
    });
}

MainMenuPage::~MainMenuPage()
{
    delete ui;
}
