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
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QIntValidator>
#include "managers/SaveManager.h"
#include "models/GameConfig.h"

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

// 难度权重配置对话框，用户确认后 outConfig 被填充，返回 true
static bool showConfigDialog(GameConfig& outConfig, QWidget* parent)
{
    QDialog dlg(parent);
    dlg.setWindowTitle("难度权重配置");
    dlg.setFixedSize(350, 250);
    dlg.setStyleSheet(
        "QDialog { background-color: #1e1e3a; color: white; }"
        "QLabel { color: white; background: transparent; }"
        "QLineEdit { color: white; background-color: #2a2a5a; border: 1px solid #555; padding: 4px; }");

    QFormLayout* form = new QFormLayout(&dlg);

    QLineEdit* easyEdit = new QLineEdit(QString::number(outConfig.easyWeight));
    QLineEdit* mediumEdit = new QLineEdit(QString::number(outConfig.mediumWeight));
    QLineEdit* hardEdit = new QLineEdit(QString::number(outConfig.hardWeight));

    easyEdit->setValidator(new QIntValidator(0, 100, &dlg));
    mediumEdit->setValidator(new QIntValidator(0, 100, &dlg));
    hardEdit->setValidator(new QIntValidator(0, 100, &dlg));

    form->addRow("简单题权重 (0-100):", easyEdit);
    form->addRow("中等题权重 (0-100):", mediumEdit);
    form->addRow("困难题权重 (0-100):", hardEdit);

    QLabel* hint = new QLabel("三个权重不能全为 0");
    hint->setStyleSheet("color: #aaa; font-size: 11px;");
    form->addRow(hint);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    buttons->button(QDialogButtonBox::Ok)->setText("确定");
    buttons->button(QDialogButtonBox::Cancel)->setText("取消");
    buttons->setStyleSheet(
        "QPushButton { color: white; background-color: #3a3a6a; border: 1px solid #777;"
        " border-radius: 4px; padding: 6px 16px; }"
        "QPushButton:hover { background-color: #5a5a8a; }");

    QObject::connect(buttons, &QDialogButtonBox::accepted, [&]() {
        // 检查每个输入框是否合法
        if (!easyEdit->hasAcceptableInput() || !mediumEdit->hasAcceptableInput() || !hardEdit->hasAcceptableInput()) {
            QMessageBox::warning(&dlg, "输入错误", "权重必须是 0~100 之间的整数！");
            return;
        }

        int easy = easyEdit->text().toInt();
        int medium = mediumEdit->text().toInt();
        int hard = hardEdit->text().toInt();

        // 二次校验范围（QIntValidator 的 Intermediate 状态可能放过超限值）
        if (easy < 0 || easy > 100 || medium < 0 || medium > 100 || hard < 0 || hard > 100) {
            QMessageBox::warning(&dlg, "输入错误", "权重必须是 0~100 之间的整数！");
            return;
        }

        if (easy + medium + hard == 0) {
            QMessageBox::warning(&dlg, "输入错误", "三个权重不能全为 0！");
            return;
        }

        outConfig.easyWeight = easy;
        outConfig.mediumWeight = medium;
        outConfig.hardWeight = hard;
        dlg.accept();
    });

    QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    form->addRow(buttons);
    return dlg.exec() == QDialog::Accepted;
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

        connect(btn, &QPushButton::clicked, this, [this, chapterId]() {
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
                    // 读档：不弹配置，直接用存档里的配置
                    mgr->setLoadExisting(true);
                    mgr->goTo(UIManager::Battle);
                } else if (msgBox.clickedButton() == btnNew) {
                    // 新游戏：弹配置对话框
                    GameConfig config = GameConfig::getDefault();
                    if (showConfigDialog(config, this)) {
                        mgr->setPendingConfig(config);
                        mgr->setLoadExisting(false);
                        SaveManager sm2;
                        sm2.setSaveDirectory(QCoreApplication::applicationDirPath() + "/saves");
                        sm2.deleteChapterArchive(chapterId);
                        mgr->goTo(UIManager::Battle);
                    }
                }
            } else {
                // 无存档：弹配置对话框
                GameConfig config = GameConfig::getDefault();
                if (showConfigDialog(config, this)) {
                    mgr->setPendingConfig(config);
                    mgr->setLoadExisting(false);
                    mgr->goTo(UIManager::Battle);
                }
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
