#include "leaf.h"


QRectF Leaf::boundingRect() const
{

    return QRectF(0, 0, 100, 50);
}

void Leaf::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rect = boundingRect();
    QRectF rect2(10, 10, 100, 50);
    painter->drawRoundedRect(rect, 6, 5);
     painter->drawText(rect2, "Hello");
}

