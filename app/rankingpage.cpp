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
    loadRankings();
}

void RankingPage::setupUI()
{
    setStyleSheet("background-color: #1e1e3a; color: white;");

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setGeometry(0, 0, 960, 720);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *titleLabel = new QLabel("竞技排行榜", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #ffcc00; background: transparent;");
    mainLayout->addWidget(titleLabel);

    m_summaryLabel = new QLabel("", this);
    m_summaryLabel->setAlignment(Qt::AlignCenter);
    m_summaryLabel->setStyleSheet("color: #aaa; font-size: 13px; background: transparent;");
    mainLayout->addWidget(m_summaryLabel);

    m_rankingList = new QListWidget(this);
    m_rankingList->setStyleSheet(
        "QListWidget { background-color: #2a2a4a; color: white; border: 1px solid #555; font-size: 14px; }"
        "QListWidget::item { padding: 10px; border-bottom: 1px solid #444; }"
    );
    mainLayout->addWidget(m_rankingList, 4);

    m_backButton = new QPushButton("返回", this);
    m_backButton->setFixedSize(160, 45);
    m_backButton->setStyleSheet(
        "QPushButton { padding: 8px 16px; font-size: 16px; background-color: #3a3a6a; color: white; border: 1px solid #666; border-radius: 4px; }"
        "QPushButton:hover { background-color: #4a4a8a; }"
    );
    connect(m_backButton, &QPushButton::clicked, this, &RankingPage::onBackClicked);
    mainLayout->addWidget(m_backButton, 0, Qt::AlignCenter);

    ui->label->hide();
    ui->btn_back->hide();
}

void RankingPage::loadRankings()
{
    m_rankingList->clear();

    QVector<RankingEntry> all = m_rankingManager.getAllRecords();
    std::sort(all.begin(), all.end(),
              [](const RankingEntry& a, const RankingEntry& b) {
                  if (a.totalDamage != b.totalDamage)
                      return a.totalDamage > b.totalDamage;
                  return a.timestamp < b.timestamp;
              });

    int rank = 0;
    for (const RankingEntry& entry : all) {
        rank++;
        QDateTime dt = QDateTime::fromSecsSinceEpoch(entry.timestamp).toLocalTime();

        QString text = QString("第 %1 名    总伤害: %2    时间: %3")
                           .arg(rank)
                           .arg(entry.totalDamage)
                           .arg(dt.toString("yyyy-MM-dd hh:mm"));

        QListWidgetItem *item = new QListWidgetItem(text);
        if (rank == 1)
            item->setBackground(QBrush(QColor(80, 60, 0)));
        else if (rank <= 3)
            item->setBackground(QBrush(QColor(50, 50, 60)));
        else
            item->setBackground(QBrush(QColor(45, 45, 55)));
        item->setForeground(QBrush(Qt::white));
        m_rankingList->addItem(item);
    }

    if (m_rankingList->count() == 0) {
        QListWidgetItem *emptyItem = new QListWidgetItem("暂无竞技记录，去战斗吧！");
        emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsSelectable);
        m_rankingList->addItem(emptyItem);
    }

    m_summaryLabel->setText(QString("共 %1 条记录，排名依据：单局总伤害").arg(m_rankingList->count()));
}

void RankingPage::onBackClicked()
{
    UIManager::instance()->goBack();
}
