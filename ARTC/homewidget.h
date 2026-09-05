#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>

/**
 * @brief Landing screen — the first thing shown at startup.
 *
 * Presents the application identity and routes the user to either the sign-in
 * or the registration screen. It holds no state and touches no database.
 */
class HomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);

signals:
    /** @brief The user chose to sign in to an existing account. */
    void signInRequested();

    /** @brief The user chose to create a new account. */
    void registerRequested();
};

#endif // HOMEWIDGET_H
