#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//菜单栏
#include <QMenuBar>
#include <QMenu>
#include <QAction>

//滚动条
#include <QScrollBar>

//控件
#include <QLineEdit>
#include <QMessageBox>


//鼠标
#include <QMouseEvent>
#include <QMimeData>

//进程
#include <QTime>
#include <QCoreApplication>
#include <QApplication>

//文件
#include <QDesktopWidget>
#include <QStandardPaths>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <vector>
#include <list>

#include <QDebug>

#include "process.h"
#include "painting.h"
#include "movablelabel.h"
#include "resizewidgetdialog.h"

#define FONT_HEIGHT  50

#define BG_WIDGET_X  0
#define BG_WIDGET_Y  FONT_HEIGHT
#define BG_WIDGET_WIDTH   1246
#define BG_WIDGET_HEIGHT  667

#define FONT_LABEL_SIZE  13
#define FONT_LABEL_SIZE_INCRE 5

using std::vector;
using std::list;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void addOrRemoveLabel();
    void undoOrRedoOper();

protected:
    void closeEvent(QCloseEvent* e);
    void resizeEvent(QResizeEvent*);

    void mousePressEvent(QMouseEvent* e);
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);
    void wheelEvent(QWheelEvent* e);
    void focusOutEvent(QFocusEvent*);

    void dragEnterEvent(QDragEnterEvent* e);
    void dropEvent(QDropEvent* e);

protected:
    void changeWidget(const int &step);
    void removeLabel(MovableLabel *label, const Ml::LabelSelect &select = Ml::None);

    MovableLabel*
    newLabel(const Ml::LabelType &type,
           const QString &text = QString(),
           const int &w = 0,
           const int &h = 0,
           const int &rotate = 0,
           const int &text_size = 0,
           const bool &flag = false);

    void buildFont();
    bool existedFileToSave();
    bool newLayout(const bool &flag = false);
    void openFile(QString filePath = QString());
    void jsonToLabel(const QJsonObject &object);
    bool saveFile();
    void closeFile();
    void clearChangedLabelsList(const bool &flag = false);

//Operation Undo and Redo
protected:
    void undoRecent();
    void cancelUndo();
private:
    list<vector<MovableLabel*>>::iterator __currentOP;

private:
    //菜单栏
    QMenuBar* _M_MenuBar;
    QMenu* _M_File;         //文件
    QMenu* _M_Edit;         //编辑
    QMenu* _M_AddLabel;     //添加
    QMenu* _M_Text;         //文字
    QMenu* _M_Widget;       //界面

    QAction* _M_NewLayout;      //新建
    QAction* _M_OpenFile;       //打开
    QAction* _M_SaveFile;       //保存
    QAction* _M_CloseFile;      //关闭

    QAction* _M_Undo;           //撤销
    QAction* _M_Redo;           //重做
    QAction* _M_ClearAllLabel;  //清空

    QAction* _M_NewBracket;  //括号
    QAction* _M_NewHLine;   //横线
    QAction* _M_AddText;    //添加文字

    QAction* _M_ResizeWidget;   //设置画布大小

    //横向滚动条
    QScrollBar* _M_HSclBar;

    //竖向滚动条
    QScrollBar* _M_VSclBar;

    //背景画布
    Painting* bgnd_widget;

    //画布遮罩
    QLabel* shade_bgnd_widget;

    //Label
    vector<MovableLabel*> labels;
    list<vector<MovableLabel*>> recent_changed_labels;

    //控件菜单
    QMenu* label_menu;

    //菜单编辑项
    QAction* label_menu_font_edit;

    //菜单移除项
    QAction* label_menu_remove;

    //文字输入
    QLineEdit* font_input;

    //无打开 提示
    QLabel* _M_NoFileIsOpen;

    //打开文件路径
    QString filePath;

private:
    bool _M_HasLabelMoving = false;
    MovableLabel* chosedLabel;
    bool selectingFontPos = false;
    bool selectedFontPos = false;
    bool canceledSelectFontPos = false;
    bool _M_is_copy_label = false;
};

#endif // MAINWINDOW_H
