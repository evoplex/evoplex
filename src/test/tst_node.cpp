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

#include <QtTest>
#include <node.h>

namespace evoplex {
class TestNode: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void tst_UNode();
    void tst_DNode();

private:
    Node::constructor_key key;
};

void TestNode::tst_UNode()
{
    // Tests if constructor and 'Node::attrs()' work as expected.
    int id = 0, x = 1, y = 2;

    const int ATTR_SIZE = 3;
    Value values[ATTR_SIZE] = {Value(123), Value(234), Value(456)};
    const char* names[ATTR_SIZE] = {"test0", "test1", "test2"};

    Attributes attrs(3);
    for(int i = 0; i < ATTR_SIZE; ++i){
        attrs.replace(i, names[i], values[i]);
    }

    UNode node(key, id, attrs, x, y);

    QCOMPARE(node.id(), id);

    QCOMPARE(node.attrs().size(), attrs.size());
    QCOMPARE(node.attrs().names(), attrs.names());
    QCOMPARE(node.attrs().values(), attrs.values());

    for (int i = 0; i < ATTR_SIZE; ++i){
        QCOMPARE(node.attrs().name(i), attrs.name(i));
        QCOMPARE(node.attrs().value(i), attrs.value(i));
        QCOMPARE(node.attr(names[i]), attrs.value(i));
        QCOMPARE(node.attr(i), attrs.value(i));
    }

    QCOMPARE(node.x(), x);
    QCOMPARE(node.y(), y);

    // Tests if 'Node::setAttr()' works as expected.
    node.setAttr(0, Value(567));

    QCOMPARE(node.attrs().name(0), attrs.name(0));
    QCOMPARE(node.attr(names[0]), Value(567));
    QCOMPARE(node.attrs().value(0), Value(567));
    QCOMPARE(node.attr(0), Value(567));
    QCOMPARE(node.attrs().size(), attrs.size());

    // Tests if 'Node::setX()' works as expected
    x = 2;
    node.setX(x);
    QCOMPARE(node.x(), x);

    // Tests if 'Node::setY()' works as expected
    y = 3;
    node.setY(y);
    QCOMPARE(node.y(), y);

    // Tests if 'Node::setYCoords()' works as expected
    x = 3;
    y = 4;
    node.setCoords(x,y);
    QCOMPARE(node.x(), x);
    QCOMPARE(node.y(), y);

    // Tests if degree functions return 0 with an empty node
    QCOMPARE(node.degree(), 0);
    QCOMPARE(node.inDegree(), 0);
    QCOMPARE(node.outDegree(), 0);

    // Tests if 'Node::clone()' works as expected.
    NodePtr c = node.clone();
    QCOMPARE(c->id(), node.id());
    QCOMPARE(c->x(), node.x());
    QCOMPARE(c->y(), node.y());
    QCOMPARE(c->attrs().size(), node.attrs().size());
    QCOMPARE(c->attrs().names(), node.attrs().names());
    QCOMPARE(c->attrs().values(), node.attrs().values());
    QCOMPARE(c->degree(), node.degree());
    QCOMPARE(c->inDegree(), node.inDegree());
    QCOMPARE(c->outDegree(), node.outDegree());
}

void TestNode::tst_DNode()
{
    // Tests if constructor and 'Node::attrs()' work as expected.
    int id = 0, x = 1, y = 2;

    const int ATTR_SIZE = 3;
    Value values[ATTR_SIZE] = {Value(123), Value(234), Value(456)};
    const char* names[ATTR_SIZE] = {"test0", "test1", "test2"};

    Attributes attrs(3);
    for(int i = 0; i < ATTR_SIZE; ++i){
        attrs.replace(i, names[i], values[i]);
    }

    DNode node(key, id, attrs, x, y);

    QCOMPARE(node.id(), id);

    QCOMPARE(node.attrs().size(), attrs.size());
    QCOMPARE(node.attrs().names(), attrs.names());
    QCOMPARE(node.attrs().values(), attrs.values());

    for (int i = 0; i < ATTR_SIZE; ++i){
        QCOMPARE(node.attrs().name(i), attrs.name(i));
        QCOMPARE(node.attrs().value(i), attrs.value(i));
        QCOMPARE(node.attr(names[i]), attrs.value(i));
        QCOMPARE(node.attr(i), attrs.value(i));
    }

    QCOMPARE(node.x(), x);
    QCOMPARE(node.y(), y);

    // Tests if 'Node::setAttr()' works as expected.
    node.setAttr(0, Value(567));

    QCOMPARE(node.attrs().name(0), attrs.name(0));
    QCOMPARE(node.attr(names[0]),Value(567));
    QCOMPARE(node.attrs().value(0), Value(567));
    QCOMPARE(node.attr(0), Value(567));
    QCOMPARE(node.attrs().size(), attrs.size());

    // Tests if 'Node::setX()' works as expected
    x = 2;
    node.setX(x);
    QCOMPARE(node.x(), x);

    // Tests if 'Node::setY()' works as expected
    y = 3;
    node.setY(y);
    QCOMPARE(node.y(), y);

    // Tests if 'Node::setYCoords()' works as expected
    x = 3;
    y = 4;
    node.setCoords(x,y);
    QCOMPARE(node.x(), x);
    QCOMPARE(node.y(), y);

    // Tests if degree functions return 0 with an empty node
    QCOMPARE(node.degree(), 0);
    QCOMPARE(node.inDegree(), 0);
    QCOMPARE(node.outDegree(), 0);

    // Tests if 'Node::clone()' works as expected.
    NodePtr clone = node.clone();
    Node *c = clone.get();
    QCOMPARE(c->id(), node.id());
    QCOMPARE(c->x(), node.x());
    QCOMPARE(c->y(), node.y());
    QCOMPARE(c->attrs().size(), node.attrs().size());
    QCOMPARE(c->attrs().names(), node.attrs().names());
    QCOMPARE(c->attrs().values(), node.attrs().values());
    QCOMPARE(c->degree(), node.degree());
    QCOMPARE(c->inDegree(), node.inDegree());
    QCOMPARE(c->outDegree(), node.outDegree());
}

} // evoplex
QTEST_MAIN(evoplex::TestNode)
#include "tst_node.moc"
