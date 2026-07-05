#ifndef QUESTIONWIDGET_H
#define QUESTIONWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include "GameData.h"

class QuestionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QuestionWidget(QWidget *parent = nullptr);

    void setQuestion(const QuestionData& data);
    QString getAnswer() const;
    void showFeedback(bool correct);
    void reset();

    void setRemainingTolerance(int remain);

signals:
    void answerSubmitted();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupChoicePanel();
    void setupCodeCompletionPanel();
    void setupCodingPanel();

    QuestionType m_currentType;
    QString m_codeTemplate;
    int m_selectedChoice;

    QWidget* m_choicePanel;
    QWidget* m_codeCompletionPanel;
    QWidget* m_codingPanel;

    QStackedWidget* m_stackedWidget;
    QList<QLabel*> m_choiceLabels;

    QLabel* m_choiceDiffLabel;
    QLabel* m_choiceToleranceLabel;
    QLabel* m_codeCompletionDiffLabel;
    QLabel* m_codingDiffLabel;

    QString m_storedCodeTemplate;
    QString m_storedSampleInput;
    QString m_storedSampleOutput;
    QPlainTextEdit* m_codeCompletionEditor;
    QLabel* m_codeCompletionToleranceLabel;

    QPlainTextEdit* m_codeEditor;
    QLabel* m_codingToleranceLabel;
};

#endif // QUESTIONWIDGET_H
