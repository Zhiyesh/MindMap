#ifndef RESIZEWIDGETDIALOG_H
#define RESIZEWIDGETDIALOG_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QKeyEvent>

class ResizeWidgetDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ResizeWidgetDialog(QWidget *parent = 0);
    ~ResizeWidgetDialog();

signals:
    void returnSize(const QSize& size);

public:
    void display(const QSize &size);
    bool focus() const;

protected:
    void keyPressEvent(QKeyEvent* e);
    void wheelEvent(QWheelEvent* e);

private:
    void editTextChange();

private:
    QLabel* __title;
    QLineEdit* __width;
    QLineEdit* __height;
    QPushButton* __okay;
};

#endif // RESIZEWIDGETDIALOG_H
