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
#include <QPainterPath>
#include <QPen>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace {

// Card geometry. A column holds one card plus the gutter its connectors run
// through; RowHeight is what one leaf card occupies.
constexpr qreal CardWidth = 196;
constexpr qreal CardHeight = 54;
constexpr qreal ColumnWidth = 268;
constexpr qreal RowHeight = 64;
constexpr qreal CornerRadius = 6;
constexpr qreal AccentWidth = 5;
constexpr qreal ScenePadding = 26;

const QColor Ink(0x16, 0x23, 0x3A);
const QColor Muted(0x76, 0x84, 0x96);
const QColor Line(0xC3, 0xCD, 0xDA);
const QColor CardFill(0xFF, 0xFF, 0xFF);
const QColor CardEdge(0xD8, 0xDF, 0xE8);

const QColor MaleAccent(0x3E, 0x7C, 0xB1);
const QColor MaleTint(0xEA, 0xF2, 0xF9);
const QColor FemaleAccent(0xB5, 0x62, 0x8E);
const QColor FemaleTint(0xF9, 0xEA, 0xF2);
const QColor HostAccent(0xD3, 0x9A, 0x2E);
const QColor HostTint(0xFD, 0xF6, 0xE7);
const QColor EmptyEdge(0xC7, 0xD1, 0xDD);

/** 2 -> "2nd", 3 -> "3rd", 4 -> "4th". */
QString ordinal(int n)
{
    if (n % 100 >= 11 && n % 100 <= 13) {
        return QStringLiteral("%1th").arg(n);
    }
    switch (n % 10) {
    case 1:  return QStringLiteral("%1st").arg(n);
    case 2:  return QStringLiteral("%1nd").arg(n);
    case 3:  return QStringLiteral("%1rd").arg(n);
    default: return QStringLiteral("%1th").arg(n);
    }
}

/**
 * @brief Name a slot by its position, the way a pedigree chart is read.
 *
 * Generation 3 is the great-grandparents; every generation past that adds one
 * "great", numbered as the requirements document lists them.
 */
QString relationship(const TreeNode *node)
{
    const bool male = node->sex != QLatin1Char('F');
    switch (node->generation) {
    case 0: return QObject::tr("Host");
    case 1: return male ? QObject::tr("Father") : QObject::tr("Mother");
    case 2: return male ? QObject::tr("Grandfather") : QObject::tr("Grandmother");
    case 3: return male ? QObject::tr("Great-grandfather")
                        : QObject::tr("Great-grandmother");
    default:
        return male ? QObject::tr("%1 great-grandfather").arg(ordinal(node->generation - 2))
                    : QObject::tr("%1 great-grandmother").arg(ordinal(node->generation - 2));
    }
}

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
    generationSpin->setValue(4);
    generationSpin->setToolTip(tr("Generations to show. The full chart is eight deep, "
                                  "which is 128 slots in the last generation alone."));
    auto *generationLabel = new QLabel(tr("Generations"), this);
    generationLabel->setObjectName(QStringLiteral("fieldLabel"));

    auto *legend = new QLabel(tr("□ male    ○ female    dashed = no match yet"), this);
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
    hostPos = QPointF(CardWidth / 2, hostY);
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

    // Out of this card's right edge, along the gutter, then into the parent's
    // left edge — the orthogonal elbow a pedigree chart is drawn with.
    const QPen pen(Line, 1.3);
    const qreal gutter = x + CardWidth + (ColumnWidth - CardWidth) * 0.5;
    for (const auto &parent : parents) {
        const qreal parentX = parent.first->generation * ColumnWidth;
        scene->addLine(x + CardWidth, y, gutter, y, pen);
        scene->addLine(gutter, y, gutter, parent.second, pen);
        scene->addLine(gutter, parent.second, parentX, parent.second, pen);
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
    const bool female = node->sex == QLatin1Char('F');

    const QColor accent = isHost ? HostAccent : (female ? FemaleAccent : MaleAccent);
    const QColor tint = isHost ? HostTint : (female ? FemaleTint : MaleTint);
    const QRectF card(x, y - CardHeight / 2, CardWidth, CardHeight);

    // An empty slot is drawn as a dashed outline on the plain card colour, so
    // the chart reads at a glance as a scaffold with a few people in it.
    QPen edge(placeholder ? EmptyEdge : CardEdge, placeholder ? 1.1 : 1.3);
    if (placeholder) {
        edge.setStyle(Qt::DashLine);
        edge.setDashPattern({4, 3});
    }

    QPainterPath path;
    path.addRoundedRect(card, CornerRadius, CornerRadius);
    QGraphicsPathItem *body = scene->addPath(path, edge,
                                             placeholder ? QBrush(CardFill) : QBrush(tint));

    // The accent stripe down the left edge carries the sex at a distance.
    QPainterPath stripe;
    stripe.addRoundedRect(QRectF(card.left(), card.top(), AccentWidth * 2, CardHeight),
                          CornerRadius, CornerRadius);
    stripe.addRect(QRectF(card.left() + AccentWidth, card.top(), AccentWidth, CardHeight));
    QColor stripeColour = accent;
    if (placeholder && !isHost) {
        stripeColour.setAlpha(90);
    }
    scene->addPath(stripe.simplified(), QPen(Qt::NoPen), QBrush(stripeColour));

    // Square for male, circle for female — the design document's notation,
    // kept as the card's avatar.
    const qreal glyph = 15;
    const qreal glyphX = card.left() + AccentWidth * 2 + 11;
    const qreal glyphY = y - glyph / 2;
    const QPen glyphPen(placeholder ? QColor(accent.red(), accent.green(), accent.blue(), 130)
                                    : accent, 1.4);
    const QBrush glyphFill = placeholder ? QBrush(Qt::NoBrush) : QBrush(accent.lighter(155));
    if (female) {
        scene->addEllipse(glyphX, glyphY, glyph, glyph, glyphPen, glyphFill);
    } else {
        scene->addRect(glyphX, glyphY, glyph, glyph, glyphPen, glyphFill);
    }

    const qreal textX = glyphX + glyph + 10;
    const qreal textWidth = card.right() - textX - 8;

    // Line one: the person's name, or the relationship for an empty slot.
    auto *primary = scene->addSimpleText(placeholder ? relationship(node)
                                                     : node->displayName());
    QFont primaryFont = primary->font();
    primaryFont.setPointSizeF(9.5);
    primaryFont.setBold(!placeholder);
    primary->setFont(primaryFont);
    primary->setBrush(placeholder ? Muted : Ink);
    primary->setPos(textX, y - 15);

    // Line two: dates for a person, the awaited-match note for a slot.
    QString secondary;
    if (placeholder) {
        secondary = QObject::tr("No match yet");
    } else if (node->birthdate.isValid() && node->birthdate.year() > 1) {
        secondary = QObject::tr("b. %1").arg(node->birthdate.toString(QStringLiteral("yyyy")));
    }
    if (!secondary.isEmpty()) {
        auto *sub = scene->addSimpleText(secondary);
        QFont subFont = sub->font();
        subFont.setPointSizeF(8);
        sub->setFont(subFont);
        sub->setBrush(Muted);
        sub->setPos(textX, y + 1);
    }

    Q_UNUSED(textWidth)
    body->setToolTip(placeholder
                         ? QObject::tr("%1 — empty slot %2 (generation %3)")
                               .arg(relationship(node), node->firstName)
                               .arg(node->generation)
                         : QObject::tr("%1 — %2 (generation %3)")
                               .arg(node->displayName(), relationship(node))
                               .arg(node->generation));
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
    // Horizontally on the middle of the chart so the deepest generation is not
    // cut off, but vertically on the host, which is what the eye looks for.
    view->centerOn(scene->sceneRect().center().x(), hostPos.y());
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
