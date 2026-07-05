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
    QString dlgStyle =
        "QDialog { background-color: #1e1e3a; color: white; }"
        "QLabel { color: white; background: transparent; }"
        "QLineEdit { color: white; background-color: #2a2a5a; border: 1px solid #555; padding: 4px; }";
    QString btnStyle =
        "QPushButton { color: white; background-color: #3a3a6a; border: 1px solid #777;"
        " border-radius: 4px; padding: 6px 16px; }"
        "QPushButton:hover { background-color: #5a5a8a; }";

    // 第一步：难度权重
    {
        QDialog dlg(parent);
        dlg.setWindowTitle("1/2 难度权重配置");
        dlg.setFixedSize(300, 210);
        dlg.setStyleSheet(dlgStyle);
        QFormLayout* form = new QFormLayout(&dlg);
        QLineEdit* easyEdit = new QLineEdit(QString::number(outConfig.easyWeight));
        QLineEdit* mediumEdit = new QLineEdit(QString::number(outConfig.mediumWeight));
        QLineEdit* hardEdit = new QLineEdit(QString::number(outConfig.hardWeight));
        easyEdit->setValidator(new QIntValidator(0, 100, &dlg));
        mediumEdit->setValidator(new QIntValidator(0, 100, &dlg));
        hardEdit->setValidator(new QIntValidator(0, 100, &dlg));
        form->addRow("简单 (0-100):", easyEdit);
        form->addRow("中等 (0-100):", mediumEdit);
        form->addRow("困难 (0-100):", hardEdit);

        QLabel* trophyHint1 = new QLabel("奖杯条件：简单题权重不超过60%");
        trophyHint1->setStyleSheet("color: #ffcc00; font-size: 11px; background: transparent;");
        form->addRow(trophyHint1);

        QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
        buttons->button(QDialogButtonBox::Ok)->setText("下一步");
        buttons->button(QDialogButtonBox::Cancel)->setText("取消");
        buttons->setStyleSheet(btnStyle);
        bool accepted = false;
        QObject::connect(buttons, &QDialogButtonBox::accepted, [&]() {
            if (!easyEdit->hasAcceptableInput() || !mediumEdit->hasAcceptableInput() || !hardEdit->hasAcceptableInput()) {
                QMessageBox::warning(&dlg, "输入错误", "权重必须是 0~100 之间的整数！"); return;
            }
            int e = easyEdit->text().toInt(), m = mediumEdit->text().toInt(), h = hardEdit->text().toInt();
            if (e < 0 || e > 100 || m < 0 || m > 100 || h < 0 || h > 100) {
                QMessageBox::warning(&dlg, "输入错误", "权重必须是 0~100 之间的整数！"); return;
            }
            if (e + m + h == 0) {
                QMessageBox::warning(&dlg, "输入错误", "难度权重不能全为 0！"); return;
            }
            outConfig.easyWeight = e; outConfig.mediumWeight = m; outConfig.hardWeight = h;
            accepted = true; dlg.accept();
        });
        QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        form->addRow(buttons);
        if (dlg.exec() != QDialog::Accepted || !accepted) return false;
    }

    // 第二步：题型权重
    {
        QDialog dlg(parent);
        dlg.setWindowTitle("2/2 题型权重配置");
        dlg.setFixedSize(300, 190);
        dlg.setStyleSheet(dlgStyle);
        QFormLayout* form = new QFormLayout(&dlg);
        QLineEdit* choiceEdit = new QLineEdit(QString::number(outConfig.choiceWeight));
        QLineEdit* completionEdit = new QLineEdit(QString::number(outConfig.codeCompletionWeight));
        choiceEdit->setValidator(new QIntValidator(0, 100, &dlg));
        completionEdit->setValidator(new QIntValidator(0, 100, &dlg));
        form->addRow("选择题 (0-100):", choiceEdit);
        form->addRow("代码补全题 (0-100):", completionEdit);

        QLabel* trophyHint2 = new QLabel("奖杯条件：选择题权重不超过80%");
        trophyHint2->setStyleSheet("color: #ffcc00; font-size: 11px; background: transparent;");
        form->addRow(trophyHint2);

        QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
        buttons->button(QDialogButtonBox::Ok)->setText("开始游戏");
        buttons->button(QDialogButtonBox::Cancel)->setText("取消");
        buttons->setStyleSheet(btnStyle);
        bool accepted = false;
        QObject::connect(buttons, &QDialogButtonBox::accepted, [&]() {
            if (!choiceEdit->hasAcceptableInput() || !completionEdit->hasAcceptableInput()) {
                QMessageBox::warning(&dlg, "输入错误", "权重必须是 0~100 之间的整数！"); return;
            }
            int c = choiceEdit->text().toInt(), p = completionEdit->text().toInt();
            if (c < 0 || c > 100 || p < 0 || p > 100) {
                QMessageBox::warning(&dlg, "输入错误", "权重必须是 0~100 之间的整数！"); return;
            }
            if (c + p == 0) {
                QMessageBox::warning(&dlg, "输入错误", "题型权重不能全为 0！"); return;
            }
            outConfig.choiceWeight = c; outConfig.codeCompletionWeight = p;
            accepted = true; dlg.accept();
        });
        QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        form->addRow(buttons);
        if (dlg.exec() != QDialog::Accepted || !accepted) return false;
    }

    return true;
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
