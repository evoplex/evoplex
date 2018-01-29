/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "graphwidget.h"
#include "ui_graphwidget.h"
#include "titlebar.h"

namespace evoplex
{

GraphWidget::GraphWidget(MainGUI* mainGUI, Experiment* exp, QWidget* parent)
    : QDockWidget(parent)
    , m_ui(new Ui_GraphWidget)
    , m_settingsDlg(new GraphSettings(mainGUI, exp, this))
    , m_exp(exp)
    , m_model(nullptr)
    , m_currStep(-1)
    , m_selectedAgent(-1)
    , m_zoomLevel(0)
    , m_nodeSizeRate(10.f)
    , m_nodeRadius(m_nodeSizeRate)
    , m_origin(5,25)
    , m_posEntered(0,0)
{
    setAttribute(Qt::WA_DeleteOnClose, true);

    QWidget* front = new QWidget;
    m_ui->setupUi(front);
    setWidget(front);

    TitleBar* titleBar = new TitleBar(exp, this);
    setTitleBarWidget(titleBar);
    connect(titleBar, SIGNAL(openSettingsDlg()), m_settingsDlg, SLOT(show()));
    connect(titleBar, SIGNAL(trialSelected(int)), SLOT(setTrial(int)));
    connect(mainGUI->mainApp()->expMgr(), &ExperimentsMgr::trialCreated,
        [this](Experiment* exp, int trialId) {
            if (exp == m_exp && trialId == m_currTrialId)
                setTrial(m_currTrialId);
    });

    connect(m_settingsDlg, &GraphSettings::agentAttrUpdated, [this](int idx) { m_agentAttr = idx; });
    connect(m_settingsDlg, SIGNAL(agentCMapUpdated(ColorMap*)), SLOT(setAgentCMap(ColorMap*)));
    m_agentAttr = m_settingsDlg->agentAttr();
    m_agentCMap = m_settingsDlg->agentCMap();

    connect(m_ui->bZoomIn, SIGNAL(clicked(bool)), SLOT(zoomIn()));
    connect(m_ui->bZoomOut, SIGNAL(clicked(bool)), SLOT(zoomOut()));
    connect(m_ui->bReset, SIGNAL(clicked(bool)), SLOT(resetView()));

    for (QString name : exp->modelPlugin()->agentAttrNames()) {
        QLineEdit* le = new QLineEdit();
        le->setReadOnly(true);
        m_attrs.emplace_back(le);
        m_ui->inspectorLayout->addRow(name, le);
    }
    m_ui->inspector->hide();
    connect(m_ui->bCloseInspector, SIGNAL(clicked(bool)), m_ui->inspector, SLOT(hide()));

    // update buffer when resize is done
    m_resizeTimer.setSingleShot(true);
    connect(&m_resizeTimer, &QTimer::timeout, [this](){ updateCache(); });

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(239,235,231));
    setAutoFillBackground(true);
    setPalette(pal);
}

GraphWidget::~GraphWidget()
{
    delete m_ui;
    delete m_agentCMap;
}

void GraphWidget::setAgentCMap(ColorMap* cmap)
{
    delete m_agentCMap;
    m_agentCMap = cmap;
    update();
}

void GraphWidget::setTrial(int trialId)
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

void GraphWidget::zoomIn()
{
    ++m_zoomLevel;
    m_nodeRadius = m_nodeSizeRate * std::pow(1.25f, m_zoomLevel);
    updateCache();
}

void GraphWidget::zoomOut()
{
    --m_zoomLevel;
    m_nodeRadius = m_nodeSizeRate * std::pow(1.25f, m_zoomLevel);
    updateCache();
}

void GraphWidget::resetView()
{
    m_origin = QPoint(5,25);
    m_zoomLevel = 0;
    m_nodeRadius = m_nodeSizeRate;
    m_selectedAgent = -1;
    m_ui->inspector->hide();
    updateCache();
}

void GraphWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        m_posEntered = e->pos();
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *e)
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

    const Agent* agent = selectAgent(e->pos());
    if (agent) {
        m_selectedAgent = agent->id();
        updateInspector(agent);
        m_ui->inspector->show();
    } else {
        m_ui->inspector->hide();
    }

    update();
}

void GraphWidget::resizeEvent(QResizeEvent* e)
{
    //m_cache.clear();
    m_resizeTimer.start(500);
    QWidget::resizeEvent(e);
}

void GraphWidget::updateInspector(const Agent* agent)
{
    m_ui->agentId->setText(QString::number(agent->id()));
    m_ui->neighbors->setText(QString::number(agent->edges().size()));
    for (int id = 0; id < agent->attrs().size(); ++id) {
        m_attrs.at(id)->setText(agent->attr(id).toQString());
    }
}

void GraphWidget::updateView()
{
    if (!m_model || m_model->currStep() == m_currStep) {
        return;
    }
    m_currStep = m_model->currStep();
    m_ui->currStep->setText(QString::number(m_currStep));
    update();
}

} // evoplex
