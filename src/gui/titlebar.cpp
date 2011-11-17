/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QMessageBox>
#include <QPainter>

#include "titlebar.h"
#include "ui_titlebar.h"

namespace evoplex {

TitleBar::TitleBar(Experiment* exp, QDockWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui_TitleBar)
    , m_exp(exp)
{
    m_ui->setupUi(this);

    setStyleSheet("background-color:rgb(40,40,40);");

    QStyle* style = qApp->style();
    m_ui->bClose->setIcon(style->standardIcon(QStyle::SP_TitleBarCloseButton));
    m_ui->bFloat->setIcon(style->standardIcon(QStyle::SP_TitleBarNormalButton));
    connect(m_ui->bClose, SIGNAL(clicked(bool)), parent, SLOT(close()));
    connect(m_ui->bFloat, &QPushButton::clicked,
            [parent]() { parent->setFloating(!parent->isFloating()); });

    for (int trialId = 0; trialId < exp->numTrials(); ++trialId) {
        m_ui->cbTrial->insertItem(trialId, QString::number(trialId));
    }

    connect(m_ui->cbTrial, SIGNAL(currentIndexChanged(int)), SLOT(slotTrialChanged(int)));

    connect(m_ui->bSettings, &QPushButton::clicked,
            [this]() { if (isFreeToUse()) emit (openSettingsDlg()); });
}

TitleBar::~TitleBar()
{
    delete m_ui;
}

void TitleBar::paintEvent(QPaintEvent* pe)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(pe);
}

bool TitleBar::isFreeToUse()
{
    if (m_exp->expStatus() == Experiment::READY) {
        return true;
    }

    QString msg;
    if (m_exp->expStatus() == Experiment::FINISHED) {
        msg = "This experiment has finished. Changes cannot be made at this stage.";
    } else if (m_exp->expStatus() == Experiment::INVALID) {
        msg = "This experiment is not valid.";
    } else {
        msg = "You should pause the experiment first.";
    }
    QMessageBox::warning(this, "Experiment", msg);
    return false;
}

void TitleBar::slotTrialChanged(int trialId)
{
    if (isFreeToUse()) {
        emit (trialSelected(trialId));
    }
}

void TitleBar::changeCurrentTrial(int trialId)
{
    m_ui->cbTrial->blockSignals(true);
    m_ui->cbTrial->setCurrentIndex(trialId);
    m_ui->cbTrial->blockSignals(false);
}

}
