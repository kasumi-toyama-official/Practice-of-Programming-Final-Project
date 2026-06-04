#ifndef COLLECTIONPAGE_H
#define COLLECTIONPAGE_H

#include <QWidget>

namespace Ui {
class CollectionPage;
}

class CollectionPage : public QWidget
{
    Q_OBJECT

public:
    explicit CollectionPage(QWidget *parent = nullptr);
    ~CollectionPage();

private:
    Ui::CollectionPage *ui;
};

#endif // COLLECTIONPAGE_H
