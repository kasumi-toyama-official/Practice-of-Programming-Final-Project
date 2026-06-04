#ifndef ERRORBOOKPAGE_H
#define ERRORBOOKPAGE_H

#include <QWidget>

namespace Ui {
class ErrorBookPage;
}

class ErrorBookPage : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorBookPage(QWidget *parent = nullptr);
    ~ErrorBookPage();

private:
    Ui::ErrorBookPage *ui;
};

#endif // ERRORBOOKPAGE_H
