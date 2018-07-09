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

using namespace evoplex;

class TestNode: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void tst_UNode();
};

void TestNode::tst_UNode(){
    // test constructor and attrs()
    int id = 0, x = 1, y = 2;
    Value values[3] = {Value(123), Value(234), Value(456)};
    QString names[3] = {"test0_0", "test0_1", "test0_2"};

    Attributes attrs(3);
    attrs.replace(0, names[0], values[0]);
    attrs.replace(1, names[1], values[1]);
    attrs.replace(2, names[2], values[2]);

    UNode node(id, attrs, x, y);

    QCOMPARE(node.id(), id);

    QCOMPARE(node.attrs().size(), attrs.size());
    QCOMPARE(node.attrs().names(), attrs.names());
    QCOMPARE(node.attrs().values(), attrs.values());

    for (int i = 0; i < 3; i++){
          QCOMPARE(node.attrs().name(i), attrs.name(i));
          QCOMPARE(node.attrs().value(i), attrs.value(i));
    }

    QCOMPARE(node.x(), x);
    QCOMPARE(node.y(), y);

    // Tests if 'Node::clone()' works as expected.
    NodePtr clone = node.clone();
    // test all associated getter methods

    // Tests if 'Node::setAttr()' works as expected.
    node.setAttr(0, Value(567));

    QCOMPARE(node.attrs().name(0), attrs.name(0));
    QCOMPARE(node.attr("test0_0"),Value(567));

    QCOMPARE(node.attr(0), Value(567));
    QCOMPARE(node.attrs().value(0), Value(567));

    // Tests if 'Node::setX()' 'Node::setY()' work as expected (separate)
    x = 2;
    node.setX(x);
    QCOMPARE(node.x(), x);

    y = 3;
    node.setY(y);
    QCOMPARE(node.y(), y);

    x = 3;
    y = 4;
    node.setCoords(x,y);
    QCOMPARE(node.x(), x);
    QCOMPARE(node.y(), y);
}
/* TO DO:

This test should cover the functions below.

constructors:
     UNode(int id, Attributes attrs, int x, int y)
     DNode(int id, Attributes attrs, int x, int y)

functions:
    inline const Attributes& attrs() const;
    inline const Value& attr(const char* name) const;
    inline const Value& attr(const int id) const;
    inline void setAttr(const int id, const Value& value);

    inline int id() const;
    inline int x() const;
    inline void setX(int x);
    inline int y() const;
    inline void setY(int y);
    inline void setCoords(int x, int y);

    inline NodePtr clone() const override;
    inline const Edges& inEdges() const override;
    inline const Edges& outEdges() const override;
    inline int degree() const override;
    inline int inDegree() const override;
    inline int outDegree() const override;
*/


QTEST_MAIN(TestNode)
#include "tst_node.moc"
