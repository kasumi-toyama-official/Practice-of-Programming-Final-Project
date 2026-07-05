#include "achievementpage.h"
#include "ui_achievementpage.h"
#include "uimanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

AchievementPage::AchievementPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AchievementPage)
{
    ui->setupUi(this);
    setupUI();

    QString defPath = QCoreApplication::applicationDirPath() + "/data/achievements.json";
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 10; ++i) {
        QString p = dir.filePath("data/achievements.json");
        if (QFileInfo::exists(p)) { defPath = p; break; }
        if (!dir.cdUp()) break;
    }
    m_achievementManager.loadDefinitions(defPath);
    m_achievementManager.loadProgress(QCoreApplication::applicationDirPath() + "/data/progress/achievements.json");
}

AchievementPage::~AchievementPage()
{
    delete ui;
}

void AchievementPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_achievementManager.loadProgress(QCoreApplication::applicationDirPath() + "/data/progress/achievements.json");
    loadAchievements();
}

void AchievementPage::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setGeometry(0, 0, 960, 720);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 标题
    QLabel *titleLabel = new QLabel("成就墙", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    mainLayout->addWidget(titleLabel);

    // 筛选与统计
    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel("筛选：", this);
    filterLabel->setStyleSheet("color: white;");
    m_filterCombo = new QComboBox(this);
    m_filterCombo->addItem("全部", static_cast<int>(AchievementCategory::Learning));
    m_filterCombo->addItem("学习", static_cast<int>(AchievementCategory::Learning));
    m_filterCombo->addItem("战斗", static_cast<int>(AchievementCategory::Combat));
    connect(m_filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AchievementPage::onFilterChanged);

    m_summaryLabel = new QLabel("0 / 0", this);
    m_summaryLabel->setStyleSheet("color: white;");
    topLayout->addWidget(filterLabel);
    topLayout->addWidget(m_filterCombo);
    topLayout->addStretch();
    topLayout->addWidget(m_summaryLabel);
    mainLayout->addLayout(topLayout);

    // 成就列表
    m_achievementList = new QListWidget(this);
    m_achievementList->setStyleSheet(
        "QListWidget { background-color: #2a2a4a; color: white; border: 1px solid #555; font-size: 14px; }"
        "QListWidget::item { padding: 10px; border-bottom: 1px solid #444; }"
        "QListWidget::item:selected { background-color: #4a4a8a; }"
    );
    mainLayout->addWidget(m_achievementList, 4);

    // 返回按钮
    m_backButton = new QPushButton("返回", this);
    m_backButton->setStyleSheet(
        "QPushButton { padding: 8px 16px; font-size: 14px; background-color: #3a3a6a; color: white; border: 1px solid #666; }"
        "QPushButton:hover { background-color: #4a4a8a; }"
    );
    connect(m_backButton, &QPushButton::clicked, this, &AchievementPage::onBackClicked);
    mainLayout->addWidget(m_backButton, 0, Qt::AlignCenter);

    // 隐藏原有 .ui 控件
    ui->label->hide();
    ui->btn_back->hide();
}

void AchievementPage::loadAchievements()
{
    m_achievementList->clear();

    int filterIndex = m_filterCombo->currentIndex();
    QVector<Achievement> achievements;
    if (filterIndex == 1) {
        achievements = m_achievementManager.getAchievementsByCategory(AchievementCategory::Learning);
    } else if (filterIndex == 2) {
        achievements = m_achievementManager.getAchievementsByCategory(AchievementCategory::Combat);
    } else {
        achievements = m_achievementManager.getAllAchievements();
    }

    int unlockedCount = 0;
    for (const Achievement& ach : achievements) {
        if (ach.unlocked) unlockedCount++;

        QString categoryText = (ach.category == AchievementCategory::Combat) ? "战斗" : "学习";
        QString statusText = ach.unlocked ? "【已解锁】" : "【未解锁】";
        QString text = QString("%1 %2\n%3\n分类：%4")
                           .arg(statusText)
                           .arg(ach.name)
                           .arg(ach.description)
                           .arg(categoryText);

        QListWidgetItem *item = new QListWidgetItem(text);
        if (ach.unlocked) {
            item->setBackground(QBrush(QColor(60, 90, 60)));
            item->setForeground(QBrush(Qt::white));
        } else {
            item->setBackground(QBrush(QColor(60, 60, 60)));
            item->setForeground(QBrush(Qt::gray));
        }
        m_achievementList->addItem(item);
    }

    m_summaryLabel->setText(QString("已解锁：%1 / %2").arg(unlockedCount).arg(achievements.size()));
}

void AchievementPage::onFilterChanged(int index)
{
    Q_UNUSED(index);
    loadAchievements();
}

void AchievementPage::onBackClicked()
{
    UIManager::instance()->goBack();
}
