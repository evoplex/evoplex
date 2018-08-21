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

#include "core/trial.h"

#include "basegraphgl.h"
#include "ui_basegraphgl.h"

namespace evoplex {

BaseGraphGL::BaseGraphGL(ExperimentPtr exp, GraphWidget* parent)
    : QOpenGLWidget(parent),
      m_ui(new Ui_BaseGraphGL),
      m_graphWidget(parent),
      m_exp(exp),
      m_trial(nullptr),
      m_currStep(-1),
      m_nodeAttr(-1),
      m_nodeCMap(nullptr),
      m_background(QColor(239,235,231)),
      m_zoomLevel(0),
      m_nodeScale(10.),
      m_nodeRadius(m_nodeScale),
      m_origin(5.,5.),
      m_cacheStatus(CacheStatus::Ready),
      m_posEntered(0.,0.),
      m_currTrialId(0)
{
    m_ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    Q_ASSERT_X(!m_exp->autoDeleteTrials(), "BaseGraphGL",
               "tried to build a BaseGraphGL for a experiment that will be auto-deleted!");

    connect(m_exp.get(), SIGNAL(restarted()), SLOT(slotRestarted()));

    // setTrial() triggers a timer that needs to be exec in the main thread
    // thus, we need to use queuedconnection here
    connect(exp.get(), &Experiment::trialCreated, this,
            [this](int trialId) { if (trialId == m_currTrialId) setTrial(m_currTrialId); },
            Qt::QueuedConnection);

    connect(exp.get(), &Experiment::statusChanged, [this](Status s) {
        if (s == Status::Invalid) return;
        for (AttrWidget* aw : m_attrWidgets) {
            if (aw) aw->setReadOnly(s == Status::Running);
        }
    });

    connect(m_ui->bZoomIn, SIGNAL(clicked(bool)), SLOT(zoomIn()));
    connect(m_ui->bZoomOut, SIGNAL(clicked(bool)), SLOT(zoomOut()));
    connect(m_ui->bReset, SIGNAL(clicked(bool)), SLOT(resetView()));

    connect(m_ui->bCloseInspector, &QPushButton::clicked,
            [this](bool) { clearSelection(); });
    setupInspector();

    m_updateCacheTimer.setSingleShot(true);
    connect(&m_updateCacheTimer, &QTimer::timeout, [this]() { updateCache(true); });
}

BaseGraphGL::~BaseGraphGL()
{
    m_attrWidgets.clear();
    m_trial = nullptr;
    m_exp = nullptr;
    delete m_ui;
}

void BaseGraphGL::setupInspector()
{
    QLayoutItem* item;
    while (m_ui->modelAttrs->count() &&
           (item = m_ui->modelAttrs->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    m_attrWidgets.clear();
    m_attrWidgets.resize(static_cast<size_t>(m_exp->modelPlugin()->nodeAttrsScope().size()));

    for (auto attrRange : m_exp->modelPlugin()->nodeAttrsScope()) {
        AttrWidget* aw = new AttrWidget(attrRange, this);
        aw->setToolTip(attrRange->attrRangeStr());
        connect(aw, &AttrWidget::valueChanged, [this, aw]() { attrChanged(aw); });
        m_attrWidgets.at(attrRange->id()) = aw;
        m_ui->modelAttrs->insertRow(attrRange->id(), attrRange->attrName(), aw);

        QWidget* l = m_ui->modelAttrs->labelForField(aw);
        l->setToolTip(attrRange->attrName());
        l->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
        l->setMinimumWidth(m_ui->lNodeId->minimumWidth());
    }

    m_ui->inspector->hide();
}

void BaseGraphGL::attrChanged(AttrWidget* aw) const
{
    if (!m_trial || !m_trial->graph() || m_ui->nodeId->value() < 0) {
        return;
    }

    if (m_trial->status() == Status::Running) {
        Node node = m_trial->graph()->node(m_ui->nodeId->value());
        aw->blockSignals(true);
        aw->setValue(node.attr(aw->id()));
        aw->blockSignals(false);
        QMessageBox::warning(parentWidget(), "Graph",
            "You cannot change things in a running experiment.\n"
            "Please, pause it and try again.");
    }

    Node node = m_trial->graph()->node(m_ui->nodeId->value());
    Value v = aw->validate();
    if (v.isValid()) {
        node.setAttr(aw->id(), v);
        // let the other widgets aware that they all need to be updated
        emit (updateWidgets(true));
    } else {
        aw->blockSignals(true);
        aw->setValue(node.attr(aw->id()));
        aw->blockSignals(false);
        QString err = "The input for '" + aw->attrName() +
                "' is invalid.\nExpected: " + aw->attrRangeStr();
        QMessageBox::warning(parentWidget(), "Graph", err);
    }
}

void BaseGraphGL::updateCache(bool force)
{
    if (!force) {
        m_updateCacheTimer.start(10);
        return;
    }

    if (m_cacheStatus == CacheStatus::Updating) {
        return;
    }

    m_mutex.lock();
    m_cacheStatus = CacheStatus::Updating;
    QFuture<CacheStatus> future = QtConcurrent::run(this, &BaseGraphGL::refreshCache);
    QFutureWatcher<CacheStatus>* watcher = new QFutureWatcher<CacheStatus>;
    connect(watcher, &QFutureWatcher<int>::finished, [this, watcher]() {
        m_cacheStatus = static_cast<CacheStatus>(watcher->result());
        watcher->deleteLater();
        if (m_cacheStatus == CacheStatus::Scheduled) {
            m_updateCacheTimer.start(10);
        }
        update();
    });
    watcher->setFuture(future);
    m_mutex.unlock();
}

void BaseGraphGL::slotRestarted()
{
    if (m_exp->autoDeleteTrials()) {
        close();
        return;
    }
    clearSelection();
    setupInspector();
    m_trial = nullptr;
    m_ui->currStep->setText("--");
    updateCache(true);
}

void BaseGraphGL::setNodeCMap(ColorMap* cmap)
{
    m_nodeCMap = cmap;
    m_nodeAttr = cmap ? cmap->attrRange()->id() : -1;
    update();
}

void BaseGraphGL::setTrial(quint16 trialId)
{
    m_currTrialId = trialId;
    m_trial = m_exp->trial(trialId);
    if (m_trial && m_trial->model()) {
        m_ui->currStep->setText(QString::number(m_trial->step()));
    } else {
        m_ui->currStep->setText("--");
    }
    updateCache();
}

void BaseGraphGL::setNodeScale(int v)
{
    m_nodeScale = v;
    m_nodeRadius = m_nodeScale * std::pow(1.25f, m_zoomLevel);
    update();
}

void BaseGraphGL::zoomIn()
{
    ++m_zoomLevel;
    m_nodeRadius = m_nodeScale * std::pow(1.25f, m_zoomLevel);
    updateCache();
    clearSelection();
}

void BaseGraphGL::zoomOut()
{
    --m_zoomLevel;
    m_nodeRadius = m_nodeScale * std::pow(1.25f, m_zoomLevel);
    updateCache();
    clearSelection();
}

void BaseGraphGL::resetView()
{
    m_origin = QPointF(5., 5.);
    m_zoomLevel = 0;
    m_nodeRadius = m_nodeScale;
    updateCache();
    clearSelection();
}

void BaseGraphGL::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        m_posEntered = e->localPos();
    }
}

void BaseGraphGL::mouseReleaseEvent(QMouseEvent *e)
{
    if (!m_trial || !m_trial->model() || e->button() != Qt::LeftButton ||
            (m_ui->inspector->isVisible() && m_inspGeo.contains(e->pos()))) {
        return;
    }

    if (e->localPos() == m_posEntered) {
        Node prevSelection = selectedNode();
        const Node& node = selectNode(e->pos());
        if (node.isNull() || prevSelection == node) {
            clearSelection();
        } else {
            updateInspector(node);
            m_ui->inspector->show();
            update();
        }
    } else {
        m_origin += (e->localPos() - m_posEntered);
        clearSelection();
        updateCache();
    }
}

void BaseGraphGL::resizeEvent(QResizeEvent* e)
{
    QOpenGLWidget::resizeEvent(e);
    m_inspGeo = QRect();
    updateCache();
}

void BaseGraphGL::updateInspector(const Node& node)
{
    m_ui->nodeId->setValue(node.id());
    m_ui->neighbors->setText(QString::number(node.outDegree()));
    for (auto aw : m_attrWidgets) {
        aw->blockSignals(true);
        aw->setValue(node.attr(aw->id()));
        aw->blockSignals(false);
    }

    m_ui->inspector->show();
    m_ui->inspector->adjustSize();
    m_inspGeo = m_ui->inspector->frameGeometry();
    m_inspGeo += QMargins(5,5,5,5);
    m_ui->inspector->hide();
}

void BaseGraphGL::updateView(bool forceUpdate)
{
    if (!m_trial || !m_trial->model() || (!forceUpdate && m_trial->step() == m_currStep)) {
        return;
    }
    m_currStep = m_trial->step();
    m_ui->currStep->setText(QString::number(m_currStep));
    update();
}

void BaseGraphGL::clearSelection()
{
    m_ui->inspector->hide();
    update();
}

} // evoplex
