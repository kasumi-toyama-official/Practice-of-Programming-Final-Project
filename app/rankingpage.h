#ifndef RANKINGPAGE_H
#define RANKINGPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

#include "managers/RankingManager.h"

namespace Ui {
class RankingPage;
}

class RankingPage : public QWidget
{
    Q_OBJECT

public:
    explicit RankingPage(QWidget *parent = nullptr);
    ~RankingPage();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onBackClicked();
    void onFilterChanged(int index);

private:
    Ui::RankingPage *ui;

    RankingManager m_rankingManager;

    QComboBox *m_chapterFilter;
    QListWidget *m_rankingList;
    QLabel *m_summaryLabel;
    QPushButton *m_backButton;

    void setupUI();
    void loadRankings();
    void refreshChapterFilter();
    QString chapterNameRead(int chapterId) const;
};

#endif // RANKINGPAGE_H
