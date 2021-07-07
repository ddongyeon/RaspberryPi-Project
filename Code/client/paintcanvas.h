#ifndef PAINTCANVAS_H
#define PAINTCANVAS_H

#include <QWidget>

class PaintCanvas : public QWidget
{
    Q_OBJECT
public:
    enum ToolType {Pen,Rect, Ellipse, Eraser};

    explicit PaintCanvas(QWidget *parent = nullptr);

    ToolType getTool() const;
    void setTool(const ToolType &value);

    int getPenWidth() const;
    void setPenWidth(int value);


    QColor getPenColor() const;
    void setPenColor(const QColor &value);

signals:

public slots:

public:
    void drawLineTo(const QPoint &endPoint);
    void eraseUnder(const QPoint & topLeft);
    void eraseCanvas();

    void resizeImage(QImage *image, const QSize &newSize);
    void Receive_Data();



    ToolType tool;
    bool fill;
    bool drawing;
    int penWidth;
    QColor fillColor;
    QColor penColor;
    QPoint lastPoint;
    QRectF lastRect;
    QRectF lastEraserRect;
    QImage image;


    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

};

#endif // PAINTCANVAS_H
