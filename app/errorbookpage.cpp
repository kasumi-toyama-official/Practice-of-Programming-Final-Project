#include "errorbookpage.h"
#include "ui_errorbookpage.h"
#include "uimanager.h"
#include "GameData.h"
#include "utils/JsonUtils.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>

ErrorBookPage::ErrorBookPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ErrorBookPage)
{
    ui->setupUi(this);
    setupUI();

    m_wrongBookManager.setFilePath(QCoreApplication::applicationDirPath() + "/data/progress/wrongbook.json");
    m_wrongBookManager.load();
}

ErrorBookPage::~ErrorBookPage()
{
    delete ui;
}

void ErrorBookPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_wrongBookManager.load();
    refreshChapterFilter();
    loadErrors();
}

void ErrorBookPage::setupUI()
{
    // 使用代码布局覆盖原有 .ui 的简单结构
    // 标题保留原有 label
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setGeometry(0, 0, 960, 720);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 标题
    QLabel *titleLabel = new QLabel("错题本", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    mainLayout->addWidget(titleLabel);

    // 筛选栏
    QHBoxLayout *filterLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel("筛选章节：", this);
    filterLabel->setStyleSheet("color: white;");
    m_chapterFilter = new QComboBox(this);
    m_chapterFilter->addItem("全部章节", -1);
    connect(m_chapterFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ErrorBookPage::onFilterChanged);
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(m_chapterFilter);
    filterLayout->addStretch();
    mainLayout->addLayout(filterLayout);

    // 错题列表
    m_errorList = new QListWidget(this);
    m_errorList->setStyleSheet(
        "QListWidget { background-color: #2a2a4a; color: white; border: 1px solid #555; font-size: 14px; }"
        "QListWidget::item { padding: 8px; border-bottom: 1px solid #444; }"
        "QListWidget::item:selected { background-color: #4a4a8a; }"
    );
    connect(m_errorList, &QListWidget::itemClicked, this, &ErrorBookPage::onItemClicked);
    mainLayout->addWidget(m_errorList, 3);

    // 详情区
    m_detailLabel = new QLabel("点击错题查看解析", this);
    m_detailLabel->setStyleSheet("color: #cccccc; font-size: 14px; padding: 10px; background-color: #1e1e3a; border: 1px solid #555;");
    m_detailLabel->setWordWrap(true);
    m_detailLabel->setMinimumHeight(80);
    mainLayout->addWidget(m_detailLabel, 1);

    // 按钮栏
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_redoButton = new QPushButton("重做本题", this);
    m_deleteButton = new QPushButton("删除记录", this);
    m_backButton = new QPushButton("返回", this);

    QString btnStyle = "QPushButton { padding: 8px 16px; font-size: 14px; background-color: #3a3a6a; color: white; border: 1px solid #666; }"
                       "QPushButton:hover { background-color: #4a4a8a; }";
    m_redoButton->setStyleSheet(btnStyle);
    m_deleteButton->setStyleSheet(btnStyle);
    m_backButton->setStyleSheet(btnStyle);

    m_redoButton->setEnabled(false);
    m_deleteButton->setEnabled(false);

    connect(m_redoButton, &QPushButton::clicked, this, &ErrorBookPage::onRedoClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &ErrorBookPage::onDeleteClicked);
    connect(m_backButton, &QPushButton::clicked, this, &ErrorBookPage::onBackClicked);

    buttonLayout->addWidget(m_redoButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_backButton);
    mainLayout->addLayout(buttonLayout);

    // 隐藏原有 .ui 中的控件，避免重复显示
    ui->label->hide();
    ui->btn_back->hide();
}

void ErrorBookPage::onBackClicked()
{
    UIManager::instance()->goBack();
}

void ErrorBookPage::onFilterChanged(int index)
{
    Q_UNUSED(index);
    loadErrors();
}

void ErrorBookPage::refreshChapterFilter()
{
    int currentChapter = m_chapterFilter->currentData().toInt();
    m_chapterFilter->clear();
    m_chapterFilter->addItem("全部章节", -1);

    QSet<int> chapters;
    for (const WrongBookEntry& entry : m_wrongBookManager.getAllEntries()) {
        chapters.insert(entry.chapterId);
    }

    QList<int> sortedChapters = chapters.values();
    std::sort(sortedChapters.begin(), sortedChapters.end());
    for (int chapterId : sortedChapters) {
        m_chapterFilter->addItem(QString("第 %1 章").arg(chapterId), chapterId);
    }

    // 恢复之前的选择
    int restoreIndex = m_chapterFilter->findData(currentChapter);
    if (restoreIndex >= 0) {
        m_chapterFilter->setCurrentIndex(restoreIndex);
    }
}

void ErrorBookPage::loadErrors()
{
    m_errorList->clear();
    m_detailLabel->setText("点击错题查看解析");
    m_selectedChapterId = -1;
    m_selectedQuestionId = -1;
    m_redoButton->setEnabled(false);
    m_deleteButton->setEnabled(false);

    int filterChapter = m_chapterFilter->currentData().toInt();
    QVector<WrongBookEntry> entries;
    if (filterChapter < 0) {
        entries = m_wrongBookManager.getAllEntries();
    } else {
        entries = m_wrongBookManager.getEntriesByChapter(filterChapter);
    }

    for (const WrongBookEntry& entry : entries) {
        // 确保对应章节题库已加载
        if (!m_questionBank.isChapterLoaded(entry.chapterId)) {
            QString path = findDataFile(QString("data/questions/chapter%1.json").arg(entry.chapterId));
            if (!path.isEmpty()) {
                m_questionBank.loadChapter(entry.chapterId, path);
            }
        }

        auto qOpt = m_questionBank.getQuestionById(entry.chapterId, entry.questionId);
        if (!qOpt.has_value()) {
            // 题库中找不到该题，跳过（可能是题目被删除）
            continue;
        }

        Question q = qOpt.value();
        QListWidgetItem *item = new QListWidgetItem(formatErrorItem(entry, q));
        item->setData(Qt::UserRole, entry.chapterId);
        item->setData(Qt::UserRole + 1, entry.questionId);
        item->setData(Qt::UserRole + 2, q.questionText);
        item->setData(Qt::UserRole + 3, q.explanation);
        m_errorList->addItem(item);
    }

    if (m_errorList->count() == 0) {
        QListWidgetItem *emptyItem = new QListWidgetItem("暂无错题，去战斗中学习吧！");
        emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsSelectable);
        m_errorList->addItem(emptyItem);
    }
}

QString ErrorBookPage::formatErrorItem(const WrongBookEntry& entry, const Question& q) const
{
    QDateTime dt = QDateTime::fromSecsSinceEpoch(entry.timestamp).toLocalTime();
    QString difficultyText;
    switch (q.difficulty) {
    case Difficulty::Easy: difficultyText = "简单"; break;
    case Difficulty::Medium: difficultyText = "中等"; break;
    case Difficulty::Hard: difficultyText = "困难"; break;
    }

    QString text = QString("[第%1章] %2  难度：%3  错%4次  %5")
                       .arg(entry.chapterId)
                       .arg(q.questionText.left(30) + (q.questionText.length() > 30 ? "..." : ""))
                       .arg(difficultyText)
                       .arg(entry.wrongCount)
                       .arg(dt.toString("yyyy-MM-dd hh:mm"));
    return text;
}

void ErrorBookPage::onItemClicked(QListWidgetItem *item)
{
    if (!item) return;

    int chapterId = item->data(Qt::UserRole).toInt();
    int questionId = item->data(Qt::UserRole + 1).toInt();
    if (chapterId <= 0 || questionId <= 0) return;

    m_selectedChapterId = chapterId;
    m_selectedQuestionId = questionId;

    QString explanation = item->data(Qt::UserRole + 3).toString();
    m_detailLabel->setText(QString("解析：\n%1").arg(explanation.isEmpty() ? "暂无解析" : explanation));

    m_redoButton->setEnabled(true);
    m_deleteButton->setEnabled(true);
}

void ErrorBookPage::onDeleteClicked()
{
    if (m_selectedChapterId <= 0 || m_selectedQuestionId <= 0) return;

    m_wrongBookManager.removeEntry(m_selectedChapterId, m_selectedQuestionId);
    m_wrongBookManager.save();
    loadErrors();
}

void ErrorBookPage::onRedoClicked()
{
    if (m_selectedChapterId <= 0 || m_selectedQuestionId <= 0) return;

    auto qOpt = m_questionBank.getQuestionById(m_selectedChapterId, m_selectedQuestionId);
    if (!qOpt.has_value()) {
        QMessageBox::warning(this, "错误", "找不到该题目");
        return;
    }

    Question q = qOpt.value();

    if (!m_redoDialog) {
        m_redoDialog = new QDialog(this);
        m_redoDialog->setWindowTitle("错题重做");
        m_redoDialog->setFixedSize(700, 520);
        QVBoxLayout *layout = new QVBoxLayout(m_redoDialog);
        m_redoQuestionWidget = new QuestionWidget(m_redoDialog);
        layout->addWidget(m_redoQuestionWidget);

        QPushButton *closeBtn = new QPushButton("关闭", m_redoDialog);
        connect(closeBtn, &QPushButton::clicked, m_redoDialog, &QDialog::accept);
        layout->addWidget(closeBtn);
    }

    QuestionData d;
    d.type = static_cast<QuestionType>(static_cast<int>(q.type));
    d.description = q.questionText;
    d.correctOptionIndex = q.correctOptionIndex;
    d.id = q.id;
    d.explanation = q.explanation;
    d.codeTemplate = q.codeTemplate;
    for (const QString& opt : q.options) {
        d.options.append(opt);
    }

    m_redoQuestionWidget->setQuestion(d);
    m_redoQuestionWidget->reset();
    m_redoDialog->exec();
}

QString ErrorBookPage::findDataFile(const QString& relativePath) const
{
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 10; ++i) {
        QString candidate = dir.filePath(relativePath);
        if (QFileInfo::exists(candidate))
            return QFileInfo(candidate).absoluteFilePath();
        if (!dir.cdUp())
            break;
    }
    if (QFileInfo::exists(relativePath))
        return QFileInfo(relativePath).absoluteFilePath();
    return QString();
}
