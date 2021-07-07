#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "paintcanvas.h"
#include <QPushButton>
#include <QCheckBox>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <QTextEdit>
#include "paintcanvas.h"
#define MAX 256
#define PORT 8606
#define PENDING 10
#define CLIENT_SIZE 5

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    friend void PaintCanvas::Receive_Data();

private slots:
    void penWidthChanged(int width);
    void changePenColor();
    void Connect();
    void CompareStr();
private:
    Ui::MainWindow *ui;
    PaintCanvas * canvas;
    QPushButton * penColorButton;
    QPushButton * fillColorButton;
    QCheckBox * fillCheckBox;
};

#endif // MAINWINDOW_H
