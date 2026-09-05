#ifndef UITHEME_H
#define UITHEME_H

#include <QString>

class QLabel;
class QWidget;

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
