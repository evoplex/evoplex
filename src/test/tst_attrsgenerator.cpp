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

#include <memory>
#include <QtTest>
#include <core/attrsgenerator.h>

namespace evoplex {
class TestAttrsGenerator: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void tst_parseStarCmd();
    void tst_parseHashCmd();
    void tst_parseStarCmd_min();
    void _tst_attrs(SetOfAttributes res, Attributes attrs,  bool testValues);
};

void TestAttrsGenerator::_tst_attrs(SetOfAttributes res, Attributes attrs, bool testValues)
{
    for(int i = 0; i < res.size(); i++){
        QCOMPARE(res.at(i).names(), attrs.names());
        if(testValues) QCOMPARE(res.at(i).values(), attrs.values());
        QCOMPARE(res.at(i).size(), attrs.size());
    }
}

void TestAttrsGenerator::tst_parseStarCmd_min(){
    QString error, cmd;
    AttributesScope attrsScope;
    AttrsGenerator* agen;
    QStringList names = {"int-zero-two", "bool", "any-string"};
    QStringList attrRgeStrs = {"int[0,2]", "bool", "string"};
    SetOfAttributes res;
    Attributes attrs;
    int sizeOfAgen = 3;

    // Valid * command with empty attrScope
    cmd = QString("*%1;min").arg(Value(sizeOfAgen).toQString());
    agen = AttrsGenerator::parse(attrsScope, cmd, error);

    QCOMPARE(agen->command(), cmd);
    QCOMPARE(agen->size(), sizeOfAgen);

    res = agen->create();
    _tst_attrs(res, attrs, true);

    // Valid * command with non-empty attrScope
    AttributeRange* col0 = AttributeRange::parse(0, names.at(0), attrRgeStrs.at(0));
    attrsScope.insert(col0->attrName(), col0);
    AttributeRange* col1 = AttributeRange::parse(1, names.at(1), attrRgeStrs.at(1));
    attrsScope.insert(col1->attrName(), col1);
    AttributeRange* col2 = AttributeRange::parse(2, names.at(2), attrRgeStrs.at(2));
    attrsScope.insert(col2->attrName(), col2);

    agen = AttrsGenerator::parse(attrsScope, cmd, error);

    attrs.resize(3);
    for(int i = 0; i < attrs.size(); i++){
        attrs.replace(i, names.at(i), NULL);
    }

    QCOMPARE(agen->command(), cmd);
    QCOMPARE(agen->size(), sizeOfAgen);

    res = agen->create();

    _tst_attrs(res, attrs, false);
}

void TestAttrsGenerator::tst_parseStarCmd()
{
    QString error;

    // - same mode for all attributes:
    //         '*integer;[min|max|rand_seed]'

    /** IMPORTANT: this function is likely to be very long,
     * if so, consider breaking into smaller functions
     * (perhaps tst_parseStarCmd_min(), tst_parseStarCmd_max() ...) **/


    // Potential cases:
    // - agen->command() should return the same 'cmd' string passed through the parser()
    // - agen->size() should return the same number of 'cmd'


    // use the create() function to create the set of attributes.
    // Each element is an 'Attributes' object
    // our aim is to check if 'cmd' is able to generate a SetOfAttributes with the correct data
    // SetOfAttributes res = agen->create();

}

void TestAttrsGenerator::tst_parseHashCmd()
{
    QString error;

    // - specific mode for each attribute:
    //       '#integer;attrName_[min|max|rand_seed|value_val];...'
    /*
    AttributesScope attrsScope =
    QString cmd =
    AttrsGenerator* AttrsGenerator::parse(attrsScope,cmd, error);
    */
    /** IMPORTANT: this function is likely to be very long,
     * if so, consider breaking into smaller functions
     * (perhaps tst_parseStarHash_min(), tst_parseHashCmd_max() ...) **/

    // Potential cases:
    // - AttrsGenerator::command() should return the same 'cmd' string passed through the parser()
    // - AttrsGenerator::size() should return the same number of 'cmd'

    // use the create() function to create the set of attributes.
    // Each element is an Attributes object
    // our aim is to check if 'cmd' is able to generate a SetOfAttributes with the correct data
    // SetOfAttributes res = agen->create();
}

} // evoplex
QTEST_MAIN(evoplex::TestAttrsGenerator)
#include "tst_attrsgenerator.moc"
