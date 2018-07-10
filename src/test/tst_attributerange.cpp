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
#include <attributerange.h>


using namespace evoplex;

class TestAttributeRange: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void tst_attrRge();
    void tst_bool();
};

void TestAttributeRange::tst_attrRge(){
//    AttributeRange::Type t = AttributeRange::Int_Range;
//    AttributeRange* a = AttributeRange::parse(0, "test", "int[0,1]");
//    Value v = a->validate("int[0,1]");
//    QCOMPARE(a->type(), t);

}
// test invalid type
void TestAttributeRange::tst_bool(){
    AttributeRange* attrRge = AttributeRange::parse(0, "test", "bool");
    const int null = 0;

    // Tests value returned by 'AttributeRange::validate()'
    Value v = attrRge->validate("true");
    QVERIFY(v.isValid());

    QVERIFY(v.type() == Value::BOOL);
    QVERIFY(v.isBool());

    QVERIFY(!v.isDouble());
    QVERIFY(!v.isChar());
    QVERIFY(!v.isInt());
    QVERIFY(!v.isString());

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), "test");
    QCOMPARE(attrRge->attrRangeStr(), "bool");

    AttributeRange::Type type = AttributeRange::Bool;
    QCOMPARE(attrRge->type(), type);

    // min() and max() are not tested as both return <null> for a boolean type

    // Tests 'AttributeRange::rand()' works as expected
    PRG* prg = new PRG(123);
    Value v1 = attrRge->rand(prg);
    QVERIFY(v1.isValid());

    QVERIFY(v1.type() == Value::BOOL);
    QVERIFY(v1.isBool());

    QVERIFY(!v1.isDouble());
    QVERIFY(!v1.isChar());
    QVERIFY(!v1.isInt());
    QVERIFY(!v1.isString());
}
//More than or equal to min, less than or equal to max
QTEST_MAIN(TestAttributeRange)
#include "tst_attributerange.moc"
