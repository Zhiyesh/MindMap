#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    labels(vector<MovableLabel*>()),
    recent_changed_labels(list<vector<MovableLabel*>>(1, labels))
{
    //此界面
    resize(1546, 967);
    setStyleSheet("font: 11.5pt \"黑体\";");

    //撤销重做操作
    connect(this, &MainWindow::addOrRemoveLabel, [this]() {
        if (!recent_changed_labels.empty()
                && recent_changed_labels.size() > 1)
        {
            _M_Undo->setEnabled(true);

            if (__currentOP != recent_changed_labels.end()
                    && __currentOP != (--recent_changed_labels.end()))
            {
                _M_Redo->setEnabled(true);
            }
            else _M_Redo->setEnabled(false);
        }
        else
        {
            _M_Undo->setEnabled(false);
            _M_Redo->setEnabled(false);
        }

        if (!labels.empty())
        {
            _M_ClearAllLabel->setEnabled(true);
        }
        else _M_ClearAllLabel->setEnabled(false);
    });
    connect(this, &MainWindow::undoOrRedoOper, [this]() {
        //Undo
        if (__currentOP != recent_changed_labels.begin())
        {
            _M_Undo->setEnabled(true);
        }
        else _M_Undo->setEnabled(false);

        //Redo
        if (__currentOP != recent_changed_labels.end()
                && __currentOP != (--recent_changed_labels.end()))
        {
            _M_Redo->setEnabled(true);
            _M_ClearAllLabel->setEnabled(true);
        }
        else _M_Redo->setEnabled(false);
    });

    //菜单栏
    _M_MenuBar = new QMenuBar(this);
    _M_MenuBar->setFixedSize(width() - 30, FONT_HEIGHT);
    _M_MenuBar->move(_M_MenuBar->x() + 15, _M_MenuBar->y());
    _M_MenuBar->setStyleSheet("background-color: rgb(0, 0, 0, 0);");

    const QString __Menu_Item_Style = QString(
        "QMenu::item {"
            "padding: 6px 18px 6px 32px;"
        "}"
        "QMenu::item:selected {"
            "background-color: rgb(158, 210, 246);"
        "}"
    );

    _M_File = new QMenu("文件", this);
    _M_File->setStyleSheet(__Menu_Item_Style);

    _M_Edit = new QMenu("编辑", this);
    _M_Edit->setStyleSheet(__Menu_Item_Style);
    _M_Edit->setEnabled(false);

    _M_AddLabel = new QMenu("添加", this);
    _M_AddLabel->setEnabled(false);

    _M_Text = new QMenu("文字", this);
    _M_Text->setStyleSheet(__Menu_Item_Style);
    _M_Text->setEnabled(false);

    _M_Widget = new QMenu("界面", this);
    _M_Widget->setStyleSheet(__Menu_Item_Style);
    _M_Widget->setEnabled(false);

    _M_MenuBar->addMenu(_M_File);
    _M_MenuBar->addMenu(_M_Edit);
    _M_MenuBar->addMenu(_M_AddLabel);
    _M_MenuBar->addMenu(_M_Text);
    _M_MenuBar->addMenu(_M_Widget);

    //文件栏
    _M_NewLayout = new QAction("新建\x20\x20\x20\x20", _M_File);
    _M_NewLayout->setShortcut(QKeySequence::New);

    _M_OpenFile = new QAction("打开", _M_File);
    _M_OpenFile->setShortcut(QKeySequence::Open);

    _M_SaveFile = new QAction("保存", _M_File);
    _M_SaveFile->setShortcut(QKeySequence::Save);
    _M_SaveFile->setEnabled(false);

    _M_CloseFile = new QAction("关闭", _M_File);
    _M_CloseFile->setShortcut(QKeySequence("Ctrl+W"));
    _M_CloseFile->setEnabled(false);

    connect(_M_NewLayout, &QAction::triggered, this, &MainWindow::newLayout);

    connect(_M_OpenFile, &QAction::triggered, [this]() { openFile(); });

    connect(_M_SaveFile, &QAction::triggered, this, &MainWindow::saveFile);

    connect(_M_CloseFile, &QAction::triggered, this, &MainWindow::closeFile);

    _M_File->addAction(_M_NewLayout);
    _M_File->addAction(_M_OpenFile);
    _M_File->addAction(_M_SaveFile);
    _M_File->addAction(_M_CloseFile);

    //编辑栏
    _M_Undo = new QAction("撤销", _M_Edit);
    _M_Undo->setShortcut(QKeySequence::Undo);
    _M_Undo->setEnabled(false);

    _M_Redo = new QAction("重做", _M_Edit);
    _M_Redo->setShortcut(QKeySequence::Redo);
    _M_Redo->setEnabled(false);

    _M_ClearAllLabel = new QAction("清空", _M_Edit);
    _M_ClearAllLabel->setEnabled(false);

    connect(_M_ClearAllLabel, &QAction::triggered, [this]() {
        removeLabel(Q_NULLPTR, Ml::AllLabels);
    });

    connect(_M_Undo, &QAction::triggered, this, &MainWindow::undoRecent);

    connect(_M_Redo, &QAction::triggered, this, &MainWindow::cancelUndo);

    _M_Edit->addAction(_M_Undo);
    _M_Edit->addAction(_M_Redo);
    _M_Edit->addAction(_M_ClearAllLabel);

    //添加控件栏
    _M_NewBracket = new QAction(QIcon(":/drawable/res/Bracket.png"), "括号", _M_AddLabel);

    _M_NewHLine = new QAction(QIcon(":/drawable/res/HLine.png"), "横线", _M_AddLabel);

    connect(_M_NewBracket, &QAction::triggered, [this]() {
        MovableLabel* currentML = newLabel(32, 183, Ml::Bracket);
        emit currentML->click();
    });

    connect(_M_NewHLine, &QAction::triggered, [this]() {
        MovableLabel* currentML = newLabel(138, 26, Ml::HLine);
        emit currentML->click();
    });

    _M_AddLabel->addAction(_M_NewBracket);
    _M_AddLabel->addAction(_M_NewHLine);

    //文字栏
    _M_AddText = new QAction("添加文字", _M_Text);

    connect(_M_AddText, &QAction::triggered, [this]() {
        if (!font_input->isHidden())
        {
            font_input->setFocus();
        }
        //Set cursor style
        setCursor(Qt::IBeamCursor);
        selectingFontPos = true;

        //鼠标左键确定文字位置
        while (!selectedFontPos)
        {
            if (isHidden()) break;
            if (canceledSelectFontPos)
            {
                canceledSelectFontPos = false;
                setCursor(Qt::ArrowCursor);
                return;
            }
            Process::continueSleep(1e2);
        }
        QPoint mouse = mapFromGlobal(cursor().pos());
        font_input->move(mouse.x(), mouse.y() - (font_input->height() >> 1));
        font_input->show();
        font_input->setFocus();
        selectedFontPos = false;
        //Recover the cursor
        setCursor(Qt::ArrowCursor);
    });

    _M_Text->addAction(_M_AddText);

    label_menu = new QMenu(this);
    label_menu->setStyleSheet(__Menu_Item_Style);

    //菜单移除项
    label_menu_remove = new QAction("移除", this);

    connect(label_menu_remove, &QAction::triggered, [this]() {
        if (chosedLabel != Q_NULLPTR)
        {
            removeLabel(chosedLabel);
            chosedLabel = Q_NULLPTR;
        }

        shade_bgnd_widget->hide();
    });

    label_menu->addAction(label_menu_remove);

    //菜单编辑项
    label_menu_font_edit = new QAction("编辑", this);

    connect(label_menu_font_edit, &QAction::triggered, [this]() {
        if (!chosedLabel->isFont()) return;

        font_input->show();
        font_input->setFocus();
        font_input->setText(chosedLabel->text());
        font_input->move(bgnd_widget->mapToParent(
            QPoint(chosedLabel->x(), chosedLabel->y()))
        );
        removeLabel(chosedLabel);

        shade_bgnd_widget->hide();
    });

    //界面栏
    _M_ResizeWidget = new QAction("设置画布大小\x20\x20", _M_Widget);
    _M_ResizeWidget->setShortcut(QKeySequence("Ctrl+R"));

    connect(_M_ResizeWidget, &QAction::triggered, [this]() {
        if (bgnd_widget->isHidden()) return;

        _M_ResizeWidget->setEnabled(false);

        ResizeWidgetDialog resize_widget_dialog;

        connect(&resize_widget_dialog, &ResizeWidgetDialog::returnSize, [this](const QSize &size) {
            if (size.width() < 100 || size.height() < 100) return;
            bgnd_widget->resize(size);
        });

        resize_widget_dialog.move(
            x() + (width() >> 1) - (resize_widget_dialog.width() >> 1),
            y() + (height() >> 1) - (resize_widget_dialog.height() >> 1)
        );

        resize_widget_dialog.display(bgnd_widget->size());

        while (!resize_widget_dialog.isHidden())
        {
            if (isMinimized() || isHidden()) break;
            if (!resize_widget_dialog.focus()) break;

            Process::continueSleep(5e1);
        }

        _M_ResizeWidget->setEnabled(true);
    });

    _M_Widget->addAction(_M_ResizeWidget);

    //文本输入框
    font_input = new QLineEdit(this);
    font_input->resize(FONT_LABEL_SIZE * 2, FONT_HEIGHT);
    font_input->setContextMenuPolicy(Qt::NoContextMenu);
    font_input->setStyleSheet(QString("font: %1pt \"黑体\"; background-color: rgb(0, 0, 0, 0); border: 1.5px dotted;").arg(FONT_LABEL_SIZE));
    font_input->hide();

    connect(font_input, &QLineEdit::textChanged, [this]() {
        //Adaptive width
        int __width = FONT_LABEL_SIZE * 2;
        for (int i = 0; i < font_input->text().size(); i++)
        {
            const QChar each = font_input->text().at(i);
            if (each.isNumber() || each.isSpace()
                    || (each > 'A' && each < 'Z') || (each > 'a' && each < 'z'))
            {
                __width += FONT_LABEL_SIZE + FONT_LABEL_SIZE_INCRE;
            }
            else __width += (FONT_LABEL_SIZE + FONT_LABEL_SIZE_INCRE) * 2;
        }
        font_input->resize(__width, font_input->height());
    });

    connect(font_input, &QLineEdit::returnPressed, this, &MainWindow::buildFont);

    //初始化画布
    bgnd_widget = new Painting(this);
    bgnd_widget->setGeometry(BG_WIDGET_X, BG_WIDGET_Y, BG_WIDGET_WIDTH, BG_WIDGET_HEIGHT);
    bgnd_widget->setStyleSheet("background-color: rgb(232, 232, 232);");
    bgnd_widget->lower();
    bgnd_widget->hide();

    connect(bgnd_widget, &Painting::sizeChanged, [this]() {
        shade_bgnd_widget->setFixedSize(bgnd_widget->size());
        resizeEvent(Q_NULLPTR);
    });

    //画布遮罩
    shade_bgnd_widget = new QLabel(bgnd_widget);
    shade_bgnd_widget->setFixedSize(bgnd_widget->size());
    shade_bgnd_widget->move(0, 0);
    shade_bgnd_widget->setStyleSheet("background-color: rgb(232, 232, 232, 167);");
    shade_bgnd_widget->hide();

    //无文件提示
    _M_NoFileIsOpen = new QLabel("无文件打开", this);
    _M_NoFileIsOpen->setFixedSize(5 * 3 * 12, FONT_HEIGHT);
    _M_NoFileIsOpen->move((width() >> 1) - (_M_NoFileIsOpen->width() >> 1),
                       (height() >> 1) - (_M_NoFileIsOpen->height() >> 1));
    _M_NoFileIsOpen->setStyleSheet("font: 12pt;");

    //竖向滚动条
    _M_VSclBar = new QScrollBar(Qt::Vertical, this);
    _M_VSclBar->setFixedSize(30, (height() - 2 * FONT_HEIGHT) >> 1);
    _M_VSclBar->move(width() - _M_VSclBar->width(), height() >> 1);

    connect(_M_VSclBar, &QScrollBar::valueChanged, [this]() {
        bgnd_widget->move(bgnd_widget->x(), BG_WIDGET_Y - _M_VSclBar->value());
    });

    //横向滚动条
    _M_HSclBar = new QScrollBar(Qt::Horizontal, this);
    _M_HSclBar->setFixedSize(_M_VSclBar->height(), _M_VSclBar->width());
    _M_HSclBar->move(width() - _M_HSclBar->width() - FONT_HEIGHT, height() - _M_HSclBar->height());

    connect(_M_HSclBar, &QScrollBar::valueChanged, [this]() {
        bgnd_widget->move(BG_WIDGET_X - _M_HSclBar->value(), bgnd_widget->y());
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (_M_SaveFile->isEnabled() && existedFileToSave())
    {
        e->ignore();
        return;
    }

    //Close Handle
    clearChangedLabelsList();

    e->accept();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    _M_NoFileIsOpen->move((width() >> 1) - (_M_NoFileIsOpen->width() >> 1),
                             (height() >> 1) - (_M_NoFileIsOpen->height() >> 1));

    _M_VSclBar->setFixedSize(30, (height() - 2 * FONT_HEIGHT) >> 1);
    _M_VSclBar->move(width() - _M_VSclBar->width(), height() >> 1);

    _M_HSclBar->setFixedSize(_M_VSclBar->height(), _M_VSclBar->width());
    _M_HSclBar->move(width() - _M_HSclBar->width() - FONT_HEIGHT, height() - _M_HSclBar->height());

    const int __bgnd_h_edge = BG_WIDGET_X + bgnd_widget->width();
    const int __bgnd_v_edge = BG_WIDGET_Y + bgnd_widget->height();

    const int __widget_h_edge = width() - _M_VSclBar->width();
    const int __widget_v_edge = height() - _M_HSclBar->height();

    if (__bgnd_h_edge > __widget_h_edge)
    {
        _M_HSclBar->show();
        bgnd_widget->move(BG_WIDGET_X - _M_HSclBar->value(), bgnd_widget->y());
        _M_HSclBar->setMaximum(__bgnd_h_edge - __widget_h_edge);
        _M_HSclBar->raise();
    }
    else
    {
        _M_HSclBar->hide();
        bgnd_widget->move(BG_WIDGET_X, bgnd_widget->y());
    }

    if (__bgnd_v_edge > __widget_v_edge)
    {
        _M_VSclBar->show();
        bgnd_widget->move(bgnd_widget->x(), BG_WIDGET_Y - _M_VSclBar->value());
        _M_VSclBar->setMaximum(__bgnd_v_edge - __widget_v_edge);
        _M_VSclBar->raise();
    }
    else
    {
        _M_VSclBar->hide();
        bgnd_widget->move(bgnd_widget->x(), BG_WIDGET_Y);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    //遮罩隐藏
    shade_bgnd_widget->hide();

    if (e->button() == Qt::LeftButton)
    {
        //左键确定字体控件位置
        if (selectingFontPos && !selectedFontPos)  //当鼠标为 I 时
        {
            selectedFontPos = true;
            selectingFontPos = false;
        }
        //再次点击左键
        else buildFont();
    }

    if (e->button() == Qt::RightButton)
    {
        //右键取消放置字体控件
        if (selectingFontPos && !selectedFontPos)  //当鼠标为 I 时
        {
            canceledSelectFontPos = true;
            selectedFontPos = false;
            selectingFontPos = false;
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    //未选中控件
    if (!_M_HasLabelMoving)
    {
        //按键快速添加控件
        if (_M_AddLabel->isEnabled() && _M_Text->isEnabled())
        {
            switch (e->key())
            {
            case Qt::Key_1:
                emit _M_NewBracket->triggered();
                break;
            case Qt::Key_2:
                emit _M_NewHLine->triggered();
                break;
            case Qt::Key_F:
                emit _M_AddText->triggered();
                break;
            }
        }
    }
    //已选中控件
    else
    {
        switch (e->key())
        {
        //回车键确定控件位置
        case Qt::Key_Return:
            if (chosedLabel != Q_NULLPTR)
            {
                emit chosedLabel->click();
                chosedLabel = Q_NULLPTR;
            }
            break;
        //删除控件
        case Qt::Key_Delete:
            {
                _M_HasLabelMoving = false;
                removeLabel(chosedLabel);
            }
            break;
        //方向键移动控件
        case Qt::Key_Left:
            QCursor::setPos(QCursor::pos().x() - 1, QCursor::pos().y());
            break;
        case Qt::Key_Right:
            QCursor::setPos(QCursor::pos().x() + 1, QCursor::pos().y());
            break;
        case Qt::Key_Up:
            QCursor::setPos(QCursor::pos().x(), QCursor::pos().y() - 1);
            break;
        case Qt::Key_Down:
            QCursor::setPos(QCursor::pos().x(), QCursor::pos().y() + 1);
            break;
        }
    }
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
    //控件大小变化步长
    const int LABEL_STEP = 2;
    //控件最大和最小大小
    const int LABEL_MAX = 343;
    const int LABEL_MIN = 17;

    //滚动条滑动步长
    const int SLIDER_STEP = 10;

    //向上滚动
    if (e->angleDelta().y() > 0)
    {
        //控件缩小
        if (_M_HasLabelMoving)
        {
            if (!chosedLabel->isFont()
                && chosedLabel->width() > LABEL_MIN && chosedLabel->height() > LABEL_MIN)
            {
                double __ratio = (double)chosedLabel->width() / (double)chosedLabel->height();
                if (__ratio > 1)
                {
                    int __chheight = chosedLabel->height() - LABEL_STEP;
                    chosedLabel->setFixedSize(__chheight * __ratio, __chheight);
                }
                else
                {
                    int __chwidth = chosedLabel->width() - LABEL_STEP;
                    chosedLabel->setFixedSize(__chwidth, (int)__chwidth / __ratio);
                }
            }
            else
            {
                chosedLabel->setTextSize(chosedLabel->textSize() - 1);
                chosedLabel->setFixedWidth(chosedLabel->text().size() * chosedLabel->textSize() * 2);
                chosedLabel->setFixedHeight(chosedLabel->width() >> 1);
            }

            return;
        }

        if (!_M_VSclBar->isHidden())
        {
            //竖向滚动条上滑
            if (e->modifiers() == Qt::NoModifier)
            {
                _M_VSclBar->setValue(_M_VSclBar->value() - SLIDER_STEP);
            }
        }

        if (!_M_HSclBar->isHidden())
        {
            //横向滚动条左滑
            if (e->modifiers() == Qt::ControlModifier)
            {
                _M_HSclBar->setValue(_M_HSclBar->value() - SLIDER_STEP);
            }
        }
    }
    //向下滚动
    else if (e->angleDelta().y() < 0)
    {
        //控件放大
        if (_M_HasLabelMoving)
        {
            if (!chosedLabel->isFont()
                && chosedLabel->width() < LABEL_MAX && chosedLabel->height() < LABEL_MAX)
            {
                double __ratio = (double)chosedLabel->width() / (double)chosedLabel->height();
                if (__ratio > 1)
                {
                    int __chheight = chosedLabel->height() + LABEL_STEP;
                    chosedLabel->setFixedSize(__chheight * __ratio, __chheight);
                }
                else
                {
                    int __chwidth = chosedLabel->width() + LABEL_STEP;
                    chosedLabel->setFixedSize(__chwidth, (int)__chwidth / __ratio);
                }
            }
            else
            {
                chosedLabel->setTextSize(chosedLabel->textSize() + 1);
                chosedLabel->setFixedWidth(chosedLabel->text().size() * chosedLabel->textSize() * 2);
                chosedLabel->setFixedHeight(chosedLabel->width() >> 1);
            }

            return;
        }

        if (!_M_VSclBar->isHidden())
        {
            //竖向滚动条下滑
            if (e->modifiers() == Qt::NoModifier)
            {
                _M_VSclBar->setValue(_M_VSclBar->value() + SLIDER_STEP);
            }
        }

        if (!_M_HSclBar->isHidden())
        {
            //横向滚动条右滑
            if (e->modifiers() == Qt::ControlModifier)
            {
                _M_HSclBar->setValue(_M_HSclBar->value() + SLIDER_STEP);
            }
        }
    }
    //向左滚动
    else if (e->angleDelta().x() < 0)
    {
        if (!_M_HSclBar->isHidden())
        {
            //横向滚动条右滑
            _M_HSclBar->setValue(_M_HSclBar->value() + SLIDER_STEP);
        }
    }
    //向右滚动
    else if (e->angleDelta().x() > 0)
    {
        if (!_M_HSclBar->isHidden())
        {
            //横向滚动条左滑
            _M_HSclBar->setValue(_M_HSclBar->value() - SLIDER_STEP);
        }
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    const QList<QUrl> urls = e->mimeData()->urls();
    if (urls.isEmpty()) return;

    //多个文件时，只读第一个文件
    QString filePath = urls.first().toLocalFile();
    if (!filePath.isEmpty())
    {
        openFile(filePath);
    }
}

void MainWindow::changeWidget(const int &step)
{
    const int targetHeight = height() + step;
    if (targetHeight < 300
        || targetHeight > QApplication::desktop()->screenGeometry().height())
            return;
    setFixedSize(width() + step, height() + step);
    move(x() - (step >> 1), y() - (step >> 1));
    _M_MenuBar->setFixedSize(_M_MenuBar->width() + step, _M_MenuBar->height());
    _M_NoFileIsOpen->move((width() >> 1) - (_M_NoFileIsOpen->width() >> 1),
                       (height() >> 1) - (_M_NoFileIsOpen->height() >> 1));
}

void MainWindow::removeLabel(MovableLabel *label, const Ml::LabelSelect &select)
{
    bool flag = false;
    if (select == Ml::AllLabels)
    {
        flag = true;
        font_input->hide();
        setFocus();
        font_input->setText("");
    }

    for (vector<MovableLabel*>::iterator it = labels.begin(); it != labels.end(); it++)
    {
        MovableLabel* currentLabel = *it;

        if (currentLabel == Q_NULLPTR) continue;

        if (currentLabel == label || flag)
        {
            currentLabel->hide();
            labels.erase(it--);

            if (!flag) break;
        }
    }

    recent_changed_labels.push_back(labels);
    __currentOP = --recent_changed_labels.end();

    if (!(filePath.isEmpty() && labels.empty())) _M_SaveFile->setEnabled(true);
    else _M_SaveFile->setEnabled(false);

    emit addOrRemoveLabel();
}

MovableLabel*
MainWindow::newLabel(const int &w,
                   const int &h,
                   const Ml::LabelType &type,
                   const QString &text,
                   const int &text_size,
                   const bool &flag)
{
    MovableLabel* const label = new MovableLabel(w, h, type, text, text_size, bgnd_widget);
    label->show();

    //控件移动
    connect(label, &MovableLabel::click, [=]() {
        if (selectingFontPos) return;
        if (!label_menu->isHidden()) return;

        _M_HasLabelMoving = !_M_HasLabelMoving;
        if (_M_HasLabelMoving)
        {
            setCursor(Qt::SizeAllCursor);
        }

        chosedLabel = label;

        shade_bgnd_widget->show();
        shade_bgnd_widget->raise();
        label->raise();

        while (_M_HasLabelMoving)
        {
            if (isHidden()) break;

            QPoint mouse = bgnd_widget->mapFromGlobal(cursor().pos());
            label->move(QPoint(mouse.x() - (label->width() >> 1),
                               mouse.y() - (label->height() >> 1)));
            Process::continueSleep(1e1);
        }
        shade_bgnd_widget->hide();

        setCursor(Qt::ArrowCursor);
        chosedLabel = Q_NULLPTR;
        _M_SaveFile->setEnabled(true);
    });

    //控件菜单栏呼出
    connect(label, &MovableLabel::clickRight, [=]() {
        if (_M_HasLabelMoving)
        {
            emit label->click();
            return;
        }
        chosedLabel = label;

        if (label->isFont())
        {
            if (label_menu->actions().at(0) != label_menu_font_edit)
            {
                label_menu->insertAction(label_menu_remove,
                                               label_menu_font_edit);
            }
        }
        else label_menu->removeAction(label_menu_font_edit);

        label_menu->move(bgnd_widget->mapToGlobal(
            QPoint(label->x() + (label->width() >> 1) - (FONT_HEIGHT >> 1),
                   label->y() + (label->height() >> 1) + (FONT_HEIGHT >> 1))
        ));

        shade_bgnd_widget->show();
        label_menu->show();

        shade_bgnd_widget->raise();
        label->raise();
        label_menu->raise();
    });

//    //字体控件编辑
//    connect(label, &MovableLabel::doubleClick, [=]() {
//        emit label->click();
//        if (!label->isFont()) return;

//        font_input->show();
//        font_input->setFocus();
//        font_input->setText(label->text());
//        font_input->move(bgnd_widget->mapToParent(QPoint(label->x(), label->y())));
//        removeLabel(label);
//    });

    labels.push_back(label);

    if (!flag)
    {
        recent_changed_labels.push_back(labels);
        __currentOP = --recent_changed_labels.end();
    }

    _M_SaveFile->setEnabled(true);

    emit addOrRemoveLabel();

    return label;
}

void MainWindow::undoRecent()
{
    if (recent_changed_labels.empty()
        || recent_changed_labels.size() <= 1) return;

    if (__currentOP != recent_changed_labels.begin())
    {
        for (auto it = labels.begin(); it != labels.end(); it++)
        {
            (*it)->hide();
        }
        labels.clear();

        --__currentOP;

        labels = *(__currentOP);
        _M_SaveFile->setEnabled(true);

        for (auto it = labels.begin(); it != labels.end(); it++)
        {
            (*it)->show();
        }
    }

    emit undoOrRedoOper();
}

void MainWindow::cancelUndo()
{
    if (recent_changed_labels.empty()
        || recent_changed_labels.size() <= 1) return;

    if (__currentOP != recent_changed_labels.end()
            && __currentOP != (--recent_changed_labels.end()))
    {
        for (auto it = labels.begin(); it != labels.end(); it++)
        {
            (*it)->hide();
        }
        labels.clear();

        ++__currentOP;

        labels = *(__currentOP);
        _M_SaveFile->setEnabled(true);

        for (auto it = labels.begin(); it != labels.end(); it++)
        {
            (*it)->show();
        }
    }

    emit undoOrRedoOper();
}

void MainWindow::buildFont()
{
    selectedFontPos = false;
    font_input->hide();
    setFocus();

    QString text = font_input->text();
    if (!text.isEmpty())
    {
        //创建Label控件替换LineEdit控件
        MovableLabel *label = newLabel(font_input->width() - FONT_LABEL_SIZE * 2,
                                             font_input->height(), Ml::FontType, text);
        label->move(bgnd_widget->mapFromParent(
            QPoint(font_input->x(), font_input->y())
        ));
    }
    font_input->setText("");
}

bool MainWindow::existedFileToSave()
{
    //提示是否要保存当前的labels
    int result = QMessageBox::information(
        this,
        "保存文件",
        "文件未保存的更改\t",
        "保存",
        "不保存",
        "取消",
        0,
        2
    );
    if (result == 2) return true;  //取消

    if (result == 0)  //保存
    {
        if (!saveFile()) return true;
    }
    //放弃  不执行任何代码

    return false;
}

bool MainWindow::newLayout(const bool &flag)
{
    if (_M_SaveFile->isEnabled() && existedFileToSave()) return false;

    removeLabel(Q_NULLPTR, Ml::AllLabels);
    clearChangedLabelsList();
    _M_Undo->setEnabled(false);
    _M_Redo->setEnabled(false);

    _M_SaveFile->setEnabled(false);
    _M_CloseFile->setText("关闭");

    filePath = QString();
    bgnd_widget->resize(BG_WIDGET_WIDTH, BG_WIDGET_HEIGHT);

    if (flag) return true;

    _M_Edit->setEnabled(true);
    _M_AddLabel->setEnabled(true);
    _M_Text->setEnabled(true);
    _M_Widget->setEnabled(true);

    _M_CloseFile->setEnabled(true);
    _M_NoFileIsOpen->hide();
    bgnd_widget->show();

    emit _M_ResizeWidget->triggered();

    return true;
}

void MainWindow::openFile(QString filePath)
{
    if (_M_SaveFile->isEnabled() && existedFileToSave()) return;

    if (filePath.isEmpty())
    {
        filePath = QFileDialog::getOpenFileName(this,
                        "打开文件",
                        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                        "Json文件 (*.json)");
    }
    if (filePath.isEmpty()) return;

    filePath = filePath;

    //读文件
    QFile fileManager(filePath);
    if (!fileManager.open(QFile::ReadOnly))
    {
        QMessageBox::warning(this, "失败", "打开失败！\n请检查文件是否正确\t");
        return;
    }

    QString fileData = fileManager.readAll();

    QJsonDocument labelInfo = QJsonDocument::fromJson(fileData.toUtf8());

    if (!labelInfo.isObject())
    {
        QMessageBox::warning(this, "错误", "文件无效！\n请检查后重试\t");
        return;
    }

    if (!labels.empty())
    {
        removeLabel(Q_NULLPTR, Ml::AllLabels);
    }

    if (!recent_changed_labels.empty())
    {
        clearChangedLabelsList(true);
    }

    QJsonObject labelObj = labelInfo.object();
    jsonToLabel(labelObj);

    recent_changed_labels.push_back(labels);
    __currentOP = --recent_changed_labels.end();

    _M_NoFileIsOpen->hide();
    bgnd_widget->show();

    _M_SaveFile->setEnabled(false);

    _M_CloseFile->setEnabled(true);
    _M_CloseFile->setText("关闭文件\x20\x20");

    _M_Edit->setEnabled(true);
    _M_AddLabel->setEnabled(true);
    _M_Text->setEnabled(true);
    _M_Widget->setEnabled(true);
}

void MainWindow::jsonToLabel(const QJsonObject &object)
{
    int x = 0, y = 0, w = 0, h = 0, type = 0;
    QString text = QString();

    QStringList __keys = object.keys();
    for (const QString &key : __keys)
    {
        QJsonValue value = object.value(key);
        if (value.isObject())
        {
            jsonToLabel(value.toObject());
            continue;
        }

        if (value.isArray())
        {
            const QJsonArray __arr = value.toArray();
            if (key == "widget") bgnd_widget->resize(__arr[0].toInt(), __arr[1].toInt());
        }

        if (value.isDouble())
        {
            if (key == "x") x = value.toDouble();
            if (key == "y") y = value.toDouble();
            if (key == "width") w = value.toDouble();
            if (key == "height") h = value.toDouble();
            if (key == "type") type = value.toDouble();
        }

        if (value.isString())
        {
            if (key == "text")
            {
                text = value.toString();
            }
        }
    }

    if (w * h != 0)
    {
        MovableLabel* label = newLabel(w, h, Ml::LabelType(type), text, 0, true);
        label->move(x, y);
    }
}

bool MainWindow::saveFile()
{
    if (filePath.isEmpty())
    {
        filePath = QFileDialog::getSaveFileName(
                    this, "保存文件",
                    QString((filePath.isNull() || filePath.isEmpty())
                            ? QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) : filePath),
                    "Json文件 (*.json)"
                    );
        if (filePath.isEmpty()) return false;
        _M_CloseFile->setEnabled(true);
        _M_CloseFile->setText("关闭文件");
    }

    //filePath is not empty

    //写文件
    QJsonDocument __label_info;

    QJsonObject __flag;
    QJsonObject __widget;
    QJsonArray __size;
    QJsonObject __label_obj;

    for (size_t i = 0; i < labels.size(); i++)
    {
        MovableLabel* const currentLabel = labels[i];
        QJsonObject currentObj;
        currentObj.insert("x", QJsonValue(currentLabel->x()));
        currentObj.insert("y", QJsonValue(currentLabel->y()));
        currentObj.insert("width", QJsonValue(currentLabel->width()));
        currentObj.insert("height", QJsonValue(currentLabel->height()));
        currentObj.insert("type", QJsonValue(currentLabel->type()));
        currentObj.insert("text", QJsonValue(currentLabel->text()));
        __label_obj.insert(QString::number(i + 1), QJsonValue(currentObj));
    }

    __size.append(bgnd_widget->width());
    __size.append(bgnd_widget->height());
    __widget.insert("widget", __size);
    __widget.insert("content", __label_obj);
    __flag.insert("Build by MindMap", __widget);

    __label_info.setObject(__flag);
    QByteArray fileData =  __label_info.toJson();

    QFile fileManager(filePath);
    if (fileManager.open(QFile::WriteOnly))
    {
        fileManager.write(fileData);
        fileManager.close();
        _M_SaveFile->setEnabled(false);
        return true;
    }
    else
    {
        QMessageBox::warning(this, "失败", "文件保存失败！\n原因未知\t");
        return false;
    }
}

void MainWindow::closeFile()
{
    if (!newLayout(true)) return;

    _M_Edit->setEnabled(false);
    _M_AddLabel->setEnabled(false);
    _M_Text->setEnabled(false);
    _M_Widget->setEnabled(false);

    _M_CloseFile->setEnabled(false);
    _M_NoFileIsOpen->show();
    bgnd_widget->hide();
}

void MainWindow::clearChangedLabelsList(const bool &flag)
{
    for (auto i = recent_changed_labels.begin();
         i != recent_changed_labels.end(); i++)
    {
        for (auto it = i->begin(); it != i->end(); it++)
        {
            MovableLabel* label = *it;
            if (label == Q_NULLPTR) continue;

            label->deleteLater();
            label = Q_NULLPTR;
        }
    }

    recent_changed_labels.clear();
    __currentOP = recent_changed_labels.end();

    if (!flag)
    {
        recent_changed_labels.push_back(vector<MovableLabel*>());
        __currentOP = --recent_changed_labels.end();
    }
}

