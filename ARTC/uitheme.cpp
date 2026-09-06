#include "uitheme.h"

#include <QColor>
#include <QFile>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

WrappingLabel::WrappingLabel(const QString &text, int width, QWidget *parent) :
    QLabel(text, parent),
    contentWidth(width)
{
    setWordWrap(true);
}

QSize WrappingLabel::minimumSizeHint() const
{
    QSize hint = QLabel::minimumSizeHint();
    const int width = contentWidth > 0 ? contentWidth : QLabel::width();
    if (width > 0) {
        hint.setHeight(qMax(hint.height(), heightForWidth(width)));
    }
    return hint;
}

void WrappingLabel::showEvent(QShowEvent *event)
{
    QLabel::showEvent(event);
    updateWrapHeight();
}

void WrappingLabel::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);
    updateWrapHeight();
}

void WrappingLabel::updateWrapHeight()
{
    // A corrected minimumSizeHint() alone is not enough: a QBoxLayout that has
    // already sized this widget will not revisit it. Setting the minimum
    // height outright forces the relayout.
    const int width = contentWidth > 0 ? contentWidth : QLabel::width();
    if (width <= 0) {
        return;
    }

    const int needed = heightForWidth(width);
    if (needed > 0 && minimumHeight() != needed) {
        setMinimumHeight(needed);
    }
}

namespace UiTheme {

QString styleSheet()
{
    QFile file(QStringLiteral(":/styles/artc.qss"));
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

void applyCardShadow(QWidget *card)
{
    auto *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(30);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(15, 23, 42, 38));
    card->setGraphicsEffect(shadow);
}

QLabel *createMonogram(const QString &text, QWidget *parent)
{
    auto *monogram = new QLabel(text, parent);
    monogram->setObjectName(QStringLiteral("monogram"));
    monogram->setFixedSize(76, 76);
    monogram->setAlignment(Qt::AlignCenter);
    return monogram;
}

QWidget *createDivider(QWidget *parent)
{
    auto *divider = new QFrame(parent);
    divider->setObjectName(QStringLiteral("divider"));
    divider->setFrameShape(QFrame::NoFrame);
    return divider;
}

QWidget *createField(const QString &labelText, QWidget *editor, QWidget *parent)
{
    auto *group = new QWidget(parent);

    auto *label = new QLabel(labelText, group);
    label->setObjectName(QStringLiteral("fieldLabel"));
    label->setBuddy(editor);

    auto *layout = new QVBoxLayout(group);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->addWidget(label);
    layout->addWidget(editor);

    return group;
}

void refreshStyle(QWidget *widget)
{
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
}

} // namespace UiTheme
