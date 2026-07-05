#ifndef ERRORBOOKPAGE_H
#define ERRORBOOKPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>

#include "managers/WrongBookManager.h"
#include "managers/QuestionBank.h"
#include "models/CodeCompletionQuestion.h"
#include "questionwidget.h"

namespace Ui {
class ErrorBookPage;
}

class ErrorBookPage : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorBookPage(QWidget *parent = nullptr);
    ~ErrorBookPage();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onBackClicked();
    void onFilterChanged(int index);
    void onItemClicked(QListWidgetItem *item);
    void onRedoClicked();
    void onDeleteClicked();
    void onSelectAllClicked();
    void onBatchDeleteClicked();

private:
    Ui::ErrorBookPage *ui;

    WrongBookManager m_wrongBookManager;
    QuestionBank m_questionBank;

    QComboBox *m_chapterFilter;
    QListWidget *m_errorList;
    QLabel *m_detailLabel;
    QPushButton *m_redoButton;
    QPushButton *m_deleteButton;
    QPushButton *m_selectAllButton;
    QPushButton *m_batchDeleteButton;
    QPushButton *m_backButton;

    int m_selectedChapterId = -1;
    int m_selectedQuestionId = -1;

    QDialog *m_redoDialog = nullptr;
    QuestionWidget *m_redoQuestionWidget = nullptr;
    QuestionType m_redoQuestionType = QuestionType::Choice;
    int m_redoCorrectIndex = -1;
    QStringList m_redoOptions;
    bool m_redoIsCodeCompletion = false;
    CodeCompletionQuestion m_redoCCQuestion;

    void setupUI();
    void loadErrors();
    void refreshChapterFilter();
    QString formatErrorItem(const WrongBookEntry& entry, const Question& q) const;
    QString findDataFile(const QString& relativePath) const;
};

#endif // ERRORBOOKPAGE_H
