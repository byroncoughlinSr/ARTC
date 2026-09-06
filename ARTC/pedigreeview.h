#ifndef PEDIGREEVIEW_H
#define PEDIGREEVIEW_H

#include <QWidget>

class FamilyTree;
class QGraphicsScene;
class QGraphicsView;
class QLabel;
class QSpinBox;
struct TreeNode;

/**
 * @brief Draws a host's pedigree chart.
 *
 * The chart uses the notation the design document specifies: a square is male,
 * a circle female. Filled shapes are people; hollow ones are the slots the
 * generator created, still waiting to be matched to a relative from a 23andMe
 * export.
 *
 * The host sits at the left and each generation is a column to its right, so
 * the chart grows rightwards and downwards rather than off both edges.
 */
class PedigreeView : public QWidget
{
    Q_OBJECT

public:
    explicit PedigreeView(QWidget *parent = nullptr);

    /**
     * @brief Load and draw the pedigree rooted at @p hostId.
     * @param[out] error message suitable for showing to the user
     * @return true when the chart was drawn
     */
    bool showHost(int hostId, QString *error = nullptr);

    /** @brief Redraw from the database, keeping the current view settings. */
    void reload();

signals:
    /** @brief The user asked to go back to the workspace. */
    void backRequested();

    /** @brief The user clicked the card for the slot with this id. */
    void slotActivated(int slotId);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void zoomIn();
    void zoomOut();
    void fitToView();

private:
    /** @brief Lay the tree out and populate the scene. */
    void draw();

    /**
     * @brief Place @p node and its ancestors, returning the node's y centre.
     *
     * Ancestors are laid out first so a node can sit midway between its father
     * and mother; a slot with no parents takes the next free row instead.
     */
    qreal place(TreeNode *node, qreal &nextRow);

    /** @brief Generations to draw; deeper slots are treated as leaves. */
    int generationLimit() const;

    /** @brief Add the shape, label and connecting lines for one node. */
    void drawNode(TreeNode *node, qreal x, qreal y);

    /**
     * @brief Put the host on screen at readable magnification.
     *
     * Fitting all eight generations shrinks a node to a few pixels — the
     * deepest row alone is 128 slots tall — so the chart opens centred on the
     * host at 1:1 and the Fit button is there when the whole shape is wanted.
     */
    void showHostArea();

    FamilyTree *tree;
    QPointF hostPos;
    bool viewPositioned = false;
    QGraphicsScene *scene;
    QGraphicsView *view;
    QLabel *heading;
    QLabel *summary;
    QSpinBox *generationSpin;
};

#endif // PEDIGREEVIEW_H
