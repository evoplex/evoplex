/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtConcurrent>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QMutex>

#include "graphwidget.h"
#include "ui_graphwidget.h"
#include "titlebar.h"

namespace evoplex
{

GraphWidget::GraphWidget(MainGUI* mainGUI, Experiment* exp, ExperimentWidget* parent)
    : QDockWidget(parent)
    , m_expWidget(parent)
    , m_ui(new Ui_GraphWidget)
    , m_settingsDlg(new GraphSettings(mainGUI, exp, this))
    , m_exp(exp)
    , m_model(nullptr)
    , m_currStep(-1)
    , m_selectedNode(-1)
    , m_zoomLevel(0)
    , m_nodeSizeRate(10.f)
    , m_nodeRadius(m_nodeSizeRate)
    , m_origin(5,25)
    , m_posEntered(0,0)
    , m_cacheStatus(Ready)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setFocusPolicy(Qt::StrongFocus);

    Q_ASSERT_X(!m_exp->autoDeleteTrials(), "GraphWidget",
               "tried to build a GraphWidget for a experiment that will be auto-deleted!");

    connect(m_exp, SIGNAL(restarted()), SLOT(slotRestarted()));

    QWidget* front = new QWidget(this);
    m_ui->setupUi(front);
    front->setFocusPolicy(Qt::StrongFocus);
    setWidget(front);

    TitleBar* titleBar = new TitleBar(exp, this);
    setTitleBarWidget(titleBar);
    connect(titleBar, SIGNAL(openSettingsDlg()), m_settingsDlg, SLOT(show()));
    connect(titleBar, SIGNAL(trialSelected(int)), SLOT(setTrial(int)));

    // setTrial() triggers a timer that needs to be exec in the main thread
    // thus, we need to use queuedconnection here
    connect(exp, &Experiment::trialCreated, this,
            [this](int trialId) { if (trialId == m_currTrialId) setTrial(m_currTrialId); },
            Qt::QueuedConnection);

    connect(m_settingsDlg, &GraphSettings::nodeAttrUpdated, [this](int idx) { m_nodeAttr = idx; });
    connect(m_settingsDlg, SIGNAL(nodeCMapUpdated(ColorMap*)), SLOT(setNodeCMap(ColorMap*)));
    m_nodeAttr = m_settingsDlg->nodeAttr();
    m_nodeCMap = m_settingsDlg->nodeCMap();

    connect(m_ui->bZoomIn, SIGNAL(clicked(bool)), SLOT(zoomIn()));
    connect(m_ui->bZoomOut, SIGNAL(clicked(bool)), SLOT(zoomOut()));
    connect(m_ui->bReset, SIGNAL(clicked(bool)), SLOT(resetView()));

    m_attrs.resize(exp->modelPlugin()->nodeAttrsScope().size());
    for (const AttributeRange* attrRange : exp->modelPlugin()->nodeAttrsScope()) {
        QLineEdit* le = new QLineEdit();
        le->setToolTip(attrRange->attrRangeStr());
        connect(le, &QLineEdit::editingFinished, [this, attrRange, le]() {
            if (!m_model || !m_model->graph() || m_ui->nodeId->value() < 0) {
                return;
            }
            QString err;
            Node* node = m_model->graph()->node(m_ui->nodeId->value());
            if (m_model->status() == Experiment::RUNNING) {
                err = "You cannot change things in a running experiment.\n"
                      "Please, pause it and try again.";
            } else {
                Value v = attrRange->validate(le->text());
                if (v.isValid()) {
                    node->setAttr(attrRange->id(), v);
                    // let the other widgets aware that they all need to be updated
                    emit (m_expWidget->updateWidgets(true));
                    return;
                } else {
                    err = "The input for '" + attrRange->attrName() + "' is invalid.\n"
                          "Expected: " + attrRange->attrRangeStr();
                }
            }
            QMessageBox::warning(this, "Graph", err);
            le->setText(node->attr(attrRange->id()).toQString());
        });
        m_attrs[attrRange->id()] = le;
        m_ui->inspectorLayout->addRow(attrRange->attrName(), le);
    }
    m_ui->inspector->hide();
    connect(m_ui->bCloseInspector, SIGNAL(clicked(bool)), m_ui->inspector, SLOT(hide()));

    m_updateCacheTimer.setSingleShot(true);
    connect(&m_updateCacheTimer, &QTimer::timeout, [this]() { updateCache(true); });

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(239,235,231));
    setAutoFillBackground(true);
    setPalette(pal);
}

GraphWidget::~GraphWidget()
{
    m_model = nullptr;
    m_exp = nullptr;
    delete m_ui;
    delete m_nodeCMap;
}

void GraphWidget::updateCache(bool force)
{
    if (!force) {
        m_updateCacheTimer.start(10);
        return;
    }

    if (m_cacheStatus == Updating) {
        return;
    }

    QMutex mutex;
    mutex.lock();
    m_cacheStatus = Updating;
    QFuture<int> future = QtConcurrent::run(this, &GraphWidget::refreshCache);
    QFutureWatcher<int>* watcher = new QFutureWatcher<int>;
    connect(watcher, &QFutureWatcher<int>::finished, [this, watcher]() {
        m_cacheStatus = (CacheStatus) watcher->result();
        watcher->deleteLater();
        if (m_cacheStatus == Scheduled) {
            m_updateCacheTimer.start(10);
        }
        update();
    });
    watcher->setFuture(future);
    mutex.unlock();
}

void GraphWidget::slotRestarted()
{
    if (m_exp->autoDeleteTrials()) {
        close();
        return;
    }
    m_selectedNode = -1;
    m_ui->inspector->hide();
    m_model = nullptr;
    m_ui->currStep->setText("--");
    updateCache(true);
}

void GraphWidget::setNodeCMap(ColorMap* cmap)
{
    delete m_nodeCMap;
    m_nodeCMap = cmap;
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
    m_selectedNode = -1;
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

    m_selectedNode = -1;
    if (e->pos() == m_posEntered) {
        const Node* node = selectNode(e->pos());
        if (node) {
            m_selectedNode = node->id();
            updateInspector(node);
            m_ui->inspector->show();
        } else {
            m_ui->inspector->hide();
        }
        update();
    } else {
        m_origin += (e->pos() - m_posEntered);
        m_ui->inspector->hide();
        updateCache();
    }
}

void GraphWidget::resizeEvent(QResizeEvent* e)
{
    updateCache();
    QWidget::resizeEvent(e);
}

void GraphWidget::updateInspector(const Node* node)
{
    m_ui->nodeId->setValue(node->id());
    m_ui->neighbors->setText(QString::number(node->edges().size()));
    for (int id = 0; id < node->attrs().size(); ++id) {
        m_attrs.at(id)->setText(node->attr(id).toQString());
    }
}

void GraphWidget::updateView(bool forceUpdate)
{
    if (!m_model || (!forceUpdate && m_model->currStep() == m_currStep)) {
        return;
    }
    m_currStep = m_model->currStep();
    m_ui->currStep->setText(QString::number(m_currStep));
    update();
}

void GraphWidget::clearSelection()
{
    m_selectedNode = -1;
    m_ui->inspector->hide();
    update();
}

} // evoplex
