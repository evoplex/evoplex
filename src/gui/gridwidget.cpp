/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QBrush>
#include <QPaintEvent>
#include <QPainter>
#include <QtMath>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QSpacerItem>

#include "gridwidget.h"
#include "ui_graphwidget.h"
#include "titlebar.h"

namespace evoplex {

GridWidget::GridWidget(ExperimentsMgr* expMgr, Experiment* exp, QWidget* parent)
    : QDockWidget(parent)
    , m_ui(new Ui_GraphWidget)
    , m_settingsDlg(new Ui_GraphSettings)
    , m_exp(exp)
    , m_model(nullptr)
    , m_agentCMap(ColorMap::DivergingSet1, exp->modelPlugin()->agentAttrSpace())
    , m_zoomLevel(0)
    , m_pixelSizeRate(10.f)
    , m_pixelSize(m_pixelSizeRate)
    , m_selectedAgent(-1)
    , m_origin(5,25)
    , m_posEntered(0,0)
{
    setWindowTitle("Grid");
    setAttribute(Qt::WA_DeleteOnClose, true);

    QWidget* front = new QWidget;
    m_ui->setupUi(front);
    m_ui->bShowAgents->hide();
    m_ui->bShowEdges->hide();
    setWidget(front);

    TitleBar* titleBar = new TitleBar(exp, this);
    setTitleBarWidget(titleBar);
    connect(titleBar, SIGNAL(trialSelected(int)), SLOT(setTrial(int)));
    setTrial(0);
    connect(expMgr, &ExperimentsMgr::statusChanged,
        [this](Experiment* exp) { if (exp == m_exp) setTrial(m_currTrialId); });

    QDialog* dlg = new QDialog(this);
    m_settingsDlg->setupUi(dlg);
    connect(titleBar, SIGNAL(openSettingsDlg()), dlg, SLOT(show()));

    for (QString name : m_exp->modelPlugin()->agentAttrNames()) {
        m_settingsDlg->agentAttr->addItem(name);

        QLineEdit* le = new QLineEdit();
        le->setReadOnly(true);
        m_attrs.emplace_back(le);
        m_ui->inspectorLayout->addRow(name, le);
    }
    for (QString name : m_exp->modelPlugin()->edgeAttrNames()) {
        m_settingsDlg->edgeAttr->addItem(name);
    }
    connect(m_settingsDlg->agentAttr, SIGNAL(currentIndexChanged(int)), SLOT(setAgentAttr(int)));
    setAgentAttr(0);

    connect(m_ui->bZoomIn, SIGNAL(clicked(bool)), SLOT(zoomIn()));
    connect(m_ui->bZoomOut, SIGNAL(clicked(bool)), SLOT(zoomOut()));
    connect(m_ui->bReset, SIGNAL(clicked(bool)), SLOT(resetView()));

    m_ui->inspector->hide();
    connect(m_ui->bCloseInspector, SIGNAL(clicked(bool)), m_ui->inspector, SLOT(hide()));

    // update buffer when resize is done
    m_resizeTimer.setSingleShot(true);
    connect(&m_resizeTimer, &QTimer::timeout, [this](){ updateCache(); });

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(239,235,231));
    setAutoFillBackground(true);
    setPalette(pal);

    updateCache();
}

GridWidget::~GridWidget()
{
    delete m_ui;
    delete m_settingsDlg;
}

void GridWidget::setAgentAttr(int idx)
{
    m_agentCMap.setAttr(m_settingsDlg->agentAttr->currentText());
    m_agentAttr = idx;
}

void GridWidget::setTrial(int trialId)
{
    m_currTrialId = trialId;
    m_model = m_exp->trial(trialId);
    if (m_model) {
        m_ui->currStep->setText(QString::number(m_model->currStep()));
    } else {
        m_ui->currStep->setText("--");
    }
    updateCache();
}

void GridWidget::zoomIn()
{
    ++m_zoomLevel;
    m_pixelSize = m_pixelSizeRate * std::pow(1.25f, m_zoomLevel);
    updateCache();
}

void GridWidget::zoomOut()
{
    --m_zoomLevel;
    m_pixelSize = m_pixelSizeRate * std::pow(1.25f, m_zoomLevel);
    updateCache();
}

void GridWidget::resetView()
{
    m_origin = QPoint(5,25);
    m_zoomLevel = 0;
    m_pixelSize = m_pixelSizeRate;
    m_selectedAgent = -1;
    m_ui->inspector->hide();
    updateCache();
}

void GridWidget::updateCache()
{
    m_cache.clear();
    m_cache.shrink_to_fit();

    if (!m_model)
        return;

    const Agents agents = m_model->graph()->agents();
    m_cache.reserve(agents.size());

    for (Agent* agent : agents) {
        QRectF r(m_origin.x() + agent->x() * m_pixelSize,
                 m_origin.y() + agent->y() * m_pixelSize,
                 m_pixelSize, m_pixelSize);

        if (!rect().contains(r.x(), r.y()))
            continue;

        Cache c;
        c.agent = agent;
        c.rect = r;
        m_cache.emplace_back(c);
    }
    m_cache.shrink_to_fit();
    update();
}

void GridWidget::paintEvent(QPaintEvent* e)
{
    if (!m_model)
        return;

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (Cache cache : m_cache) {
        QColor color;
        if (m_selectedAgent == cache.agent->id()) {
            color = QColor(10,10,10,100);
            updateInspector(cache.agent);
        } else {
            const Value& value = cache.agent->attr(m_agentAttr);
            color = m_agentCMap.colorFromValue(value);
        }
        painter.setBrush(color);
        painter.setPen(color);
        painter.drawRect(cache.rect);
    }

    m_ui->currStep->setText(QString::number(m_model->currStep()));

    painter.end();
}

void GridWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        m_posEntered = e->pos();
}

void GridWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (!m_model || e->button() != Qt::LeftButton
            || (m_ui->inspector->isVisible()
                && m_ui->inspector->geometry().contains(e->pos()))) {
        return;
    }

    m_selectedAgent = -1;
    if (e->pos() != m_posEntered) {
        m_origin += (e->pos() - m_posEntered);
        m_ui->inspector->hide();
        updateCache();
        return;
    }

    for (Cache cache : m_cache) {
        if (cache.rect.contains(e->pos())) {
            m_selectedAgent = cache.agent->id();
            updateInspector(cache.agent);
            m_ui->inspector->show();
            break;
        }
    }

    if (m_selectedAgent == -1) {
        m_ui->inspector->hide();
    }

    update();
}

void GridWidget::resizeEvent(QResizeEvent* e)
{
    m_cache.clear();
    m_resizeTimer.start(500);
    QWidget::resizeEvent(e);
}

void GridWidget::updateInspector(const Agent* agent)
{
    m_ui->agentId->setText(QString::number(agent->id()));
    m_ui->neighbors->setText(QString::number(agent->edges().size()));
    for (int id = 0; id < agent->attrs().size(); ++id) {
        m_attrs.at(id)->setText(agent->attr(id).toQString());
    }
}

}
