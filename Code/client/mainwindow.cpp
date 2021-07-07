#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QSpinBox>
#include <QColorDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QBoxLayout>
#include <stdlib.h>
#include <string>
#include "paintcanvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <QProcess>
int sock_flag, conn_flag;
int flag, ent, xpos, ypos, l_xpos, l_ypos, penwidth, r, g, b, a;
QPoint last, end;
QTextEdit *SystemText;
QString word[] = {"Apple", "Banana", "Computer", "PineApple", "Raspberrypi", "DonggukUniv", "MME", "Elephant", "Iphone", "Samsung", "network"};
QLineEdit *AnswerText;
int idx=0;
QLineEdit *Word;
QPushButton* InputAnswer;
QTextEdit *childText;
QTextEdit *parentText;
PaintCanvas::PaintCanvas(QWidget *parent) : QWidget(parent),
    tool(Pen),
    fill(false),
    drawing(false),
    penWidth(3),
    fillColor(Qt::red),
    penColor(Qt::green),
    lastRect(QRectF(0,0,0,0)),
    lastEraserRect(QRectF(0,0,0,0))
{

}

PaintCanvas::ToolType PaintCanvas::getTool() const
{
    return tool;
}

void PaintCanvas::setTool(const ToolType &value)
{
    tool = value;
}


int PaintCanvas::getPenWidth() const
{
    return penWidth;
}

void PaintCanvas::setPenWidth(int value)
{
    penWidth = value;
}


QColor PaintCanvas::getPenColor() const
{
    return penColor;
}

void PaintCanvas::setPenColor(const QColor &value)
{
    penColor = value;
}

void PaintCanvas::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(&image);
    painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawLine(lastPoint, endPoint);
    int adjustment = penWidth  + 2;
    update(QRect(lastPoint, endPoint).normalized()
           .adjusted(-adjustment, -adjustment, +adjustment, +adjustment));
    lastPoint = endPoint;
}

void PaintCanvas::eraseUnder(const QPoint &topLeft)
{

    QPainter painter(&image);
    //Erase last eraser rect
    painter.setBrush(Qt::white);
    painter.setPen(Qt::white);
    painter.drawRect(lastEraserRect);

    //Erase the content under current eraser rect. Brush and pen still white
    QRect currentRect(topLeft,QSize(100,100));
    painter.setBrush(Qt::white);
    painter.setPen(Qt::white);
    painter.drawRect(currentRect);

    //Draw current eraser rect
    painter.setBrush(Qt::black);
    painter.setPen(Qt::black);
    painter.drawRect(currentRect);

    lastEraserRect = currentRect;

    if(!drawing){
        painter.setBrush(Qt::white);
        painter.setPen(Qt::white);
        painter.drawRect(lastEraserRect);
        lastEraserRect = QRect(0,0,0,0);
    }
    update();
}

void PaintCanvas::eraseCanvas()
{

    QPainter painter(&image);
    //Erase last eraser rect
    painter.setBrush(Qt::white);
    painter.setPen(Qt::white);
    painter.drawRect(lastEraserRect);

    //Erase the content under current eraser rect. Brush and pen still white
    QRect currentRect(QPoint(0,0),QSize(948,571));
    painter.setBrush(Qt::white);
    painter.setPen(Qt::white);
    painter.drawRect(currentRect);

    //Draw current eraser rect
    painter.setBrush(Qt::white);
    painter.setPen(Qt::white);
    painter.drawRect(currentRect);

    lastEraserRect = currentRect;
    update();
}

void PaintCanvas::resizeImage(QImage *image, const QSize &newSize)
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

void PaintCanvas::Receive_Data(){
    pid_t pid = fork();
    char buf[MAX];
    if(pid==0){
      while(1){
          while(1){
            int t= read(conn_flag, buf, sizeof(buf));
            if(t<0) break;
            flag = atoi(strtok(buf, "/"));
            ent = atoi(strtok(NULL, "/"));
            xpos = atoi(strtok(NULL, "/"));
            ypos = atoi(strtok(NULL, "/"));
            l_xpos = atoi(strtok(NULL, "/"));
            l_ypos = atoi(strtok(NULL, "/"));
            penwidth = atoi(strtok(NULL, "/"));
            r = atoi(strtok(NULL, "/"));
            g = atoi(strtok(NULL, "/"));
            b = atoi(strtok(NULL, "/"));
            a = atoi(strtok(NULL, "/"));
            setPenColor(QColor(r,g,b,a));
            setPenWidth(penwidth);
            //press
            if(ent == 0){
                lastPoint.setX(l_xpos);
                lastPoint.setY(l_ypos);
                drawing = true;
            }
            //move
            else if(ent == 1){
                if(flag == 0){
                    drawLineTo(QPoint(xpos,ypos));
                }
                if(flag == 1){
                    eraseUnder(QPoint(xpos,ypos));
                }
            }
            //release
            else if(ent == 2){
                if(flag == 0){
                    drawing = false;
                    drawLineTo(QPoint(xpos,ypos));
                }
                if(flag == 1){
                    drawing = false;
                    eraseUnder(QPoint(xpos,ypos));
                }
                //Reset the last rect
                pid_t pid2 = fork();
                if(pid2==0){
                    qApp->processEvents();
                    exit(0);
                }
                lastRect = QRect(0,0,0,0);
            }
            else if(ent ==100)
            {
                this->eraseCanvas(); //canvas clear
                Word->setText(""); //word clear
                childText->append("Player 1: Correct Answer!!");
                pid_t pid2 = fork();
                if(pid2==0){
                    qApp->processEvents();
                    exit(0);
                }
            }
          }
      }
    }
}

void PaintCanvas::mousePressEvent(QMouseEvent *event)
{
    int flag = 4, ent=0;
    int xpos, ypos;
    xpos = event->pos().x(), ypos = event->pos().y();
    char buf[MAX];
    memset(buf, 0x00, sizeof(buf));
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        drawing = true;
        QColor color = getPenColor();
        int r, g, b, a;
        color.getRgb(&r,&g, &b, &a);
        sprintf(buf,"%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d",flag, ent, xpos, ypos, lastPoint.x(), lastPoint.y(), penWidth,r, g, b, a);
        write(conn_flag, buf, sizeof(buf));
    }
}

void PaintCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && drawing){
        char buf[MAX];
        memset(buf, 0x00, sizeof(buf));
        int xpos, ypos;
        xpos = event->pos().x(), ypos = event->pos().y();
        QColor color = getPenColor();
        int r, g, b, a;
        color.getRgb(&r,&g, &b, &a);
        if(tool == Pen){
            int flag = 0, ent=1;
            sprintf(buf,"%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d",flag, ent, xpos, ypos, lastPoint.x(), lastPoint.y(), penWidth,r, g, b, a);
            write(conn_flag, buf, sizeof(buf));
            drawLineTo(event->pos());
        }
        if(tool == Eraser){
            int flag = 1, ent=1;
            sprintf(buf,"%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d",flag, ent, xpos, ypos, lastPoint.x(), lastPoint.y(), penWidth,r, g, b, a);
            write(conn_flag, buf, sizeof(buf));
            eraseUnder(event->pos());
        }
    }
}

void PaintCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && drawing) {
        drawing = false;
        char buf[MAX];
        memset(buf, 0x00, sizeof(buf));
        int xpos, ypos;
        xpos = event->pos().x(), ypos = event->pos().y();
        QColor color = getPenColor();
        int r, g, b, a;
        color.getRgb(&r,&g, &b, &a);
        if(tool == Pen){
            int flag = 0, ent=2;
            sprintf(buf,"%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d",flag, ent, xpos, ypos, lastPoint.x(), lastPoint.y(), penWidth,r, g, b, a);
            write(conn_flag, buf, sizeof(buf));
            drawLineTo(event->pos());
        }
        if(tool ==Eraser){
            int flag = 1, ent=2;
            sprintf(buf,"%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d",flag, ent, xpos, ypos, lastPoint.x(), lastPoint.y(), penWidth,r, g, b, a);
            write(conn_flag, buf, sizeof(buf));
            eraseUnder(event->pos());
        }
        //Reset the last rect
        lastRect = QRect(0,0,0,0);
    }

}

void PaintCanvas::paintEvent(QPaintEvent *event)
{
   QPainter painter(this);
   QRect rectToDraw = event->rect();
   painter.drawImage(rectToDraw, image, rectToDraw);
}

void PaintCanvas::resizeEvent(QResizeEvent *event)
{
   if (width() > image.width() || height() > image.height()) {
       int newWidth = qMax(width() + 128, image.width());
       int newHeight = qMax(height() + 128, image.height());
       resizeImage(&image, QSize(newWidth, newHeight));
       update();
   }
   QWidget::resizeEvent(event);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //캔버스
    canvas = new PaintCanvas(this);
    setCentralWidget(canvas);

    QLabel *WordLabel=new QLabel("Word",canvas);
    WordLabel->move(10,10);
    WordLabel->setFixedSize(150,40);

    Word=new QLineEdit(WordLabel);
    Word->move(40,10);
    Word->setFixedSize(80,20);
    Word->setReadOnly(true);
    \
    QFont font;
    font.setPointSize(20);
    font.setBold(true);

    AnswerText=new QLineEdit(canvas);
    AnswerText->move(325,500);
    AnswerText->setAlignment(Qt::AlignCenter);
    AnswerText->setFixedSize(300,50);
    AnswerText->setFont(font);

    //텍스트 창
    SystemText=new QTextEdit(this);
    SystemText->setFixedSize(200,80);

    parentText=new QTextEdit(this);
    parentText->setFixedSize(190,80);

    //child textarea
    childText=new QTextEdit(this);
    childText->setFixedSize(190,80);

    //CONNECT 버튼
    QPushButton * ConnectButton = new QPushButton();
    ConnectButton->setText("CONNECT");

    //Pen 굵기
    QLabel * penWidthLabel = new QLabel("Pen Width",this);
    QSpinBox * penWidthSpinBox = new QSpinBox(this);
    penWidthSpinBox->setValue(2);
    penWidthSpinBox->setRange(1,15);

    //Pen 색상
    QLabel * penColorLabel = new QLabel("Pen Color",this);
    penColorButton = new QPushButton(this);

    //Tool 버튼
    QPushButton * penButton = new QPushButton(this);
    penButton->setIcon(QIcon(":/images/pen.png"));

    QPushButton * eraserButton = new QPushButton(this);
    eraserButton->setIcon(QIcon(":/images/eraser.png"));

    //Input answer Button
    InputAnswer=new QPushButton(canvas);
    InputAnswer->setText("INPUT");
    InputAnswer->move(650,500);

    connect(penButton,&QPushButton::clicked,[=](){
            //Set current tool to rect
            canvas->setTool(PaintCanvas::Pen);
            statusBar()->showMessage("Current tool : Pen");

        });

    connect(eraserButton,&QPushButton::clicked,[=](){
            //Set current tool to rect
            canvas->setTool(PaintCanvas::Eraser);
            statusBar()->showMessage("Current tool : Eraser");

        });

    //INPUTANSWER Button
    connect(InputAnswer,SIGNAL(clicked()),this,SLOT(CompareStr()));
    //Connect button
    connect(ConnectButton,SIGNAL(clicked()),this,SLOT(Connect()));
    //Slots connections
    connect(penWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(penWidthChanged(int)));
    connect(penColorButton, SIGNAL(clicked()), this, SLOT(changePenColor()));

    ui->mainToolBar->addWidget(SystemText);
    ui->mainToolBar->addWidget(ConnectButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(penWidthLabel);
    ui->mainToolBar->addWidget(penWidthSpinBox);
    ui->mainToolBar->addWidget(penColorLabel);
    ui->mainToolBar->addWidget(penColorButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(penButton);
    ui->mainToolBar->addWidget(eraserButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(parentText);
    ui->mainToolBar->addWidget(childText);

    QString css = QString("background-color : %1").arg(canvas->getPenColor().name());
    penColorButton->setStyleSheet(css);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::penWidthChanged(int width)
{
    canvas->setPenWidth(width);
}

void MainWindow::changePenColor()
{
    QColor color = QColorDialog::getColor(canvas->getPenColor());
    if(color.isValid()){
        canvas->setPenColor(color);
        QString css = QString("background-color : %1").arg(color.name());
        penColorButton->setStyleSheet(css);
    }

}
void MainWindow::Connect(){
    struct sockaddr_in server_addr;

    // connect server
    if((conn_flag = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        printf("Socket Creation Failure...\n");
        SystemText->append("Socket Creation Failure...");
    }
    else{
        printf("Socket Creation Success...\n");
        SystemText->append("Socket Creation Success...");
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("/*IPAddress*/");
    server_addr.sin_port = htons(PORT);
    if((::connect(conn_flag, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0){
        printf("Fail Server-Client Connection...\n");
        SystemText->append("Fail Server-Client Connection...");
    }
    else{
        printf("Success Server-Client Connection...\n");
        SystemText->append("Success Server-Client Connection...");
        canvas->Receive_Data();
    }

}
void MainWindow::CompareStr(){
    if(word[idx].toUpper().compare(AnswerText->text().toUpper())==0){
        parentText->append("EXCELLENT!!");
        int ent=100;
        char buf[MAX];
        memset(buf,0x00,sizeof(buf));
        sprintf(buf,"%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d",-1, ent, -1, -1, -1, -1, -1,0, 0, 0, 0);
        write(conn_flag, buf, sizeof(buf));
        canvas->eraseCanvas();
        idx+=2;
        Word->setText(word[idx-1]);
        AnswerText->setText("");
    }
    else{
        parentText->append("Try Again. . .");
    }

}
