#include "rankingpage.h"
#include "ui_rankingpage.h"
#include "uimanager.h"

#include <QCoreApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

RankingPage::RankingPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RankingPage)
{
    ui->setupUi(this);
    setupUI();

    m_rankingManager.setFilePath(QCoreApplication::applicationDirPath() + "/data/progress/ranking.json");
    m_rankingManager.load();
}

RankingPage::~RankingPage()
{
    delete ui;
}

void RankingPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_rankingManager.load();
    refreshChapterFilter();
    loadRankings();
}

void RankingPage::setupUI()
{
    setStyleSheet("background-color: #1e1e3a; color: white;");

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setGeometry(0, 0, 960, 720);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 标题
    QLabel *titleLabel = new QLabel("排行榜", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #ffcc00; background: transparent;");
    mainLayout->addWidget(titleLabel);

    // 筛选与统计
    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel("筛选章节：", this);
    filterLabel->setStyleSheet("color: white;");
    m_chapterFilter = new QComboBox(this);
    m_chapterFilter->addItem("全部章节", -1);
    connect(m_chapterFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &RankingPage::onFilterChanged);

    m_summaryLabel = new QLabel("记录数：0", this);
    m_summaryLabel->setStyleSheet("color: white;");
    topLayout->addWidget(filterLabel);
    topLayout->addWidget(m_chapterFilter);
    topLayout->addStretch();
    topLayout->addWidget(m_summaryLabel);
    mainLayout->addLayout(topLayout);

    // 排行榜列表
    m_rankingList = new QListWidget(this);
    m_rankingList->setStyleSheet(
        "QListWidget { background-color: #2a2a4a; color: white; border: 1px solid #555; font-size: 14px; }"
        "QListWidget::item { padding: 10px; border-bottom: 1px solid #444; }"
        "QListWidget::item:selected { background-color: #4a4a8a; }"
    );
    mainLayout->addWidget(m_rankingList, 4);

    // 返回按钮
    m_backButton = new QPushButton("返回", this);
    m_backButton->setFixedSize(160, 45);
    m_backButton->setStyleSheet(
        "QPushButton { padding: 8px 16px; font-size: 16px; background-color: #3a3a6a; color: white; border: 1px solid #666; border-radius: 4px; }"
        "QPushButton:hover { background-color: #4a4a8a; }"
    );
    connect(m_backButton, &QPushButton::clicked, this, &RankingPage::onBackClicked);
    mainLayout->addWidget(m_backButton, 0, Qt::AlignCenter);

    // 隐藏原有 .ui 控件
    ui->label->hide();
    ui->btn_back->hide();
}

void RankingPage::refreshChapterFilter()
{
    int currentChapter = m_chapterFilter->currentData().toInt();
    m_chapterFilter->clear();
    m_chapterFilter->addItem("全部章节", -1);

    // 固定列出 1~8 章
    for (int i = 1; i <= 8; ++i) {
        m_chapterFilter->addItem(chapterNameRead(i), i);
    }

    int restoreIndex = m_chapterFilter->findData(currentChapter);
    if (restoreIndex >= 0) {
        m_chapterFilter->setCurrentIndex(restoreIndex);
    }
}

void RankingPage::loadRankings()
{
    m_rankingList->clear();

    int filterChapter = m_chapterFilter->currentData().toInt();
    QVector<RankingEntry> entries;

    if (filterChapter < 0) {
        // 全部章节：展示每章最佳记录
        QSet<int> visitedChapters;
        for (const RankingEntry& entry : m_rankingManager.getAllRecords()) {
            if (visitedChapters.contains(entry.chapterId)) continue;
            auto bestOpt = m_rankingManager.getBestRecord(entry.chapterId);
            if (bestOpt.has_value()) {
                entries.append(bestOpt.value());
                visitedChapters.insert(entry.chapterId);
            }
        }
        std::sort(entries.begin(), entries.end(),
                  [](const RankingEntry& a, const RankingEntry& b) {
                      if (a.totalDamage != b.totalDamage) return a.totalDamage > b.totalDamage;
                      return a.rounds < b.rounds;
                  });
    } else {
        // 指定章节：展示全部历史记录，按时间降序
        entries = m_rankingManager.getRecordsByChapter(filterChapter);
    }

    for (const RankingEntry& entry : entries) {
        QDateTime dt = QDateTime::fromSecsSinceEpoch(entry.timestamp).toLocalTime();
        QString chapterText = chapterNameRead(entry.chapterId);
        QString trophyText = entry.trophyEarned ? "🏆 奖杯" : "未获奖杯";
        QString resultText = entry.victory ? "胜利" : "失败";

        QString text = QString("%1\n"
                               "结果：%2  |  总伤害：%3  |  回合：%4  |  答对：%5  |  %6\n"
                               "时间：%7")
                           .arg(chapterText)
                           .arg(resultText)
                           .arg(entry.totalDamage)
                           .arg(entry.rounds)
                           .arg(entry.correctCount)
                           .arg(trophyText)
                           .arg(dt.toString("yyyy-MM-dd hh:mm"));

        QListWidgetItem *item = new QListWidgetItem(text);
        if (entry.victory) {
            item->setBackground(QBrush(QColor(40, 70, 40)));
            item->setForeground(QBrush(Qt::white));
        } else {
            item->setBackground(QBrush(QColor(60, 40, 40)));
            item->setForeground(QBrush(Qt::gray));
        }
        m_rankingList->addItem(item);
    }

    if (m_rankingList->count() == 0) {
        QListWidgetItem *emptyItem = new QListWidgetItem("暂无排行榜记录，去战斗吧！");
        emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsSelectable);
        m_rankingList->addItem(emptyItem);
    }

    m_summaryLabel->setText(QString("记录数：%1").arg(m_rankingList->count()));
}

QString RankingPage::chapterNameRead(int chapterId) const
{
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 10; ++i) {
        QString path = dir.filePath(QString("data/questions/chapter%1.json").arg(chapterId));
        if (QFileInfo::exists(path)) {
            QFile f(path);
            if (f.open(QIODevice::ReadOnly)) {
                QJsonObject root = QJsonDocument::fromJson(f.readAll()).object();
                return QString("第%1章 %2").arg(chapterId).arg(root["chapterName"].toString());
            }
        }
        if (!dir.cdUp()) break;
    }
    return QString("第%1章").arg(chapterId);
}

void RankingPage::onFilterChanged(int index)
{
    Q_UNUSED(index);
    loadRankings();
}

void RankingPage::onBackClicked()
{
    UIManager::instance()->goBack();
}
