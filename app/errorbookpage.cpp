#include "errorbookpage.h"
#include "ui_errorbookpage.h"
#include "uimanager.h"
#include "GameData.h"
#include "utils/JsonUtils.h"
#include "utils/CodeJudge.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>

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
    ui->label->setStyleSheet("font-size: 28px; font-weight: bold; color: white; background: transparent;");

    // Remove original spacers and btn_back from UI layout, keep label only
    for (int i = ui->verticalLayout->count() - 1; i >= 1; --i) {
        QLayoutItem *item = ui->verticalLayout->takeAt(i);
        if (item->widget()) item->widget()->hide();
        delete item;
    }

    // Filter bar
    QHBoxLayout *filterLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel("筛选章节：", this);
    filterLabel->setStyleSheet("color: white; background: transparent;");
    m_chapterFilter = new QComboBox(this);
    m_chapterFilter->addItem("全部章节", -1);
    m_chapterFilter->setStyleSheet("color: white; background-color: #2a2a4a; border: 1px solid #555;");
    connect(m_chapterFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ErrorBookPage::onFilterChanged);
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(m_chapterFilter);
    filterLayout->addStretch();
    ui->verticalLayout->addLayout(filterLayout);

    // Error list
    m_errorList = new QListWidget(this);
    m_errorList->setSelectionMode(QAbstractItemView::MultiSelection);
    m_errorList->setStyleSheet(
        "QListWidget { background-color: #2a2a4a; color: white; border: 1px solid #555; font-size: 14px; }"
        "QListWidget::item { padding: 8px; border-bottom: 1px solid #444; }"
        "QListWidget::item:selected { background-color: #4a4a8a; }"
    );
    connect(m_errorList, &QListWidget::itemClicked, this, &ErrorBookPage::onItemClicked);
    connect(m_errorList, &QListWidget::itemSelectionChanged, this, [this]() {
        QList<QListWidgetItem *> sel = m_errorList->selectedItems();
        if (sel.size() == 1) {
            QListWidgetItem *item = sel.first();
            m_selectedChapterId = item->data(Qt::UserRole).toInt();
            m_selectedQuestionId = item->data(Qt::UserRole + 1).toInt();
            m_redoButton->setEnabled(true);
            m_deleteButton->setEnabled(true);
        } else {
            m_redoButton->setEnabled(false);
            m_deleteButton->setEnabled(false);
        }
    });
    ui->verticalLayout->addWidget(m_errorList, 3);

    // Hint label
    QLabel *hintLabel = new QLabel("提示：点击可选中/取消，使用全选按钮可一次性选中全部", this);
    hintLabel->setStyleSheet("color: #888; font-size: 11px; background: transparent;");
    ui->verticalLayout->addWidget(hintLabel);

    // Detail label
    m_detailLabel = new QLabel("点击错题查看解析", this);
    m_detailLabel->setStyleSheet("color: #cccccc; font-size: 14px; padding: 10px; background-color: #1e1e3a; border: 1px solid #555;");
    m_detailLabel->setWordWrap(true);
    m_detailLabel->setMinimumHeight(80);
    ui->verticalLayout->addWidget(m_detailLabel, 1);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_redoButton = new QPushButton("重做本题", this);
    m_deleteButton = new QPushButton("删除记录", this);
    m_selectAllButton = new QPushButton("全选", this);
    m_batchDeleteButton = new QPushButton("批量删除", this);
    m_backButton = new QPushButton("返回", this);

    QString btnStyle = "QPushButton { padding: 8px 16px; font-size: 14px; background-color: #3a3a6a; color: white; border: 1px solid #666; }"
                       "QPushButton:hover { background-color: #4a4a8a; }"
                       "QPushButton:disabled { background-color: #2a2a4a; color: #888; }";
    m_redoButton->setStyleSheet(btnStyle);
    m_deleteButton->setStyleSheet(btnStyle);
    m_selectAllButton->setStyleSheet(btnStyle);
    m_batchDeleteButton->setStyleSheet(btnStyle);
    m_backButton->setStyleSheet(btnStyle);

    m_redoButton->setEnabled(false);
    m_deleteButton->setEnabled(false);

    connect(m_redoButton, &QPushButton::clicked, this, &ErrorBookPage::onRedoClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &ErrorBookPage::onDeleteClicked);
    connect(m_selectAllButton, &QPushButton::clicked, this, &ErrorBookPage::onSelectAllClicked);
    connect(m_batchDeleteButton, &QPushButton::clicked, this, &ErrorBookPage::onBatchDeleteClicked);
    connect(m_backButton, &QPushButton::clicked, this, &ErrorBookPage::onBackClicked);

    buttonLayout->addWidget(m_redoButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(m_selectAllButton);
    buttonLayout->addWidget(m_batchDeleteButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_backButton);
    ui->verticalLayout->addLayout(buttonLayout);
    ui->verticalLayout->addSpacing(8);
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
    m_chapterFilter->blockSignals(true);
    m_chapterFilter->clear();
    for (int chapterId = 0; chapterId <= 8; ++chapterId) {
        m_chapterFilter->addItem(QString("第 %1 章").arg(chapterId), chapterId);
    }
    m_chapterFilter->addItem("竞技模式", -1);
    m_chapterFilter->addItem("全部", -2);
    m_chapterFilter->blockSignals(false);

    int restoreIndex = m_chapterFilter->findData(currentChapter);
    if (restoreIndex >= 0) {
        m_chapterFilter->setCurrentIndex(restoreIndex);
    } else {
        m_chapterFilter->setCurrentIndex(m_chapterFilter->count() - 1);
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
    if (filterChapter == -2) {
        entries = m_wrongBookManager.getAllEntries();
    } else {
        entries = m_wrongBookManager.getEntriesByChapter(filterChapter);
    }

    for (const WrongBookEntry& entry : entries) {
        if (!m_questionBank.isChapterLoaded(entry.chapterId)) {
            if (entry.chapterId == -1) {
                for (int ch = 0; ch <= 8; ++ch) {
                    QString path = findDataFile(QString("data/questions/chapter%1_completion.json").arg(ch));
                    if (!path.isEmpty()) {
                        m_questionBank.appendChapter(-1, path);
                    }
                }
            } else {
                QString path = findDataFile(QString("data/questions/chapter%1.json").arg(entry.chapterId));
                if (!path.isEmpty()) {
                    m_questionBank.loadChapter(entry.chapterId, path);
                }
            }
        }

        auto qOpt = m_questionBank.getQuestionById(entry.chapterId, entry.questionId);
        if (!qOpt.has_value()) {
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

    QString chapterLabel = (entry.chapterId == -1) ? "竞技模式" : QString("第%1章").arg(entry.chapterId);
    QString text = QString("[%1] %2  难度：%3  错%4次  %5")
                       .arg(chapterLabel)
                       .arg(q.questionText.left(30) + (q.questionText.length() > 30 ? "..." : ""))
                       .arg(difficultyText)
                       .arg(entry.wrongCount)
                       .arg(dt.toString("yyyy-MM-dd hh:mm"));
    return text;
}

void ErrorBookPage::onItemClicked(QListWidgetItem *item)
{
    if (!item) return;

    int questionId = item->data(Qt::UserRole + 1).toInt();
    if (questionId <= 0) return;

    QString explanation = item->data(Qt::UserRole + 3).toString();
    explanation.replace("&", "&&");
    m_detailLabel->setText(QString("解析：\n%1").arg(explanation.isEmpty() ? "暂无解析" : explanation));
}

void ErrorBookPage::onDeleteClicked()
{
    if (m_selectedQuestionId <= 0) return;

    m_wrongBookManager.removeEntry(m_selectedChapterId, m_selectedQuestionId);
    m_wrongBookManager.save();
    loadErrors();
}

void ErrorBookPage::onSelectAllClicked()
{
    int selectableCount = 0;
    for (int i = 0; i < m_errorList->count(); ++i) {
        if (m_errorList->item(i)->flags() & Qt::ItemIsSelectable)
            ++selectableCount;
    }
    if (m_errorList->selectedItems().size() == selectableCount && selectableCount > 0) {
        m_errorList->clearSelection();
        m_selectAllButton->setText("全选");
    } else {
        m_errorList->selectAll();
        m_selectAllButton->setText("取消全选");
    }
}

void ErrorBookPage::onBatchDeleteClicked()
{
    QList<QListWidgetItem *> selected = m_errorList->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选中要删除的错题（可 Ctrl/Shift 多选或使用全选）。");
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("确认删除");
    msgBox.setText(QString("确定要删除选中的 %1 道错题吗？").arg(selected.size()));
    msgBox.setStyleSheet("QMessageBox { background-color: #f0f0f0; } QMessageBox QLabel { color: black; }");
    QPushButton *btnYes = msgBox.addButton("确定", QMessageBox::YesRole);
    msgBox.addButton("取消", QMessageBox::NoRole);
    msgBox.exec();
    if (msgBox.clickedButton() != btnYes) return;

    for (QListWidgetItem *item : selected) {
        int chapterId = item->data(Qt::UserRole).toInt();
        int questionId = item->data(Qt::UserRole + 1).toInt();
        if (questionId > 0) {
            m_wrongBookManager.removeEntry(chapterId, questionId);
        }
    }
    m_wrongBookManager.save();
    loadErrors();
    m_selectAllButton->setText("全选");
}

void ErrorBookPage::onRedoClicked()
{
    if (m_selectedQuestionId <= 0) return;

    auto qOpt = m_questionBank.getQuestionById(m_selectedChapterId, m_selectedQuestionId);
    if (!qOpt.has_value()) {
        QMessageBox::warning(this, "错误", "找不到该题目");
        return;
    }

    Question q = qOpt.value();
    bool isCC = (q.type == QuestionType::CodeCompletion);

    // Build CodeCompletionQuestion for CodeJudge
    CodeCompletionQuestion ccq;
    if (isCC) {
        ccq.id = q.id;
        ccq.chapterId = q.chapterId;
        ccq.difficulty = q.difficulty;
        ccq.codeTemplate = q.codeTemplate;
        ccq.referenceSolution = q.referenceSolution;
        ccq.blankCount = q.blankCount;
        for (const auto& tc : q.testCases) {
            CodeCompletionTestCase cct;
            cct.input = tc.input;
            cct.output = tc.output;
            ccq.testCases.append(cct);
        }
        ccq.explanation = q.explanation;
    }

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

        connect(m_redoQuestionWidget, &QuestionWidget::answerSubmitted, this, [this]() {
            QString answer = m_redoQuestionWidget->getAnswer();
            bool correct = false;
            if (m_redoQuestionType == QuestionType::Choice && m_redoCorrectIndex >= 0
                && m_redoCorrectIndex < m_redoOptions.size()) {
                correct = (answer.trimmed() == m_redoOptions[m_redoCorrectIndex].trimmed());
            } else if (m_redoIsCodeCompletion) {
                CodeJudgeResult r = CodeJudge::compileAndRun(m_redoCCQuestion, answer.trimmed());
                correct = r.passed;
                if (!r.passed) {
                    QMessageBox msgBox(m_redoDialog);
                    msgBox.setWindowTitle("判题结果");
                    msgBox.setText(r.errorMessage.isEmpty() ? "答案错误" : r.errorMessage);
                    msgBox.setStyleSheet("QMessageBox { background-color: #1e1e3a; } QMessageBox QLabel { color: white; } QPushButton { color: black; background-color: #e0e0e0; border: 1px solid #aaa; padding: 4px 12px; }");
                    msgBox.exec();
                }
            }
            m_redoQuestionWidget->showFeedback(correct);
            if (correct) {
                m_wrongBookManager.removeEntry(m_selectedChapterId, m_selectedQuestionId);
                m_wrongBookManager.save();
                QTimer::singleShot(500, m_redoDialog, &QDialog::accept);
            }
        });
    }

    QuestionData d;
    d.type = isCC ? QuestionType::CodeCompletion : QuestionType::Choice;
    d.description = q.questionText;
    d.correctOptionIndex = q.correctOptionIndex;
    d.id = q.id;
    d.explanation = q.explanation;
    d.codeTemplate = q.codeTemplate;
    d.tolerance = 1;
    if (!q.testCases.isEmpty()) {
        d.testCaseInput = q.testCases.first().input;
        d.testCaseOutput = q.testCases.first().output;
    }
    for (const QString& opt : q.options) {
        d.options.append(opt);
    }

    m_redoQuestionType = d.type;
    m_redoCorrectIndex = d.correctOptionIndex;
    m_redoIsCodeCompletion = isCC;
    m_redoCCQuestion = ccq;
    m_redoOptions.clear();
    for (const QString& opt : q.options)
        m_redoOptions.append(opt);

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
