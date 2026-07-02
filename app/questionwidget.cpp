#include "questionwidget.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QMouseEvent>
#include <QTextEdit>

QuestionWidget::QuestionWidget(QWidget *parent)
    : QWidget{parent}, m_currentType(QuestionType::Choice), m_codeTemplate(""), m_selectedChoice(-1)
{
    setStyleSheet(
        "color: white; background-color: #2a2a4a;"
        "QPushButton { color: white; background-color: #3a3a6a; border: 1px solid #777; border-radius: 4px; padding: 6px; font-size: 15px; }"
        "QPushButton:hover { background-color: #5a5a8a; }"
        "QPushButton:checked { background-color: #2a7a2a; border: 2px solid #4caf50; }"
        "QLineEdit { color: white; background-color: #1e1e3a; border: 1px solid #555; border-radius: 3px; padding: 4px; }"
        "QPlainTextEdit { color: white; background-color: #1e1e3a; border: 1px solid #555; }"
        "QTextEdit { color: white; background-color: #1e1e3a; border: 1px solid #555; }");

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

    QTextEdit* descArea = new QTextEdit;
    descArea->setObjectName("choiceDescLabel");
    descArea->setReadOnly(true);
    descArea->setPlainText("选择题描述");
    descArea->setStyleSheet(
        "QTextEdit { border: none; background: transparent; color: white; font-size: 16px; }"
        "QScrollBar:vertical { width: 8px; background: #1a1c2c; }"
        "QScrollBar::handle:vertical { background: #7a7fba; min-height: 20px; }");
    layout->addWidget(descArea, 3);

    for (int i = 0; i < 4; ++i)
    {
        QLabel* label = new QLabel;
        label->setObjectName(QString("choiceLabel_%1").arg(i));
        label->setWordWrap(true);
        label->setMargin(6);
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

    QLabel* descLabel = new QLabel("代码补全题描述");
    descLabel->setObjectName("codeCompletionDescLabel");
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel, 1);

    m_codeCompletionToleranceLabel = new QLabel("剩余容错次数：0");
    m_codeCompletionToleranceLabel->setObjectName("codeCompletionToleranceLabel");
    layout->addWidget(m_codeCompletionToleranceLabel);

    m_codeCompletionTemplateEdit = new QTextEdit;
    m_codeCompletionTemplateEdit->setObjectName("codeCompletionTemplateEdit");
    m_codeCompletionTemplateEdit->setReadOnly(true);
    m_codeCompletionTemplateEdit->setPlaceholderText("代码模板");
    m_codeCompletionTemplateEdit->setStyleSheet("font-family: monospace;");
    layout->addWidget(m_codeCompletionTemplateEdit, 2);

    m_codeCompletionEditor = new QPlainTextEdit;
    m_codeCompletionEditor->setObjectName("codeCompletionEditor");
    m_codeCompletionEditor->setPlaceholderText("请在此处输入补全的代码片段...");
    m_codeCompletionEditor->setStyleSheet("font-family: monospace;");
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
            m_codeCompletionPanel->findChild<QLabel*>("codeCompletionDescLabel")->setText(desc);
        }
        {
            QString tpl = data.codeTemplate;
            tpl.replace("\\n", "\n");
            // 使用显式占位提示，方便用户定位需要补全的位置
            tpl.replace("{{BLANK}}", "/* 在此处补充你的代码 */");
            m_codeCompletionPanel->findChild<QTextEdit*>("codeCompletionTemplateEdit")->setPlainText(tpl);
        }
        m_codeCompletionPanel->findChild<QPlainTextEdit*>("codeCompletionEditor")->clear();
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
    QWidget* current = m_stackedWidget->currentWidget();
    if (!current) return;
    QString originalStyle = current->styleSheet();
    if (correct)
        current->setStyleSheet("background-color: rgba(0, 200, 0, 80);");
    else
        current->setStyleSheet("background-color: rgba(200, 0, 0, 80);");
    QTimer::singleShot(400, this, [current, originalStyle](){
        if (current) current->setStyleSheet(originalStyle);
    });
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
    if (m_currentType == QuestionType::CodeCompletion && m_codeCompletionToleranceLabel)
        m_codeCompletionToleranceLabel->setText(QString("剩余容错次数：%1").arg(remain));
    else if (m_currentType == QuestionType::Coding && m_codingToleranceLabel)
        m_codingToleranceLabel->setText(QString("剩余容错次数：%1").arg(remain));
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
