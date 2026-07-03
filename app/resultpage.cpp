#include "resultpage.h"
#include "ui_resultpage.h"
#include "uimanager.h"

#include <QCoreApplication>
#include <QDateTime>

ResultPage::ResultPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ResultPage)
{
    ui->setupUi(this);
    setupUI();

    m_rankingManager.setFilePath(QCoreApplication::applicationDirPath() + "/data/progress/ranking.json");
    m_rankingManager.load();
}

ResultPage::~ResultPage()
{
    delete ui;
}

void ResultPage::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setGeometry(0, 0, 960, 720);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(20);

    m_titleLabel = new QLabel("战斗结算", this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: white;");
    mainLayout->addWidget(m_titleLabel);

    m_statsLabel = new QLabel(this);
    m_statsLabel->setAlignment(Qt::AlignCenter);
    m_statsLabel->setStyleSheet("font-size: 18px; color: white; line-height: 1.6;");
    m_statsLabel->setWordWrap(true);
    mainLayout->addWidget(m_statsLabel, 2);

    m_bestLabel = new QLabel(this);
    m_bestLabel->setAlignment(Qt::AlignCenter);
    m_bestLabel->setStyleSheet("font-size: 16px; color: #cccccc;");
    m_bestLabel->setWordWrap(true);
    mainLayout->addWidget(m_bestLabel, 1);

    m_backButton = new QPushButton("返回主菜单", this);
    m_backButton->setFixedSize(160, 45);
    m_backButton->setStyleSheet(
        "QPushButton { padding: 8px 16px; font-size: 16px; background-color: #3a3a6a; color: white; border: 1px solid #666; border-radius: 4px; }"
        "QPushButton:hover { background-color: #4a4a8a; }"
    );
    connect(m_backButton, &QPushButton::clicked, this, &ResultPage::onBackClicked);
    mainLayout->addWidget(m_backButton, 0, Qt::AlignCenter);

    // 隐藏原有 .ui 控件
    ui->label->hide();
    ui->btn_back->hide();
}

void ResultPage::setResult(bool victory, int chapterId, int totalDamage, int rounds, int correctCount, bool trophyEarned)
{
    m_victory = victory;
    m_chapterId = chapterId;
    m_totalDamage = totalDamage;
    m_rounds = rounds;
    m_correctCount = correctCount;
    m_trophyEarned = trophyEarned;
}

void ResultPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_rankingManager.load();
    refreshDisplay();
}

void ResultPage::refreshDisplay()
{
    m_titleLabel->setText(m_victory ? "胜利！" : "失败...");
    m_titleLabel->setStyleSheet(m_victory
        ? "font-size: 32px; font-weight: bold; color: #4caf50;"
        : "font-size: 32px; font-weight: bold; color: #f44336;");

    QString stats = QString("章节：第 %1 章\n"
                            "总伤害：%2\n"
                            "回合数：%3\n"
                            "答对题数：%4\n"
                            "奖杯：%5")
                        .arg(m_chapterId)
                        .arg(m_totalDamage)
                        .arg(m_rounds)
                        .arg(m_correctCount)
                        .arg(m_trophyEarned ? "获得" : "未获得");
    m_statsLabel->setText(stats);

    auto bestOpt = m_rankingManager.getBestRecord(m_chapterId);
    if (bestOpt.has_value()) {
        const RankingEntry& best = bestOpt.value();
        QDateTime dt = QDateTime::fromSecsSinceEpoch(best.timestamp).toLocalTime();
        m_bestLabel->setText(QString("本章最佳记录\n"
                                     "总伤害：%1  回合：%2  答对：%3\n"
                                     "时间：%4")
                                   .arg(best.totalDamage)
                                   .arg(best.rounds)
                                   .arg(best.correctCount)
                                   .arg(dt.toString("yyyy-MM-dd hh:mm")));
    } else {
        m_bestLabel->setText("暂无本章排行榜记录");
    }
}

void ResultPage::onBackClicked()
{
    UIManager* mgr = UIManager::instance();
    mgr->clearHistory();
    mgr->goTo(UIManager::MainMenu);
}
