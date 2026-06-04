#include "chapterselectpage.h"
#include "uimanager.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QFileInfo>
#include "managers/SaveManager.h"

static QString chapterNameRead(int id)
{
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 10; ++i) {
        QString path = dir.filePath(QString("data/questions/chapter%1.json").arg(id));
        if (QFileInfo::exists(path)) {
            QFile f(path);
            if (f.open(QIODevice::ReadOnly)) {
                QJsonObject root = QJsonDocument::fromJson(f.readAll()).object();
                return QString("第%1章  %2").arg(id).arg(root["chapterName"].toString());
            }
        }
        if (!dir.cdUp()) break;
    }
    return QString("第%1章").arg(id);
}

ChapterSelectPage::ChapterSelectPage(QWidget *parent)
    : QWidget(parent)
    , ui(nullptr)
{
    setStyleSheet("background-color: #1e1e3a; color: white;");

    QVBoxLayout* outer = new QVBoxLayout(this);
    outer->setContentsMargins(40, 30, 40, 30);

    QLabel* title = new QLabel("选择章节");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #ffcc00; background: transparent;");
    outer->addWidget(title);
    outer->addSpacing(20);

    QGridLayout* grid = new QGridLayout;
    grid->setSpacing(15);

    for (int i = 0; i < 8; ++i) {
        int chapterId = i + 1;

        QWidget* cell = new QWidget;
        QVBoxLayout* cellLayout = new QVBoxLayout(cell);
        cellLayout->setContentsMargins(0, 0, 0, 0);
        cellLayout->setSpacing(3);

        QPushButton* btn = new QPushButton(chapterNameRead(chapterId));
        btn->setMinimumSize(280, 100);
        btn->setStyleSheet(
            "QPushButton { background-color: #2a2a5a; color: white; border: 1px solid #555;"
            " border-radius: 8px; font-size: 16px; padding: 8px 18px; }"
            "QPushButton:hover { background-color: #3a3a7a; border-color: #888; }");
        cellLayout->addWidget(btn);

        connect(btn, &QPushButton::clicked, this, [chapterId]() {
            UIManager* mgr = UIManager::instance();
            mgr->setPendingChapterId(chapterId);

            BattlePage* bp = qobject_cast<BattlePage*>(mgr->getPage(UIManager::Battle));
            if (bp) bp->setArenaMode(false);

            SaveManager sm;
            sm.setSaveDirectory(QCoreApplication::applicationDirPath() + "/saves");
            bool hasSave = sm.hasArchive(chapterId);

            if (hasSave) {
                QMessageBox msgBox;
                msgBox.setWindowTitle("章节 " + QString::number(chapterId));
                msgBox.setText("检测到已有存档，请选择：");
                msgBox.setStyleSheet("color: black; background-color: #f0f0f0;");
                QPushButton* btnNew = msgBox.addButton("新游戏", QMessageBox::ActionRole);
                QPushButton* btnLoad = msgBox.addButton("读档继续", QMessageBox::AcceptRole);
                msgBox.addButton("取消", QMessageBox::RejectRole);
                msgBox.exec();

                if (msgBox.clickedButton() == btnLoad) {
                    mgr->setLoadExisting(true);
                    mgr->goTo(UIManager::Battle);
                } else if (msgBox.clickedButton() == btnNew) {
                    mgr->setLoadExisting(false);
                    SaveManager sm;
                    sm.setSaveDirectory(QCoreApplication::applicationDirPath() + "/saves");
                    sm.deleteChapterArchive(chapterId);
                    mgr->goTo(UIManager::Battle);
                }
            } else {
                mgr->setLoadExisting(false);
                mgr->goTo(UIManager::Battle);
            }
        });

        grid->addWidget(cell, i / 3, i % 3);
    }

    outer->addLayout(grid);
    outer->addStretch();

    QPushButton* backBtn = new QPushButton("返回主菜单");
    backBtn->setFixedSize(160, 40);
    backBtn->setStyleSheet(
        "QPushButton { background-color: #3a3a6a; color: white; border: 1px solid #777; border-radius: 4px; }"
        "QPushButton:hover { background-color: #5a5a8a; }");
    connect(backBtn, &QPushButton::clicked, []() {
        UIManager::instance()->goBack();
    });
    outer->addWidget(backBtn, 0, Qt::AlignCenter);
}

ChapterSelectPage::~ChapterSelectPage() {}
