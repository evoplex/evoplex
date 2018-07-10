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
#include <edge.h>
#include <node.h>

using namespace evoplex;

class TestEdge: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase() {}
    void tst_edge();

private:
    NodePtr m_nodeA;
    NodePtr m_nodeB;
};

void TestEdge::initTestCase()
{
    m_nodeA = std::make_shared<UNode>(0, Attributes());
    m_nodeB = std::make_shared<UNode>(1, Attributes());
}

void TestEdge::tst_edge(){
    // NOTES:
    // create for both directions and ways
    // check marked tests
    // check with boolean, and that it deletes that attributes pointer

    // Tests if both ways of creating edges work as expected

    // Tests method 1: adding attributes after edge is constructed
    Edge edge(0, m_nodeA, m_nodeB);

    // Tests if the attributes of a node is empty on creation
    QVERIFY(edge.attrs()->isEmpty());
    QCOMPARE(edge.attrs()->size(), 0);
    QCOMPARE(edge.attrs()->names().size(), 0);
    QCOMPARE(edge.attrs()->values().size(), 0);

    // Tests if 'Edge::addAttr()' works as expected, for an empty attribute
    edge.addAttr("test0", Value(123));
    QVERIFY(!edge.attrs()->isEmpty());
    QCOMPARE(edge.attrs()->size(), 1);
    QCOMPARE(edge.attrs()->names().size(), 1);
    QCOMPARE(edge.attrs()->values().size(), 1);

    QCOMPARE(edge.attrs()->name(0), "test0");
    QCOMPARE(edge.attrs()->value(0), Value(123));

    // Tests if 'Edge::attr()' works as expected
    QCOMPARE(edge.attr("test0"), Value(123));
    QCOMPARE(edge.attr(0), Value(123));

    // Tests if 'Edge::setAttr()' works as expected
    edge.setAttr(0, Value(234));
    QCOMPARE(edge.attrs()->name(0), "test0"); // Tests that name has not changed
    QCOMPARE(edge.attr(0), Value(234));

    // Tests if 'Edge::addAttr()' works as expected, for an attribute with an existing value
    edge.addAttr("test1", Value(345));
    QVERIFY(!edge.attrs()->isEmpty());
    QCOMPARE(edge.attrs()->size(), 2);
    QCOMPARE(edge.attrs()->names().size(), 2);
    QCOMPARE(edge.attrs()->values().size(), 2);

    QCOMPARE(edge.attrs()->name(1), "test1");
    QCOMPARE(edge.attrs()->value(1), Value(345));

    // Tests if 'Edge::id()' works as expected
    QCOMPARE(edge.id(), 0);
    QCOMPARE(edge.origin(), m_nodeA);
    QCOMPARE(edge.neighbour(), m_nodeB);

    // Tests method 2: adding attributes in constructor
    Attributes* attrs = new Attributes();
    attrs->push_back("test0", Value(123));
    Edge edge2(1, m_nodeA, m_nodeB, attrs);

    QVERIFY(!edge2.attrs()->isEmpty());
    QCOMPARE(edge2.attrs()->size(), 1);
    QCOMPARE(edge2.attrs()->names().size(), 1);
    QCOMPARE(edge2.attrs()->values().size(), 1);

    QCOMPARE(edge2.attrs()->name(0), "test0");
    QCOMPARE(edge2.attrs()->value(0), Value(123));

    // Tests if 'Edge::attr()' works as expected
    QCOMPARE(edge2.attr("test0"), Value(123));
    QCOMPARE(edge2.attr(0), Value(123));

    // Tests if 'Edge::setAttr()' works as expected
    edge2.setAttr(0, Value(234));
    QCOMPARE(edge2.attrs()->name(0), "test0"); // Tests that name has not changed
    QCOMPARE(edge2.attr(0), Value(234));

    // CHECK !!!
//    // Tests if 'Edge::addAttr()' works as expected, for an attribute with an existing value
//    edge.addAttr("test1", Value(345));
//    QVERIFY(!edge2.attrs()->isEmpty());
//    QCOMPARE(edge2.attrs()->size(), 2);
//    QCOMPARE(edge2.attrs()->names().size(), 2);
//    QCOMPARE(edge2.attrs()->values().size(), 2);

//    QCOMPARE(edge2.attrs()->name(1), "test1");
//    QCOMPARE(edge2.attrs()->value(1), Value(345));

    // Tests if 'Edge::id()' works as expected
    QCOMPARE(edge2.id(), 1);
    QCOMPARE(edge2.origin(), m_nodeA);
    QCOMPARE(edge2.neighbour(), m_nodeB);

}

QTEST_MAIN(TestEdge)
#include "tst_edge.moc"
