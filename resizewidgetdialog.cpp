#include "resizewidgetdialog.h"

ResizeWidgetDialog::ResizeWidgetDialog(QWidget *parent) :
    QWidget(parent),
    __title(new QLabel(this)),
    __width(new QLineEdit(this)),
    __height(new QLineEdit(this)),
    __okay(new QPushButton("好", this))
{
    setWindowTitle("设置画布大小");
    setFixedSize(518, 387);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);

    __width->setFixedSize(197, 64);
    __width->setPlaceholderText("宽度");
    __width->setContextMenuPolicy(Qt::NoContextMenu);
    __width->setAlignment(Qt::AlignHCenter);
    __width->setStyleSheet("font: 11.5pt \"黑体\";");
    __width->move((width() >> 1) - __width->width() - 24, 107);

    __title->setText("设置画布大小");
    __title->setStyleSheet(__width->styleSheet() + "font-size: 10pt");
    __title->setFixedSize(__width->size());
    __title->move(__width->x(), __width->y() - 90);

    __height->setFixedSize(__width->size());
    __height->setPlaceholderText("高度");
    __height->setContextMenuPolicy(__width->contextMenuPolicy());
    __height->setAlignment(__width->alignment());
    __height->setStyleSheet(__width->styleSheet());
    __height->move(__width->x() + __width->width() + 40,
                   __width->y());

    __okay->setFixedSize(166, 63);
    __okay->setStyleSheet("font: 11pt \"黑体\";");
    __okay->move((width() >> 1) - (__okay->width() >> 1), height() - __okay->height() - 70);

    connect(__width, &QLineEdit::textChanged, this, &ResizeWidgetDialog::editTextChange);
    connect(__height, &QLineEdit::textChanged, this, &ResizeWidgetDialog::editTextChange);

    connect(__okay, &QPushButton::clicked, this, &QWidget::close);
}

ResizeWidgetDialog::~ResizeWidgetDialog()
{
}

void ResizeWidgetDialog::display(const QSize &size)
{
    __width->setText(QString::number(size.width()));
    __height->setText(QString::number(size.height()));
    show();
    __width->setFocus();
}

bool ResizeWidgetDialog::focus() const
{
    return hasFocus() || __width->hasFocus()
            || __height->hasFocus() || __okay->hasFocus();
}

void ResizeWidgetDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) this->close();

    if (e->key() == Qt::Key_Return)
    {
        if (__width->hasFocus())
        {
            __height->setFocus();
        }
        else if (__height->hasFocus())
        {
            __okay->click();
        }
    }

    if (e->key() == Qt::Key_Left)
    {
        __width->setFocus();
    }
    else if (e->key() == Qt::Key_Right)
    {
        __height->setFocus();
    }
}

void ResizeWidgetDialog::wheelEvent(QWheelEvent *e)
{
    QLineEdit* __edit = __width->hasFocus() ? __width : __height;

    if (e->angleDelta().y() > 0)
    {
        __edit->setText(QString::number(__edit->text().toInt() - 10));
    }

    if (e->angleDelta().y() < 0)
    {
        __edit->setText(QString::number(__edit->text().toInt() + 10));
    }
}

void ResizeWidgetDialog::editTextChange()
{
    QString __w = __width->text(),
            __h = __height->text();

    if (__w.isEmpty() || __h.isEmpty()) return;

    emit returnSize(
        QSize(__w.toInt(), __h.toInt())
    );
}

