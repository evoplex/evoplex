/**
 * This file is part of Evoplex.
 *
 * Evoplex is a multi-agent system for networks.
 * Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtConcurrent>
#include <QFutureWatcher>
#include <QMessageBox>

#include "basegraphgl.h"
#include "ui_basegraphgl.h"

namespace evoplex {

// (cardinot) TODO: nodeAttrsScope should not be here
BaseGraphGL::BaseGraphGL(QWidget* parent)
    : QOpenGLWidget(parent),
      m_ui(new Ui_BaseGraphGL),
      m_abstractGraph(nullptr),
      m_isReadOnly(false),
      m_currStep(-1),
      m_nodeAttr(-1),
      m_nodeCMap(nullptr),
      m_background(QColor(239,235,231)),
      m_zoomLevel(0.f),
      m_nodeScale(10.),
      m_nodeRadius(m_nodeScale),
      m_origin(m_nodeScale, m_nodeScale),
      m_cacheStatus(CacheStatus::Ready),
      m_posEntered(0,0),
      m_curMode(SelectionMode::Select),
      m_fullInspectorVisible(false),
      m_nodeAttrs(Attributes(0)),
      m_edgeAttrs(Attributes(0))
{
    m_ui->setupUi(this);

    // Qt uses this attribute to optimize paint events on resizes (see docs)
    setAttribute(Qt::WA_StaticContents, true);

    connect(m_ui->bZoomIn, SIGNAL(pressed()), SLOT(zoomIn()));
    connect(m_ui->bZoomOut, SIGNAL(pressed()), SLOT(zoomOut()));
    connect(m_ui->bReset, SIGNAL(pressed()), SLOT(resetView()));
    connect(m_ui->deleteEdge, SIGNAL(clicked()), this, SLOT(removeEdgeEvent()));
    
    connect(this, SIGNAL(nodesMoved()), SLOT(slotUpdateSelection()));

    m_bCenter = new QtMaterialIconButton(QIcon(":/icons/material/center_white_18"), this);
    m_bCenter->setToolTip("centralize selection");
    m_bCenter->setCheckable(true);
    m_bCenter->setChecked(false);
    m_bCenter->setColor(Qt::white);
    connect(m_bCenter, &QtMaterialIconButton::toggled, [this](bool checked) {
        if (!selectedNode().isNull() && checked) {
            selectNode(selectedNodePos(), true);
            updateCache();
        }
        m_bCenter->setColor(checked ? palette().color(QPalette::Link) : Qt::white);
    });
    m_ui->topLayout->addWidget(m_bCenter);

    m_bRefresh = new QtMaterialIconButton(QIcon(":/icons/material/refresh_white_18"), this);
    m_bRefresh->setToolTip("refresh");
    m_bRefresh->setColor(Qt::white);
    connect(m_bRefresh, &QtMaterialIconButton::pressed, [this]() {
        updateInspector(selectedNode());
    });
    m_ui->topLayout->addWidget(m_bRefresh);

    setupInspector();
    m_ui->currStep->hide();

    m_updateCacheTimer.setSingleShot(true);
    connect(&m_updateCacheTimer, &QTimer::timeout, [this]() { updateCache(true); });
}

BaseGraphGL::~BaseGraphGL()
{
    m_nodeAttrWidgets.clear();
    delete m_ui;
}

void BaseGraphGL::setup(AbstractGraph* abstractGraph, AttributesScope nodeAttrsScope, AttributesScope edgeAttrsScope)
{
    m_abstractGraph = abstractGraph;
    m_nodeAttrsScope = nodeAttrsScope;
    m_edgeAttrsScope = edgeAttrsScope;
    setupInspector();
    updateCache();
    m_nodeAttrs.resize(m_nodeAttrsScope.size());
    m_edgeAttrs.resize(m_edgeAttrsScope.size());

    int n = 0;
    for(auto i = m_nodeAttrsScope.begin(); i != m_nodeAttrsScope.end(); ++i) {
        m_nodeAttrs.replace(n, i.value().get()->attrName(), i.value().get()->min());
        ++n;
    }

    n = 0;
    for(auto i = m_edgeAttrsScope.begin(); i != m_edgeAttrsScope.end(); ++i) {
        m_edgeAttrs.replace(n, i.value().get()->attrName(), i.value().get()->min());
        ++n;
    }
}

void BaseGraphGL::paint(QPaintDevice* device, bool paintBackground) const
{
    QPainter painter;
    painter.begin(device);
    painter.setRenderHint(QPainter::Antialiasing);
    if (paintBackground) {
        painter.fillRect(rect(), m_background);
    }

    painter.translate(m_origin);
    if (m_cacheStatus == CacheStatus::Ready) {
        paintFrame(painter);
    }
    painter.end();
}

void BaseGraphGL::slotFullInspectorVisible(int visible)
{
    m_fullInspectorVisible = visible;

    clearSelection();
    updateCache();
}

void BaseGraphGL::slotSelectNode(int nodeid)
{
    try {
        Node node = m_abstractGraph->node(nodeid);
        selectNode(node, m_bCenter->isChecked());
    } catch (std::out_of_range) {
        if (selectedNode().isNull()) {
            clearSelection();
        }
    }
}

void BaseGraphGL::slotDeleteSelectedNodes()
{
    for (auto node : m_selectedNodes) {
        m_abstractGraph->removeNode(node.second);
    }

    clearSelection();
    updateCache();
}

void BaseGraphGL::slotDeleteSelectedEdges()
{
    for (auto edge : m_selectedEdges) {
        const Edge e = m_abstractGraph->edge(edge.second.id());
        m_abstractGraph->removeEdge(e);
    }

    clearSelection();
    updateCache();
}

void BaseGraphGL::createNode(const QPointF& pos)
{
    QPointF p = nodePoint(pos - m_origin);
    m_abstractGraph->addNode(m_nodeAttrs, p.x(), p.y());
    updateCache();
}

void BaseGraphGL::createEdge(const Node& orig, const Node& neigh) {
    Attributes* edgeAttrs = &m_edgeAttrs;
    m_abstractGraph->addEdge(orig.id(), neigh.id(), edgeAttrs);
    updateCache();
}

void BaseGraphGL::deleteNode(const QPointF pos)
{
    const Node& node = findNode(pos);
    clearSelection();
    m_abstractGraph->removeNode(node);
    updateCache();
}

void BaseGraphGL::moveSelectedNodes(const Node& node, const QPointF pos) {
    QPointF v = nodePoint(pos - m_origin) - QPointF(node.x(), node.y());
    for (Node _node : m_selectedNodes) {
        _node.setCoords(_node.x() + v.x(), _node.y() + v.y());
    }
    emit(nodesMoved());
    updateCache();
}

void BaseGraphGL::moveNode(Node& node, const QPointF pos) {
    QPointF p = nodePoint(pos - m_origin);
    node.setCoords(p.x(), p.y());
    emit(nodesMoved());
    updateCache();
}

void BaseGraphGL::setupInspector()
{
    // important! for some reason, changing the layout (add/delete itens)
    // in a invisible UI disables all fields in an irreversible way.
    // Tested on Qt 5.9--5.11
    m_ui->inspector->setCurrentIndex(0);
    m_ui->inspector->show();

    while (m_ui->modelAttrs->count()) {
        auto item = m_ui->modelAttrs->takeRow(0);
        delete item.labelItem->widget();
        delete item.labelItem;
        delete item.fieldItem;
    }

    m_ui->inspector->hide();

    m_nodeAttrWidgets.clear();
    m_nodeAttrWidgets.resize(static_cast<size_t>(m_nodeAttrsScope.size()));

    for (auto attrRange : m_nodeAttrsScope) {
        auto aw = std::make_shared<AttrWidget>(attrRange, nullptr);
        aw->setToolTip(attrRange->attrRangeStr());
        int aId = aw->id();
        connect(aw.get(), &AttrWidget::valueChanged, [this, aId]() { attrValueChanged(aId); });
        m_nodeAttrWidgets.at(attrRange->id()) = aw;
        m_ui->modelAttrs->insertRow(attrRange->id(), attrRange->attrName(), aw.get());
        QWidget* l = m_ui->modelAttrs->labelForField(aw.get());
        l->setToolTip(attrRange->attrName());
        l->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
        l->setMinimumWidth(m_ui->lNodeId->minimumWidth());
    }

}

void BaseGraphGL::attrValueChanged(int attrId) const
{
    if (!m_abstractGraph || m_selectedNodes.empty()) {
        return;
    }

    std::shared_ptr<AttrWidget> aw;

    try { 
        aw = m_nodeAttrWidgets.at(attrId);
    } catch (std::out_of_range) { return; }

    for (auto node : m_selectedNodes) {
        if (m_isReadOnly) {
            aw->blockSignals(true);
            aw->setValue(node.second.attr(aw->id()));
            aw->blockSignals(false);
            QMessageBox::warning(parentWidget(), "Graph",
                "You cannot change things in a running experiment.\n"
                "Please, pause it and try again.");
            continue;
        }

        Value v;
        v = aw->validate();
        

        if (v.isValid()) {
            node.second.setAttr(aw->id(), v);
        }
        else {
            aw->blockSignals(true);
            aw->setValue(node.second.attr(aw->id()));
            aw->blockSignals(false);
            QString err = "The input for '" + aw->attrName() +
                "' is invalid.\nExpected: " + aw->attrRangeStr();
            QMessageBox::warning(parentWidget(), "Graph", err);
        }
        // let the other widgets aware that they all need to be updated
        emit(updateWidgets(true));
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

void BaseGraphGL::slotStatusChanged(Status s)
{
    m_isReadOnly = s == Status::Running;
    for (auto aw : m_nodeAttrWidgets) {
        if (aw) {
            aw->setReadOnly(m_isReadOnly);
        }
    }
}

void BaseGraphGL::slotRestarted()
{
    clearSelection();
    setCurrentStep(-1); // TODO
    setup(nullptr, AttributesScope(), AttributesScope());
}

void BaseGraphGL::edgesListItemClicked(QListWidgetItem* item)
{
    const Edge e = m_abstractGraph->edge(item->text().toInt());
    updateEdgeInspector(e);
}

void BaseGraphGL::removeEdgeEvent()
{
    int edgeId = (m_ui->edgeId->text()).toInt();
    const Edge e = m_abstractGraph->edge(edgeId);
    m_abstractGraph->removeEdge(e);
    m_ui->inspector->hide();
    clearSelection();
    updateCache(true);
}

void BaseGraphGL::setNodeCMap(ColorMap* cmap)
{
    m_nodeCMap = cmap;
    m_nodeAttr = cmap ? cmap->attrRange()->id() : -1;
    update();
}

// (cardinot) TODO: showing the current time step only makes sense if the graph is loaded for an experiment.
//                  let's use <0 to hide the counter, but it should be improved later
void BaseGraphGL::setCurrentStep(int step)
{
    m_ui->currStep->show();
    m_ui->currStep->setText(QString::number(step));
    m_currStep = step;
}

void BaseGraphGL::setCurrentSelectionMode(SelectionMode m) {
    m_curMode = m;
    //TODO: Don't clear selection each time the mode is changed
    clearSelection();
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
}

void BaseGraphGL::zoomOut()
{
    --m_zoomLevel;
    m_nodeRadius = m_nodeScale * std::pow(1.25f, m_zoomLevel);
    updateCache();
}

void BaseGraphGL::resetView()
{
    m_origin = QPointF(m_nodeScale, m_nodeScale);
    m_zoomLevel = 0;
    m_nodeRadius = m_nodeScale;
    updateCache();
    clearSelection();
}

void BaseGraphGL::wheelEvent(QWheelEvent* e)
{
    m_zoomLevel += (e->angleDelta().y() / 120.f);
    auto newNodeRadius = m_nodeScale * std::pow(1.25f, m_zoomLevel);

    auto fromOrigin = e->posF() - m_origin;
    auto newPos = fromOrigin * (newNodeRadius / m_nodeRadius);
    m_origin -= (newPos - fromOrigin);
    m_nodeRadius = newNodeRadius;

    updateCache();
    clearSelection();
    e->accept();
}

void BaseGraphGL::mousePressEvent(QMouseEvent* e)
{
    m_posEntered = e->pos();
    QOpenGLWidget::mousePressEvent(e);
}

void BaseGraphGL::mouseReleaseEvent(QMouseEvent *e)
{
    QOpenGLWidget::mouseReleaseEvent(e);

    if (m_ui->inspector->isVisible() && m_inspGeo.contains(e->pos())) {
        auto p = m_ui->bCloseInspector->mapFrom(this, e->pos());
        if (m_ui->bCloseInspector->rect().contains(p)) {
            clearSelection();
        }
        return;
    }

    if (!m_abstractGraph) {
        return;
    }

    const Node _prevNode = findNode(m_posEntered);
    Node node = findNode(e->localPos());
    if (e->button() == Qt::LeftButton) {
        bool fNodeSelected;
        Node prevSelection = selectedNode();

        if (!node.isNull() && inSelectedNodes(node)) {
            fNodeSelected = true;
            if (e->modifiers().testFlag(Qt::ControlModifier)) {
                deselectNode(node);
                emit(nodeDeselected(node));
                refreshCache();
            }
        } else {
            fNodeSelected = false;
        }

        if (e->pos() == m_posEntered) {
            if (!e->modifiers().testFlag(Qt::ControlModifier)) {
                if (m_curMode == SelectionMode::NodeEdit && node.isNull()) {
                    if (m_selectedNodes.empty()) {
                        createNode(e->localPos());
                    } else {
                        clearSelection();
                    }
                } else if (!node.isNull() && m_curMode == SelectionMode::EdgeEdit) {
                    if (!inSelectedNodes(node)) {
                        selectNode(e->localPos(), m_bCenter->isChecked());
                        m_selectedNodes.insert(std::make_pair(node.id(), node));

                        for (Edge edge : node.outEdges()) {
                            if (!inSelectedEdges(edge) && inSelectedNodes(edge.neighbour())) {
                                m_selectedEdges.insert(std::make_pair(edge.id(), edge));
                                selectEdge(edge);
                                emit(edgeSelected(edge));
                            }
                        }
                    } else {
                        for (Edge edge : node.outEdges()) {
                            deselectEdge(edge);
                        }
                        deselectNode(node);
                    }
                } else {
                    clearSelection();
                }
            }
            if (!node.isNull() && (!fNodeSelected || !e->modifiers().testFlag(Qt::ControlModifier)) && m_curMode != SelectionMode::EdgeEdit) {
                selectNode(e->localPos(), m_bCenter->isChecked());
                m_selectedNodes.insert(std::make_pair(node.id(), node));               
                updateInspector(node);
                emit(nodeSelected(node));
                refreshCache();
            }
            m_bCenter->isChecked() ? updateCache() : update();
        } else {
            if (m_curMode == SelectionMode::NodeEdit && !_prevNode.isNull()) {
                Node movedNode;
                if (inSelectedNodes(_prevNode)) {
                    moveSelectedNodes(_prevNode, e->localPos());
                } else {
                    clearSelection();
                    movedNode = selectNode(m_posEntered, false);
                    moveNode(movedNode, e->localPos());
                    m_selectedNodes.insert(std::make_pair(movedNode.id(), movedNode));
                    updateInspector(movedNode);
                    emit(nodeSelected(movedNode));
                }
            } else if (m_curMode == SelectionMode::EdgeEdit && !node.isNull() && !_prevNode.isNull()) {
                createEdge(_prevNode, node);
            } else {
                m_origin += (e->pos() - m_posEntered);
                updateCache();
            }
        }
    } else if (e->button() == Qt::RightButton && m_curMode == SelectionMode::NodeEdit && !node.isNull()) {
        deleteNode(e->localPos());
    } else if (e->button() == Qt::RightButton && m_nodeAttr >= 0 && !m_isReadOnly) {
        Node node = selectNode(e->localPos(), false);
        if (!node.isNull()) {
            const QString& attrName = node.attrs().name(m_nodeAttr);
            auto attrRange = m_nodeAttrsScope.value(attrName);
            node.setAttr(m_nodeAttr, attrRange->next(node.attr(m_nodeAttr)));
            clearSelection();
            emit(updateWidgets(true));
            updateCache();
        }
    }
}

void BaseGraphGL::keyPressEvent(QKeyEvent* e)
{
    if (e->modifiers().testFlag(Qt::ControlModifier)) {
        if (e->key() == Qt::Key_0) {
            resetView();
        } else if (e->key() == Qt::Key_Plus || e->key() == Qt::Key_Equal) {
            zoomIn();
        } else if (e->key() == Qt::Key_Minus || e->key() == Qt::Key_Underscore) {
            zoomOut();
        }
        QOpenGLWidget::keyPressEvent(e);
        return;
    }

    qreal increment = e->isAutoRepeat() ? 5.0 : 1.0;
    if (e->modifiers().testFlag(Qt::ShiftModifier)) {
        increment *= 5.0;
    }

    if (e->key() == Qt::Key_Right) {
        m_origin.rx() += increment;
    } else if (e->key() == Qt::Key_Left) {
        m_origin.rx() -= increment;
    } else if (e->key() == Qt::Key_Up) {
        m_origin.ry() -= increment;
    } else if (e->key() == Qt::Key_Down) {
        m_origin.ry() += increment;
    }
    QOpenGLWidget::keyPressEvent(e);
}

void BaseGraphGL::keyReleaseEvent(QKeyEvent* e)
{
    if (!e->isAutoRepeat()) {
        updateCache();
        if (e->key() == Qt::Key_Space) {
            Node node = selectedNode();
            if (!node.isNull()) {
                const QString& attrName = node.attrs().name(m_nodeAttr);
                auto attrRange = m_nodeAttrsScope.value(attrName);
                node.setAttr(m_nodeAttr, attrRange->next(node.attr(m_nodeAttr)));
                updateInspector(node);
                emit(updateWidgets(true));
            }
        }
    }
    QOpenGLWidget::keyReleaseEvent(e);
}

bool BaseGraphGL::deselectNode(const Node& node) {
    if (inSelectedNodes(node)) {
        m_selectedNodes.erase(node.id());
        return true;
    }
    return false;
}

bool BaseGraphGL::deselectEdge(const Edge& edge) {
    if (m_selectedEdges.find(edge.id()) != m_selectedEdges.end()) {
        emit(edgeDeselected(edge));
        m_selectedEdges.erase(edge.id());
        return true;
    }
    return false;
}

void BaseGraphGL::resizeEvent(QResizeEvent* e)
{
    QOpenGLWidget::resizeEvent(e);
    m_inspGeo = QRect();
    updateCache();
}

void BaseGraphGL::updateEdgeInspector(const Edge& edge)
{
    if (m_fullInspectorVisible) {
        return;
    }

    m_ui->inspector->setCurrentIndex(1);

    m_ui->edgeId->setText(QString::number(edge.id()));
    
    m_ui->inspector->show();
    m_ui->inspector->adjustSize();
    m_inspGeo = m_ui->inspector->frameGeometry();
    m_inspGeo += QMargins(5, 5, 5, 5);   
}

void BaseGraphGL::updateNodesInspector(const Node& node)
{
    if (m_fullInspectorVisible) {
        return;
    }

    QString nodes;
    QString neighbors;

    if (m_selectedNodes.size() == 1) {
        nodes = QString::number(node.id());
    } else {
        nodes = m_ui->nodeIds->text() + " " + QString::number(node.id());
    }

    for (auto const& e : node.outEdges()) {
            sneighbors.insert(e.second.neighbour().id());
    }
    
    for (int nbr : sneighbors) {
        neighbors.append(QString::number(nbr) + " ");
    }
    
    m_ui->nodeIds->setText(nodes);
    m_ui->nodesNeighbors->setText(neighbors);

    m_ui->inspector->setCurrentIndex(2);

    m_ui->inspector->show();
    m_ui->inspector->adjustSize();
    
    m_inspGeo = m_ui->inspector->frameGeometry();
    m_inspGeo += QMargins(5, 5, 5, 5);
}

void BaseGraphGL::updateInspector(const Node& node)
{
    m_ui->inspector->setCurrentIndex(0);

    QString nodes;
    QString neighbors;
    QString edges;
    
    if (m_selectedNodes.size() == 1) {
        nodes = QString::number(node.id());
    } else {
        nodes = m_ui->nodeIds->text() + " " + QString::number(node.id());
    }

    for (auto const& e : node.outEdges()) {
        sedges.insert(e.first);
        sneighbors.insert(e.second.neighbour().id());
    }

    for (auto const& e : node.outEdges()) {
        sneighbors.insert(e.second.neighbour().id());
    }

    for (int nbr : sneighbors) {
        neighbors.append(QString::number(nbr) + " ");
    }

    for (int edg : sedges) {
        edges.append(QString::number(edg) + " ");
    }

    m_ui->nodeIds->setText(nodes);
    m_ui->nodesNeighbors->setText(neighbors);
    m_ui->edges->setText(edges);

    for (auto aw : m_nodeAttrWidgets) {
        aw->blockSignals(true);
        aw->setValue(node.attr(aw->id()));
        aw->blockSignals(false);
    }

    if (!m_fullInspectorVisible) {
        m_ui->inspector->show();
        m_ui->inspector->adjustSize();
        m_inspGeo = m_ui->inspector->frameGeometry();
        m_inspGeo += QMargins(5, 5, 5, 5);
    }
}

void BaseGraphGL::clearSelection()
{
    emit(clearedSelected());
    m_selectedNodes.clear();
    m_selectedEdges.clear();

    m_ui->nodeIds->clear();
    sedges.clear();
    sneighbors.clear();
    if (m_ui->inspector->isVisible()) {
        m_ui->inspector->hide();
        update();
    }
}

} // evoplex
