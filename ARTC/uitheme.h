#ifndef UITHEME_H
#define UITHEME_H

#include <QLabel>
#include <QString>

class QWidget;

/**
 * @brief A word-wrapped label that reports an honest minimum height.
 *
 * A plain wrapped QLabel reports roughly one line as its minimum size, so a
 * fixed-width card laid out around it collapses to that height and clips the
 * remaining lines. Reporting heightForWidth() at the card's content width
 * makes the surrounding layout reserve the right space, and re-reports it
 * whenever the text changes.
 */
class WrappingLabel : public QLabel
{
    Q_OBJECT

public:
    /**
     * @param text initial text
     * @param contentWidth width the text will be laid out at, inside margins
     * @param parent owning widget
     */
    WrappingLabel(const QString &text, int contentWidth, QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    /** Pin the minimum height to what the wrapped text actually needs. */
    void updateWrapHeight();

    int contentWidth;
};

/**
 * @brief Shared look-and-feel helpers used by every screen.
 *
 * The palette and widget rules live in styles/artc.qss; this namespace holds
 * the parts a stylesheet cannot express, plus the small builders that keep the
 * screens visually consistent.
 */
namespace UiTheme {

/**
 * @brief Read the application stylesheet out of the resource bundle.
 * @return the stylesheet, or an empty string if the resource is missing
 */
QString styleSheet();

/**
 * @brief Apply the soft drop shadow that lifts a card off the canvas.
 * @param card widget to shadow; takes ownership of the effect
 */
void applyCardShadow(QWidget *card);

/**
 * @brief Build the circular monogram badge shown on the home screen.
 * @param text short monogram, e.g. "ARTC"
 * @param parent owning widget
 */
QLabel *createMonogram(const QString &text, QWidget *parent = nullptr);

/**
 * @brief Build a one-pixel horizontal rule.
 * @param parent owning widget
 */
QWidget *createDivider(QWidget *parent = nullptr);

/**
 * @brief Stack a caption above an editor to form a labelled field.
 * @param labelText caption shown above the editor
 * @param editor the input widget; becomes a child of the returned group
 * @param parent owning widget
 * @return a widget holding the caption and the editor
 */
QWidget *createField(const QString &labelText, QWidget *editor, QWidget *parent = nullptr);

/**
 * @brief Repaint a widget after its style-affecting properties changed.
 *
 * Qt does not re-evaluate stylesheet property selectors such as
 * QLineEdit[invalid="true"] on its own.
 *
 * @param widget widget whose style should be recomputed
 */
void refreshStyle(QWidget *widget);

} // namespace UiTheme

#endif // UITHEME_H
