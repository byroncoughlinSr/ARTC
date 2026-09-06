#include "pedigreeview.h"
#include "familytree.h"
#include "uitheme.h"

#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace {

// Chart geometry. RowHeight is what one leaf slot occupies; everything else
// is centred against it, so the deepest generation sets the overall height.
constexpr qreal ColumnWidth = 132;
constexpr qreal RowHeight = 26;
constexpr qreal NodeSize = 16;
constexpr qreal ScenePadding = 40;

const QColor Ink(0x16, 0x23, 0x3A);
const QColor Accent(0x1E, 0x3A, 0x5F);
const QColor Host(0xE8, 0xB4, 0x4A);
const QColor Empty(0xA9, 0xB7, 0xC9);
const QColor Line(0xC3, 0xCD, 0xDA);

} // namespace

PedigreeView::PedigreeView(QWidget *parent) :
    QWidget(parent),
    tree(new FamilyTree)
{
    setObjectName(QStringLiteral("screen"));
    setAttribute(Qt::WA_StyledBackground, true);

    heading = new QLabel(tr("Family tree"), this);
    heading->setObjectName(QStringLiteral("screenTitle"));

    summary = new QLabel(this);
    summary->setObjectName(QStringLiteral("bodyText"));

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    view->setObjectName(QStringLiteral("chart"));
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    auto *backButton = new QPushButton(tr("Back"), this);
    backButton->setObjectName(QStringLiteral("secondaryButton"));
    backButton->setCursor(Qt::PointingHandCursor);

    auto *fitButton = new QPushButton(tr("Fit"), this);
    fitButton->setObjectName(QStringLiteral("secondaryButton"));
    fitButton->setCursor(Qt::PointingHandCursor);

    auto *outButton = new QPushButton(tr("−"), this);
    outButton->setObjectName(QStringLiteral("secondaryButton"));
    outButton->setCursor(Qt::PointingHandCursor);

    auto *inButton = new QPushButton(tr("+"), this);
    inButton->setObjectName(QStringLiteral("secondaryButton"));
    inButton->setCursor(Qt::PointingHandCursor);

    generationSpin = new QSpinBox(this);
    generationSpin->setRange(2, 8);
    generationSpin->setValue(5);
    generationSpin->setToolTip(tr("Generations to show. The full chart is eight deep, "
                                  "which is 128 slots in the last generation alone."));
    auto *generationLabel = new QLabel(tr("Generations"), this);
    generationLabel->setObjectName(QStringLiteral("fieldLabel"));

    auto *legend = new QLabel(tr("□ male    ○ female    ● host    "
                                 "hollow = slot awaiting a DNA match"), this);
    legend->setObjectName(QStringLiteral("footerText"));

    auto *toolbar = new QHBoxLayout;
    toolbar->setSpacing(10);
    toolbar->addWidget(backButton);
    toolbar->addStretch(1);
    toolbar->addWidget(legend);
    toolbar->addSpacing(16);
    toolbar->addWidget(generationLabel);
    toolbar->addWidget(generationSpin);
    toolbar->addSpacing(12);
    toolbar->addWidget(outButton);
    toolbar->addWidget(inButton);
    toolbar->addWidget(fitButton);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(0);
    layout->addWidget(heading);
    layout->addSpacing(4);
    layout->addWidget(summary);
    layout->addSpacing(16);
    layout->addWidget(view, 1);
    layout->addSpacing(12);
    layout->addLayout(toolbar);

    connect(backButton, &QPushButton::clicked, this, &PedigreeView::backRequested);
    connect(fitButton, &QPushButton::clicked, this, &PedigreeView::fitToView);
    connect(inButton, &QPushButton::clicked, this, &PedigreeView::zoomIn);
    connect(outButton, &QPushButton::clicked, this, &PedigreeView::zoomOut);
    connect(generationSpin, &QSpinBox::valueChanged, this, [this] {
        if (tree->root()) {
            draw();
            viewPositioned = false;
            showHostArea();
        }
    });
}

bool PedigreeView::showHost(int hostId, QString *error)
{
    if (!tree->load(hostId, error)) {
        return false;
    }

    draw();
    showHostArea();
    return true;
}

void PedigreeView::draw()
{
    scene->clear();

    TreeNode *root = tree->root();
    if (!root) {
        return;
    }

    heading->setText(tr("Family tree — %1")
                         .arg(root->displayName().isEmpty() ? tr("host") : root->displayName()));

    // Note: "slots" is a Qt macro, so it cannot be used as an identifier here.
    const int slotCount = tree->nodes().size();
    const int emptyCount = tree->placeholderCount();
    summary->setText(tr("%1 generations built, %2 slots, %3 still empty. "
                        "Showing %4 generations — import a 23andMe DNA Relatives export "
                        "to start filling the slots in.")
                         .arg(tree->depth() + 1)
                         .arg(slotCount)
                         .arg(emptyCount)
                         .arg(generationLimit()));

    qreal nextRow = 0;
    const qreal hostY = place(root, nextRow);
    hostPos = QPointF(0, hostY);
    viewPositioned = false;

    scene->setSceneRect(scene->itemsBoundingRect()
                            .adjusted(-ScenePadding, -ScenePadding, ScenePadding, ScenePadding));
}

qreal PedigreeView::place(TreeNode *node, qreal &nextRow)
{
    const qreal x = node->generation * ColumnWidth;

    // A slot with no parents is a leaf: it takes the next free row. Anything
    // else is centred on the rows its parents ended up occupying.
    if ((!node->father && !node->mother) || node->generation + 1 >= generationLimit()) {
        const qreal y = nextRow * RowHeight;
        nextRow += 1;
        drawNode(node, x, y);
        return y;
    }

    QList<QPair<TreeNode *, qreal>> parents;
    if (node->father) {
        parents.append({node->father, place(node->father, nextRow)});
    }
    if (node->mother) {
        parents.append({node->mother, place(node->mother, nextRow)});
    }

    qreal sum = 0;
    for (const auto &parent : parents) {
        sum += parent.second;
    }
    const qreal y = sum / parents.size();

    drawNode(node, x, y);

    // An elbow out to the midpoint, up or down to the parent's row, then across.
    const QPen pen(Line, 1.2);
    const qreal mid = x + ColumnWidth * 0.5;
    for (const auto &parent : parents) {
        const qreal parentX = parent.first->generation * ColumnWidth;
        scene->addLine(x + NodeSize * 0.6, y, mid, y, pen);
        scene->addLine(mid, y, mid, parent.second, pen);
        scene->addLine(mid, parent.second, parentX - NodeSize * 0.6, parent.second, pen);
    }

    return y;
}

int PedigreeView::generationLimit() const
{
    return generationSpin->value();
}

void PedigreeView::drawNode(TreeNode *node, qreal x, qreal y)
{
    const bool placeholder = node->isPlaceholder();
    const bool isHost = node->generation == 0;
    const qreal half = NodeSize / 2;

    QPen pen(placeholder ? Empty : Accent, placeholder ? 1.2 : 1.6);
    QBrush brush(isHost ? Host : (placeholder ? QBrush(Qt::NoBrush) : QBrush(Accent)));

    QGraphicsItem *shape = nullptr;
    if (node->sex == QLatin1Char('F')) {
        shape = scene->addEllipse(x - half, y - half, NodeSize, NodeSize, pen, brush);
    } else {
        shape = scene->addRect(x - half, y - half, NodeSize, NodeSize, pen, brush);
    }

    QString tip = placeholder
                      ? tr("Empty slot %1 — generation %2")
                            .arg(node->firstName).arg(node->generation)
                      : tr("%1 — generation %2").arg(node->displayName()).arg(node->generation);
    shape->setToolTip(tip);

    // Only real people are labelled: the chart should read as an empty
    // scaffold, not as a wall of generated slot codes.
    if (!placeholder) {
        auto *label = scene->addSimpleText(node->displayName());
        label->setBrush(Ink);
        QFont font = label->font();
        font.setPointSizeF(8.5);
        font.setBold(isHost);
        label->setFont(font);
        // Above the node rather than beside it: the elbow out to the parents
        // runs along y and would strike the text through.
        label->setPos(x - half, y - half - label->boundingRect().height() - 3);
    }
}

void PedigreeView::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (!viewPositioned) {
        showHostArea();
    }
}

void PedigreeView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!viewPositioned) {
        showHostArea();
    }
}

void PedigreeView::showHostArea()
{
    // Wait for a viewport that has really been laid out: centring against a
    // provisional size latches the wrong scroll position, and the retry only
    // happens while viewPositioned is still false.
    if (!tree->root() || !view->isVisible() || view->viewport()->width() <= 50
        || view->viewport()->height() <= 50) {
        return;
    }

    view->resetTransform();
    view->centerOn(hostPos);
    viewPositioned = true;
}

void PedigreeView::zoomIn()
{
    view->scale(1.25, 1.25);
}

void PedigreeView::zoomOut()
{
    view->scale(1 / 1.25, 1 / 1.25);
}

void PedigreeView::fitToView()
{
    if (scene->sceneRect().isValid()) {
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }
}
