/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *  Copyright (C) 2018 - Ethan Padden <e.padden1@nuigalway.ie>
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

#include <QtTest>
#include <QStringList>

#include <node.h>

namespace evoplex {
class TestNode: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void tst_Node();

private:
    BaseNode::constructor_key key;
    void _tst_attrs(BaseNode* node, Attributes attrs);
    void _tst_xy(BaseNode* node, int x, int y);
    void _tst_clone(const BaseNode* node);
    void _tst_degree(BaseNode* node);
};

void TestNode::_tst_attrs(BaseNode* node, Attributes attrs)
{
    QCOMPARE(node->attrs().names(), attrs.names());
    QCOMPARE(node->attrs().values(), attrs.values());

    // Tests if 'BaseNode::setAttr()' works as expected.
    const Value newValue = "another-type";
    node->setAttr(0, newValue);

    QCOMPARE(node->attrs().name(0), attrs.name(0));
    QCOMPARE(node->attr(attrs.name(0)), newValue);
    QCOMPARE(node->attrs().value(0), newValue);
    QCOMPARE(node->attr(0), newValue);
    QCOMPARE(node->attrs().size(), attrs.size());
}

void TestNode::_tst_xy(BaseNode* node, int x, int y)
{
    QCOMPARE(node->x(), x);
    QCOMPARE(node->y(), y);

    // Tests if 'BaseNode::setX()' works as expected
    x = 2;
    node->setX(x);
    QCOMPARE(node->x(), x);

    // Tests if 'BaseNode::setY()' works as expected
    y = 3;
    node->setY(y);
    QCOMPARE(node->y(), y);

    // Tests if 'BaseNode::setYCoords()' works as expected
    x = 3;
    y = 4;
    node->setCoords(x,y);
    QCOMPARE(node->x(), x);
    QCOMPARE(node->y(), y);
}

void TestNode::_tst_clone(const BaseNode* node) {
    // Tests if 'BaseNode::clone()' works as expected.
    NodePtr c = node->clone();
    QCOMPARE(c->id(), node->id());
    QCOMPARE(c->x(), node->x());
    QCOMPARE(c->y(), node->y());
    QCOMPARE(c->attrs().size(), node->attrs().size());
    QCOMPARE(c->attrs().names(), node->attrs().names());
    QCOMPARE(c->attrs().values(), node->attrs().values());
    QCOMPARE(c->degree(), node->degree());
    QCOMPARE(c->inDegree(), node->inDegree());
    QCOMPARE(c->outDegree(), node->outDegree());
}

void TestNode::_tst_degree(BaseNode* node)
{
    // Tests if degree functions return 0 with an empty node
    QCOMPARE(node->degree(), 0);
    QCOMPARE(node->inDegree(), 0);
    QCOMPARE(node->outDegree(), 0);
}

void TestNode::tst_Node()
{
    int id = 0, x = 1, y = 2;
    const QStringList names = { "test0", "test1", "test2" };
    const Value values[] = { Value(123), Value(234), Value(456) };
    Attributes attrs(names.size());
    for(int i = 0; i < names.size(); ++i){
        attrs.replace(i, names[i], values[i]);
    }

    auto tests = [this, id, x, y, attrs](BaseNode* node) {
        QCOMPARE(node->id(), id);
        _tst_attrs(node, attrs);
        _tst_xy(node, x, y);
        _tst_clone(node);
        _tst_degree(node);
    };

    std::unique_ptr<UNode> unode(new UNode(key, id, attrs, x, y));
    tests(unode.get());

    std::unique_ptr<DNode> dnode(new DNode(key, id, attrs, x, y));
    tests(dnode.get());
}

} // evoplex
QTEST_MAIN(evoplex::TestNode)
#include "tst_node.moc"
