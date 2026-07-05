#include "questionwidget.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QMouseEvent>

QuestionWidget::QuestionWidget(QWidget *parent)
    : QWidget{parent}, m_currentType(QuestionType::Choice), m_codeTemplate(""), m_selectedChoice(-1)
{
    setStyleSheet(
        "color: white; background-color: #2a2a4a;"
        "QPushButton { color: white; background-color: #3a3a6a; border: 1px solid #777; border-radius: 4px; padding: 6px; font-size: 15px; }"
        "QPushButton:hover { background-color: #5a5a8a; }"
        "QPushButton:checked { background-color: #2a7a2a; border: 2px solid #4caf50; }"
        "QLineEdit { color: white; background-color: #1e1e3a; border: 1px solid #555; border-radius: 3px; padding: 4px; }"
        "QPlainTextEdit { color: white; background-color: #1e1e3a; border: 1px solid #555; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget, 1);
    setLayout(mainLayout);

    setupChoicePanel();
    setupCodeCompletionPanel();
    setupCodingPanel();

    m_stackedWidget->setCurrentWidget(m_choicePanel);
}

void QuestionWidget::setupChoicePanel()
{
    m_choicePanel = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(m_choicePanel);

    // 难度标签（左上角）
    m_choiceDiffLabel = new QLabel("中等");
    m_choiceDiffLabel->setStyleSheet(
        "QLabel { color: #ffcc00; font-size: 12px; font-weight: bold;"
        " background: transparent; padding: 0; }");
    layout->addWidget(m_choiceDiffLabel);

    m_choiceToleranceLabel = new QLabel;
    m_choiceToleranceLabel->hide();
    m_choiceToleranceLabel->setStyleSheet("color: #ff9800; font-size: 12px; background: transparent;");
    layout->addWidget(m_choiceToleranceLabel);

    QTextEdit* descArea = new QTextEdit;
    descArea->setObjectName("choiceDescLabel");
    descArea->setReadOnly(true);
    descArea->setPlainText("选择题描述");
    descArea->setStyleSheet(
        "QTextEdit { border: none; background: transparent; color: white; font-size: 16px; }"
        "QScrollBar:vertical { width: 8px; background: #1a1c2c; }"
        "QScrollBar::handle:vertical { background: #7a7fba; min-height: 20px; }");
    layout->addWidget(descArea, 3);

    QWidget* buddyDummy = new QWidget;
    buddyDummy->hide();
    for (int i = 0; i < 4; ++i)
    {
        QLabel* label = new QLabel;
        label->setObjectName(QString("choiceLabel_%1").arg(i));
        label->setWordWrap(true);
        label->setMargin(6);
        label->setBuddy(buddyDummy);
        label->setStyleSheet(
            "QLabel { color: white; background-color: #3a3a6a; border: 1px solid #777;"
            " border-radius: 4px; padding: 6px; font-size: 15px; }"
            "QLabel:hover { background-color: #5a5a8a; }");
        label->installEventFilter(this);
        layout->addWidget(label);
        m_choiceLabels.append(label);
    }

    m_stackedWidget->addWidget(m_choicePanel);
}

void QuestionWidget::setupCodeCompletionPanel()
{
    m_codeCompletionPanel = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(m_codeCompletionPanel);

    m_codeCompletionDiffLabel = new QLabel("中等");
    m_codeCompletionDiffLabel->setStyleSheet(
        "QLabel { color: #ffcc00; font-size: 12px; font-weight: bold;"
        " background: transparent; padding: 0; }");
    layout->addWidget(m_codeCompletionDiffLabel);

    QTextEdit* descArea = new QTextEdit;
    descArea->setObjectName("codeCompletionDescLabel");
    descArea->setReadOnly(true);
    descArea->setStyleSheet(
        "QTextEdit { border: none; background: transparent; color: white; font-size: 16px; }"
        "QScrollBar:vertical { width: 8px; background: #1a1c2c; }"
        "QScrollBar::handle:vertical { background: #7a7fba; min-height: 20px; }");
    layout->addWidget(descArea, 2);

    m_codeCompletionToleranceLabel = new QLabel("剩余容错次数：0");
    m_codeCompletionToleranceLabel->setObjectName("codeCompletionToleranceLabel");
    layout->addWidget(m_codeCompletionToleranceLabel);

    QPushButton* viewCodeBtn = new QPushButton("查看代码片段与示例");
    connect(viewCodeBtn, &QPushButton::clicked, this, [this]() {
        QDialog* dlg = new QDialog(this);
        dlg->setWindowTitle("代码模板与样例");
        dlg->setMinimumSize(960, 640);
        dlg->setStyleSheet("QDialog { background-color: #1e1e3a; }");
        QVBoxLayout* dl = new QVBoxLayout(dlg);

        QHBoxLayout* topBar = new QHBoxLayout;
        QPushButton* copyCodeBtn = new QPushButton("一键复制代码");
        copyCodeBtn->setStyleSheet("QPushButton { color: white; background-color: #3a3a6a;"
            " border: 1px solid #777; border-radius: 4px; padding: 6px 12px; }");
        connect(copyCodeBtn, &QPushButton::clicked, this, [this, copyCodeBtn]() {
            QApplication::clipboard()->setText(m_storedCodeTemplate);
            copyCodeBtn->setText("已复制!");
        });
        topBar->addWidget(copyCodeBtn);
        QPushButton* copyInputBtn = new QPushButton("复制样例输入");
        copyInputBtn->setStyleSheet("QPushButton { color: white; background-color: #3a3a6a;"
            " border: 1px solid #777; border-radius: 4px; padding: 6px 12px; }");
        connect(copyInputBtn, &QPushButton::clicked, this, [this, copyInputBtn]() {
            QApplication::clipboard()->setText(m_storedSampleInput);
            copyInputBtn->setText("已复制!");
        });
        topBar->addWidget(copyInputBtn);
        QPushButton* copyOutputBtn = new QPushButton("复制样例输出");
        copyOutputBtn->setStyleSheet("QPushButton { color: white; background-color: #3a3a6a;"
            " border: 1px solid #777; border-radius: 4px; padding: 6px 12px; }");
        connect(copyOutputBtn, &QPushButton::clicked, this, [this, copyOutputBtn]() {
            QApplication::clipboard()->setText(m_storedSampleOutput);
            copyOutputBtn->setText("已复制!");
        });
        topBar->addWidget(copyOutputBtn);
        topBar->addStretch();
        dl->addLayout(topBar);

        QHBoxLayout* split = new QHBoxLayout;

        QVBoxLayout* codeCol = new QVBoxLayout;
        QLabel* codeHeader = new QLabel("代码模板");
        codeHeader->setStyleSheet("color: #ffcc00; font-size: 13px; font-weight: bold; background: transparent;");
        codeCol->addWidget(codeHeader);
        QPlainTextEdit* codeEdit = new QPlainTextEdit;
        codeEdit->setReadOnly(true);
        codeEdit->setPlainText(m_storedCodeTemplate);
        codeEdit->setStyleSheet("QPlainTextEdit { color: white; background-color: #2a2a4a;"
            " font-family: monospace; font-size: 13px; }");
        codeCol->addWidget(codeEdit, 1);
        split->addLayout(codeCol, 3);

        QVBoxLayout* rightCol = new QVBoxLayout;
        QLabel* inputHeader = new QLabel("样例输入");
        inputHeader->setStyleSheet("color: #ffcc00; font-size: 13px; font-weight: bold; background: transparent;");
        rightCol->addWidget(inputHeader);
        QPlainTextEdit* inputEdit = new QPlainTextEdit;
        inputEdit->setReadOnly(true);
        inputEdit->setPlainText(m_storedSampleInput);
        inputEdit->setStyleSheet("QPlainTextEdit { color: white; background-color: #2a2a4a;"
            " font-family: monospace; font-size: 13px; }");
        rightCol->addWidget(inputEdit, 1);

        QLabel* outputHeader = new QLabel("样例输出");
        outputHeader->setStyleSheet("color: #ffcc00; font-size: 13px; font-weight: bold; background: transparent;");
        rightCol->addWidget(outputHeader);
        QPlainTextEdit* outputEdit = new QPlainTextEdit;
        outputEdit->setReadOnly(true);
        outputEdit->setPlainText(m_storedSampleOutput);
        outputEdit->setStyleSheet("QPlainTextEdit { color: white; background-color: #2a2a4a;"
            " font-family: monospace; font-size: 13px; }");
        rightCol->addWidget(outputEdit, 1);
        split->addLayout(rightCol, 2);

        dl->addLayout(split, 1);

        QPushButton* closeBtn = new QPushButton("关闭");
        closeBtn->setStyleSheet("QPushButton { color: white; background-color: #3a3a6a;"
            " border: 1px solid #777; border-radius: 4px; padding: 6px 12px; }");
        connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::accept);
        dl->addWidget(closeBtn, 0, Qt::AlignCenter);
        dlg->exec();
        dlg->deleteLater();
    });
    layout->addWidget(viewCodeBtn);

    m_codeCompletionEditor = new QPlainTextEdit;
    m_codeCompletionEditor->setObjectName("codeCompletionEditor");
    m_codeCompletionEditor->setPlaceholderText("在此处输入补全的代码...");
    m_codeCompletionEditor->setStyleSheet("font-family: monospace;");
    m_codeCompletionEditor->setMinimumHeight(60);
    layout->addWidget(m_codeCompletionEditor, 1);

    QPushButton* submitBtn = new QPushButton("确认提交");
    submitBtn->setObjectName("codeCompletionSubmitBtn");
    connect(submitBtn, &QPushButton::clicked, this, &QuestionWidget::answerSubmitted);
    layout->addWidget(submitBtn);

    m_stackedWidget->addWidget(m_codeCompletionPanel);
}

void QuestionWidget::setupCodingPanel()
{
    m_codingPanel = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(m_codingPanel);

    // 难度标签（左上角）
    m_codingDiffLabel = new QLabel("中等");
    m_codingDiffLabel->setStyleSheet(
        "QLabel { color: #ffcc00; font-size: 12px; font-weight: bold;"
        " background: transparent; padding: 0; }");
    layout->addWidget(m_codingDiffLabel);

    QLabel* descLabel = new QLabel("编程题描述");
    descLabel->setObjectName("codingDescLabel");
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel, 1);

    m_codingToleranceLabel = new QLabel("剩余容错次数：0");
    m_codingToleranceLabel->setObjectName("codingToleranceLabel");
    layout->addWidget(m_codingToleranceLabel);

    m_codeEditor = new QPlainTextEdit;
    m_codeEditor->setPlaceholderText("请在此处编写代码...");
    m_codeEditor->setStyleSheet("font-family: monospace;");
    layout->addWidget(m_codeEditor, 2);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    QPushButton* copyTemplateBtn = new QPushButton("复制模板");
    connect(copyTemplateBtn, &QPushButton::clicked, this, [this](){
        if (!m_codeTemplate.isEmpty())
            QApplication::clipboard()->setText(m_codeTemplate);
    });
    btnLayout->addWidget(copyTemplateBtn);

    QPushButton* copyCodeBtn = new QPushButton("复制代码");
    connect(copyCodeBtn, &QPushButton::clicked, this, [this](){
        QApplication::clipboard()->setText(m_codeEditor->toPlainText());
    });
    btnLayout->addWidget(copyCodeBtn);

    QPushButton* pasteBtn = new QPushButton("粘贴");
    connect(pasteBtn, &QPushButton::clicked, this, [this](){
        m_codeEditor->insertPlainText(QApplication::clipboard()->text());
    });
    btnLayout->addWidget(pasteBtn);

    QPushButton* submitCodeBtn = new QPushButton("提交代码");
    connect(submitCodeBtn, &QPushButton::clicked, this, &QuestionWidget::answerSubmitted);
    btnLayout->addWidget(submitCodeBtn);

    layout->addLayout(btnLayout);
    m_stackedWidget->addWidget(m_codingPanel);
}

void QuestionWidget::setQuestion(const QuestionData &data)
{
    m_currentType = data.type;
    m_codeTemplate = data.codeTemplate;

    // 设置难度标签（三个面板都设，切到哪个都能看到）
    QString diffText;
    QString diffColor;
    switch (data.difficulty) {
    case Difficulty::Easy:
        diffText = "简单"; diffColor = "#4caf50"; break;   // 绿色
    case Difficulty::Medium:
        diffText = "中等"; diffColor = "#ffcc00"; break;   // 黄色
    case Difficulty::Hard:
        diffText = "困难"; diffColor = "#f44336"; break;   // 红色
    }
    QString diffStyle = QString(
        "QLabel { color: %1; font-size: 12px; font-weight: bold;"
        " background: transparent; padding: 0; }").arg(diffColor);
    m_choiceDiffLabel->setStyleSheet(diffStyle);
    m_choiceDiffLabel->setText(diffText);
    m_codeCompletionDiffLabel->setStyleSheet(diffStyle);
    m_codeCompletionDiffLabel->setText(diffText);
    m_codingDiffLabel->setStyleSheet(diffStyle);
    m_codingDiffLabel->setText(diffText);

    switch (data.type)
    {
    case QuestionType::Choice:
        m_stackedWidget->setCurrentWidget(m_choicePanel);
        m_selectedChoice = -1;
        for (QLabel* l : m_choiceLabels)
            l->setStyleSheet(
                "QLabel { color: white; background-color: #3a3a6a; border: 1px solid #777;"
                " border-radius: 4px; padding: 6px; font-size: 15px; }");
        {
            QString desc = data.description;
            desc.replace("\\n", "\n");
            m_choicePanel->findChild<QTextEdit*>("choiceDescLabel")->setPlainText(desc);
        }
        for (int i = 0; i < m_choiceLabels.size(); ++i)
        {
            if (i < data.options.size())
            {
                QString opt = data.options[i];
                opt.replace("\\n", "\n");
                // QLabel::setText 会把 & 当作快捷键助记符，需要转义
                opt.replace("&", "&&");
                m_choiceLabels[i]->setText(opt);
                m_choiceLabels[i]->show();
            }
            else
            {
                m_choiceLabels[i]->hide();
            }
        }
        break;
    case QuestionType::CodeCompletion:
        m_stackedWidget->setCurrentWidget(m_codeCompletionPanel);
        {
            QString desc = data.description;
            desc.replace("\\n", "\n");
            m_codeCompletionPanel->findChild<QTextEdit*>("codeCompletionDescLabel")->setPlainText(desc);
        }
        {
            QString tpl = data.codeTemplate;
            tpl.replace("\\n", "\n");
            m_storedCodeTemplate = tpl;
            m_storedCodeTemplate.replace("{{BLANK}}", "// 在此处补充你的代码");
            m_storedSampleInput = data.testCaseInput;
            m_storedSampleOutput = data.testCaseOutput;
        }
        m_codeCompletionEditor->clear();
        break;
    case QuestionType::Coding:
        m_stackedWidget->setCurrentWidget(m_codingPanel);
        {
            QString desc = data.description;
            desc.replace("\\n", "\n");
            m_codingPanel->findChild<QLabel*>("codingDescLabel")->setText(desc);
        }
        m_codeEditor->clear();
        if (!data.codeTemplate.isEmpty())
            m_codeEditor->setPlainText(data.codeTemplate);
        break;
    }
}

QString QuestionWidget::getAnswer() const
{
    switch (m_currentType) {
    case QuestionType::Choice: {
        if (m_selectedChoice >= 0 && m_selectedChoice < m_choiceLabels.size())
            return m_choiceLabels[m_selectedChoice]->text();
        return QString();
    }
    case QuestionType::CodeCompletion:
        return m_codeCompletionEditor->toPlainText();
    case QuestionType::Coding:
        return m_codeEditor->toPlainText();
    }
    return QString();
}

void QuestionWidget::showFeedback(bool correct)
{
    QString color = correct ? "#4caf50" : "#f44336";

    if (m_currentType == QuestionType::Choice) {
        if (m_selectedChoice >= 0 && m_selectedChoice < m_choiceLabels.size()) {
            for (int i = 0; i < m_choiceLabels.size(); ++i) {
                QLabel* label = m_choiceLabels[i];
                if (i == m_selectedChoice) {
                    label->setStyleSheet(
                        QString("QLabel { color: white; background-color: %1; border: 2px solid %1;"
                                " border-radius: 4px; padding: 6px; font-size: 15px; }").arg(color));
                } else {
                    label->setStyleSheet(
                        "QLabel { color: white; background-color: #3a3a6a; border: 1px solid #777;"
                        " border-radius: 4px; padding: 6px; font-size: 15px; }");
                }
            }
            QTimer::singleShot(400, this, [this](){
                m_selectedChoice = -1;
                for (QLabel* l : m_choiceLabels)
                    l->setStyleSheet(
                        "QLabel { color: white; background-color: #3a3a6a; border: 1px solid #777;"
                        " border-radius: 4px; padding: 6px; font-size: 15px; }");
            });
        }
    } else if (m_currentType == QuestionType::CodeCompletion) {
        QString orig = m_codeCompletionEditor->styleSheet();
        m_codeCompletionEditor->setStyleSheet(QString("QPlainTextEdit { color: white; background-color: %1;"
            " border: 2px solid %1; }").arg(color));
        QTimer::singleShot(400, this, [this, orig](){
            m_codeCompletionEditor->setStyleSheet(orig);
        });
    } else if (m_currentType == QuestionType::Coding) {
        QString orig = m_codeEditor->styleSheet();
        m_codeEditor->setStyleSheet(QString("QPlainTextEdit { color: white; background-color: %1;"
            " border: 2px solid %1; }").arg(color));
        QTimer::singleShot(400, this, [this, orig](){
            if (m_codeEditor) m_codeEditor->setStyleSheet(orig);
        });
    }
}

void QuestionWidget::reset()
{
    QWidget* current = m_stackedWidget->currentWidget();
    if (current)
        current->setStyleSheet("");
    if (m_currentType == QuestionType::Choice) {
        m_selectedChoice = -1;
        for (QLabel* l : m_choiceLabels)
            l->setStyleSheet(
                "QLabel { color: white; background-color: #3a3a6a; border: 1px solid #777;"
                " border-radius: 4px; padding: 6px; font-size: 15px; }");
    } else if (m_currentType == QuestionType::CodeCompletion) {
        m_codeCompletionEditor->clear();
    } else if (m_currentType == QuestionType::Coding) {
        m_codeEditor->clear();
    }
}

void QuestionWidget::setRemainingTolerance(int remain)
{
    if (m_currentType == QuestionType::CodeCompletion && m_codeCompletionToleranceLabel) {
        m_codeCompletionToleranceLabel->setText(QString("剩余容错次数：%1").arg(remain));
    } else if (m_currentType == QuestionType::Coding && m_codingToleranceLabel) {
        m_codingToleranceLabel->setText(QString("剩余容错次数：%1").arg(remain));
    }
}

bool QuestionWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        for (int i = 0; i < m_choiceLabels.size(); ++i) {
            if (obj == m_choiceLabels[i]) {
                for (QLabel* l : m_choiceLabels)
                    l->setStyleSheet(
                        "QLabel { color: white; background-color: #3a3a6a; border: 1px solid #777;"
                        " border-radius: 4px; padding: 6px; font-size: 15px; }");
                m_choiceLabels[i]->setStyleSheet(
                    "QLabel { color: white; background-color: #2a7a2a; border: 2px solid #4caf50;"
                    " border-radius: 4px; padding: 5px; font-size: 15px; }");
                m_selectedChoice = i;
                emit answerSubmitted();
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}
