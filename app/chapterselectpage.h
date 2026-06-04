#ifndef CHAPTERSELECTPAGE_H
#define CHAPTERSELECTPAGE_H

#include <QWidget>
#include "battlepage.h"

class ChapterSelectPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChapterSelectPage(QWidget *parent = nullptr);
    ~ChapterSelectPage();

private:
    void* ui;
};

#endif // CHAPTERSELECTPAGE_H
