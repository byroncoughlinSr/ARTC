#include "homewidget.h"
#include "uitheme.h"

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
constexpr int CardWidth = 440;
constexpr int CardMargin = 40;
}

HomeWidget::HomeWidget(QWidget *parent) :
    QWidget(parent)
{
    setObjectName(QStringLiteral("screen"));
    setAttribute(Qt::WA_StyledBackground, true);

    auto *card = new QFrame(this);
    card->setObjectName(QStringLiteral("card"));
    card->setFixedWidth(CardWidth);
    UiTheme::applyCardShadow(card);

    auto *monogram = UiTheme::createMonogram(QStringLiteral("ARTC"), card);

    auto *title = new QLabel(tr("ARTC"), card);
    title->setObjectName(QStringLiteral("appTitle"));
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(tr("Ancestral Relation Tree Creation"), card);
    subtitle->setObjectName(QStringLiteral("subtitle"));
    subtitle->setAlignment(Qt::AlignCenter);

    auto *blurb = new WrappingLabel(tr("Build a pedigree chart from your DNA relatives and "
                                       "trace how each match connects to you."),
                                    CardWidth - 2 * CardMargin, card);
    blurb->setObjectName(QStringLiteral("bodyText"));
    blurb->setAlignment(Qt::AlignCenter);

    auto *signInButton = new QPushButton(tr("Sign In"), card);
    signInButton->setObjectName(QStringLiteral("primaryButton"));
    signInButton->setCursor(Qt::PointingHandCursor);
    signInButton->setDefault(true);

    auto *registerButton = new QPushButton(tr("Create Account"), card);
    registerButton->setObjectName(QStringLiteral("secondaryButton"));
    registerButton->setCursor(Qt::PointingHandCursor);

    auto *footer = new QLabel(tr("Pre-release prototype"), card);
    footer->setObjectName(QStringLiteral("footerText"));
    footer->setAlignment(Qt::AlignCenter);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(CardMargin, 44, CardMargin, 32);
    cardLayout->setSpacing(0);
    cardLayout->addWidget(monogram, 0, Qt::AlignHCenter);
    cardLayout->addSpacing(22);
    cardLayout->addWidget(title);
    cardLayout->addSpacing(4);
    cardLayout->addWidget(subtitle);
    cardLayout->addSpacing(18);
    cardLayout->addWidget(blurb);
    cardLayout->addSpacing(26);
    cardLayout->addWidget(UiTheme::createDivider(card));
    cardLayout->addSpacing(26);
    cardLayout->addWidget(signInButton);
    cardLayout->addSpacing(12);
    cardLayout->addWidget(registerButton);
    cardLayout->addSpacing(24);
    cardLayout->addWidget(footer);

    auto *screenLayout = new QVBoxLayout(this);
    screenLayout->setContentsMargins(24, 24, 24, 24);
    screenLayout->addStretch(1);
    screenLayout->addWidget(card, 0, Qt::AlignHCenter);
    screenLayout->addStretch(1);

    connect(signInButton, &QPushButton::clicked, this, &HomeWidget::signInRequested);
    connect(registerButton, &QPushButton::clicked, this, &HomeWidget::registerRequested);
}
