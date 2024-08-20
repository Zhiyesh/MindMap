#ifndef MOVABLELABEL_H
#define MOVABLELABEL_H

#include <QLabel>
#include <QMouseEvent>

#define FONT_HEIGHT  50
#define FONT_LABEL_SIZE  13

namespace Ml {
    enum LabelType
    {
        FontType = 0,
        Bracket,  //括号
        Line    //横线
    };

    enum LabelSelect
    {
        None = 0,
        AllLabels
    };
}

const QString LabelRes[] = {
    ":/drawable/res/Bracket.png",
    ":/drawable/res/Line.png",
};

class MovableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit MovableLabel(const Ml::LabelType &type,
        const QString &text = QString(),
        const int &w = 0,
        const int &h = 0,
        const int &rotate = 0,
        const int &text_size = 0,
        QWidget *parent = 0
    );

public:
    Ml::LabelType type() const;
    void rotateRight();
    int rotateValue() const;
    bool isFont() const;
    void setTextSize(const int &text_size);
    int textSize() const;

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);

signals:
    void click();
    void clickRight();
    void doubleClick();

private:
    Ml::LabelType labelType;
    QImage ResImg;
    int rotate_value = 0;
    bool is_font = false;
    QSize scale_value;
    int text_size = 0;
};

#endif // MOVABLELABEL_H
