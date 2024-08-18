#ifndef MOVABLELABEL_H
#define MOVABLELABEL_H

#include <QLabel>
#include <QMouseEvent>


namespace Ml {
    enum LabelType
    {
        FontType = 0,
        Bracket,  //括号
        HLine    //横线
    };

    enum LabelSelect
    {
        None = 0,
        AllLabels
    };
}

class MovableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit MovableLabel(
        const int &w,
        const int &h,
        const Ml::LabelType &type,
        const QString &text = QString(),
        const int &text_size = 0,
        QWidget *parent = 0
    );

public:
    Ml::LabelType type() const;
    bool isFont() const;
    int textSize() const;
    void setTextSize(const int &text_size);

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);

signals:
    void click();
    void clickRight();
    void doubleClick();

private:
    Ml::LabelType labelType;
    bool is_font = false;
    int text_size = 0;
};

#endif // MOVABLELABEL_H
