#ifndef LEAF_H
#define LEAF_H

#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>

class Leaf : public QGraphicsItem
{
public:
   QRectF boundingRect() const;
   void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
   qreal x;
   qreal y;
};

#endif // LEAF_H
