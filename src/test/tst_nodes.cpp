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
#include <QDir>
#include <QStringList>

#include <attributerange.h>
#include <nodes.h>

namespace evoplex {
class TestNodes: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}

    void tst_fromCmd();

    // saving a set of nodes without attributes
    void tst_saveToFile_no_attrs();
    // saving a set of nodes with attributes
    void tst_saveToFile_with_attrs();

    // file with valid attributes, file without 2d coordinates
    void tst_fromFile_nodes_no_xy();
    // valid attributes AND both xy coordinates
    void tst_fromFile_nodes_with_xy();
    // valid attributes AND x coordinates (only)
    void tst_fromFile_nodes_with_x();
    // valid attributes AND y coordinates (only)
    void tst_fromFile_nodes_with_y();
    // invalid attributes
    void tst_fromFile_nodes_invalid_attrs();
    // invalid file
    void tst_fromFile_nodes_invalid_file();
private:
    // checks if sets of nodes have the same content
    void _compare_nodes(const Nodes& a, const Nodes& b) const;
};

void TestNodes::tst_fromCmd()
{
    QString errorMsg;

    /* example of empty call
     *
    QString cmd = "";
    AttributesScope attrsScope;
    GraphType graphType = GraphType::Undirected;
    Nodes nodes = Nodes::fromCmd(cmd, attrsScope, graphType, errorMsg);
    */

    /*
     * Potential cases:
     *  - valid * command
     *  - valid # command
     *  - empty command: should fail
     *  - empty attrsScope: ok! a set of nodes without attrs
     *  - invalid commands
     *      - negavite number of nodes
     *      - zero nodes
     *      - cmd with an attribute that is not in attrsScope
     *  - Invalid graph type: should fail
     *  - Undirected graph: should return a set of UNodes
     *  - Directed graph: should return a set of DNodes
     */
     /** IMPORTANT! The command validation is powered by another class, which we will be testing later.
      *    So, the main purpose of this test is to check if the returned Nodes are reflecting the command,
      *    that's the number of nodes is right, the attrs are there etc.
      *    There's no need to extend it to the point of testing all possible combinations of commands
      *    NOR testing if all possible functions (eg., min, max, rand ...) work as expected -- it'll be done later for another class.
     */
}

void TestNodes::tst_fromFile_nodes_no_xy()
{
    QString errorMsg;
    GraphType graphType = GraphType::Undirected;

    // valid attrsScope for the existing file
    const QString filePath(":/data/data/nodes_no_xy.csv");
    AttributesScope attrsScope;
    AttributeRange* col0 = AttributeRange::parse(0, "int-zero-two", "int[0,2]");
    attrsScope.insert(col0->attrName(), col0);
    AttributeRange* col1 = AttributeRange::parse(1, "bool", "bool");
    attrsScope.insert(col1->attrName(), col1);
    AttributeRange* col2 = AttributeRange::parse(2, "any-string", "string");
    attrsScope.insert(col2->attrName(), col2);

    Nodes nodes = Nodes::fromFile(filePath, attrsScope, graphType, errorMsg);

    /*
     * Potential cases:
     *  - check if Nodes contains the same data as 'nodes_no_xy.csv'
     *  - what if attrsScope has more attributes than 'nodes_no_xy.csv'? should fail
     */
}

/****************** TO DO ****************************/
// valid attributes AND both xy coordinates
void TestNodes::tst_fromFile_nodes_with_xy() {
    QString errorMsg;
    GraphType graphType = GraphType::Undirected;

    // valid attrsScope for the existing file
    const QString filePath(":/data/data/nodes_with_xy.csv");
    AttributesScope attrsScope;

    // AttributeRange to be used for both sets of nodes
    AttributeRange* col0 = AttributeRange::parse(0, "bool", "bool");
    attrsScope.insert(col0->attrName(), col0);

    // Nodes with values read from file
    Nodes nodesFromFile = Nodes::fromFile(filePath, attrsScope, graphType, errorMsg);

    // Nodes to test against
    Nodes nodes = Nodes::fromCmd("*3;min", attrsScope, graphType, errorMsg);

    nodes.at(0)->setAttr(0, true);
    nodes.at(0)->setX(123);
    nodes.at(0)->setY(456);

    nodes.at(1)->setAttr(0, false);
    nodes.at(1)->setX(456);
    nodes.at(1)->setY(-789);

    nodes.at(2)->setAttr(0, true);
    nodes.at(2)->setX(-789);
    nodes.at(2)->setY(789);

    _compare_nodes(nodes,nodesFromFile);
}
// valid attributes AND x coordinates (only)
void TestNodes::tst_fromFile_nodes_with_x() {
    QString errorMsg;
    GraphType graphType = GraphType::Undirected;

    // valid attrsScope for the existing file
    const QString filePath(":/data/data/nodes_with_x.csv");
    AttributesScope attrsScope;

    // Attribute Range to be used for both sets of nodes
    AttributeRange* col0 = AttributeRange::parse(0, "double-zero-one", "double[0,2]");
    attrsScope.insert(col0->attrName(), col0);

    // Nodes with values read from file
    Nodes nodesFromFile = Nodes::fromFile(filePath, attrsScope, graphType, errorMsg);

    // Nodes to test against ones from file
    Nodes nodes = Nodes::fromCmd("*3;min", attrsScope, graphType, errorMsg);

    nodes.at(0)->setAttr(0, 0.12345678);
    nodes.at(0)->setX(123);

    nodes.at(1)->setAttr(0, 1.49999999);
    nodes.at(1)->setX(789);

    nodes.at(2)->setAttr(0, 2);
    nodes.at(2)->setX(456);

    // equivalent to _compare_nodes without the y value test
    QCOMPARE(nodes.size(), nodesFromFile.size());
    for (int id = 0; id < static_cast<int>(nodes.size()); ++id) {
        NodePtr nA = nodes.at(id);
        NodePtr nB = nodesFromFile.at(id);
        QCOMPARE(nA->id(), nB->id());
        QCOMPARE(nA->attrs().names(), nB->attrs().names());
        QCOMPARE(nA->attrs().values(), nB->attrs().values());
        QCOMPARE(nA->x(), nB->x());
    }
}
// valid attributes AND y coordinates (only)
void TestNodes::tst_fromFile_nodes_with_y() {
    QString errorMsg;
    GraphType graphType = GraphType::Undirected;

    // valid attrsScope for the existing file
    const QString filePath(":/data/data/nodes_with_y.csv");
    AttributesScope attrsScope;

    // Attribute Range to be used for both sets of nodes
    AttributeRange* col0 = AttributeRange::parse(0, "double-zero-one", "double[0,2]");
    attrsScope.insert(col0->attrName(), col0);

    // Nodes with values read from file
    Nodes nodesFromFile = Nodes::fromFile(filePath, attrsScope, graphType, errorMsg);

    // Nodes to test against ones from file
    Nodes nodes = Nodes::fromCmd("*3;min", attrsScope, graphType, errorMsg);

    nodes.at(0)->setAttr(0, 1.49999999);
    nodes.at(0)->setX(123);

    nodes.at(1)->setAttr(0, 1.49999999);
    nodes.at(1)->setX(789);

    nodes.at(2)->setAttr(0, 2);
    nodes.at(2)->setX(456);

    // equivalent to _compare_nodes without the x value test
    QCOMPARE(nodes.size(), nodesFromFile.size());
    for (int id = 0; id < static_cast<int>(nodes.size()); ++id) {
        NodePtr nA = nodes.at(id);
        NodePtr nB = nodesFromFile.at(id);
        QCOMPARE(nA->id(), nB->id());
        QCOMPARE(nA->attrs().names(), nB->attrs().names());
        QCOMPARE(nA->attrs().values(), nB->attrs().values());
        QCOMPARE(nA->y(), nB->y());
    }
}
// invalid attributes
void TestNodes::tst_fromFile_nodes_invalid_attrs() {
    QString errorMsg;
    GraphType graphType = GraphType::Undirected;

    // valid attrsScope for the existing file
    const QString filePath(":/data/data/nodes_with_invalid_attributes.csv");
    AttributesScope attrsScope;

    // Attribute Range to be used for both sets of nodes
    AttributeRange* col0 = AttributeRange::parse(0, "bool", "bool");
    attrsScope.insert(col0->attrName(), col0);

    // Nodes with values read from file
    Nodes nodesFromFile = Nodes::fromFile(filePath, attrsScope, graphType, errorMsg);

    QCOMPARE(nodesFromFile.size(), 0);
}
// invalid file
void TestNodes::tst_fromFile_nodes_invalid_file() {
    QString errorMsg;
    GraphType graphType = GraphType::Undirected;

    // valid attrsScope for the existing file
    const QString filePath(":/data/data/nodes_with_invalid_file.csv");
    AttributesScope attrsScope;

    // Attribute Range to be used for both sets of nodes
    AttributeRange* col0 = AttributeRange::parse(0, "bool", "bool");
    attrsScope.insert(col0->attrName(), col0);

    // Nodes with values read from file
    Nodes nodesFromFile = Nodes::fromFile(filePath, attrsScope, graphType, errorMsg);

    QCOMPARE(nodesFromFile.size(), 0);
}
/****************** TO DO ****************************/

void TestNodes::tst_saveToFile_no_attrs()
{
    QString errorMsg;
    const QString tempFilePath = QDir::temp().absoluteFilePath("nodes.csv");

    // saving a set of nodes without attributes
    AttributesScope attrsScope;
    Nodes nodes = Nodes::fromCmd("*5;min", attrsScope, GraphType::Undirected, errorMsg);
    QVERIFY(nodes.saveToFile(tempFilePath));

    // retrieve saved file
    Nodes nodesFromFile = Nodes::fromFile(tempFilePath, attrsScope, GraphType::Undirected, errorMsg);
    _compare_nodes(nodes, nodesFromFile);
}

void TestNodes::tst_saveToFile_with_attrs()
{
    QString errorMsg;
    const QString tempFilePath = QDir::temp().absoluteFilePath("nodes.csv");

    /** TO DO:
     * similar to 'tst_saveToFile_no_attrs()', but here we pass a non-empty attrsScope **/
}

void TestNodes::_compare_nodes(const Nodes& a, const Nodes& b) const
{
    QCOMPARE(a.size(), b.size());
    for (int id = 0; id < static_cast<int>(a.size()); ++id) {
        NodePtr nA = a.at(id);
        NodePtr nB = b.at(id);
        QCOMPARE(nA->id(), nB->id());
        QCOMPARE(nA->attrs().names(), nB->attrs().names());
        QCOMPARE(nA->attrs().values(), nB->attrs().values());
        QCOMPARE(nA->x(), nB->x());
        QCOMPARE(nA->y(), nB->y());
    }
}

} // evoplex
QTEST_MAIN(evoplex::TestNodes)
#include "tst_nodes.moc"
