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
    void tst_int_range();
    void tst_double_range();

    // Auxiliary functions
    void _tst_int_value(Value v);
    void _tst_double_value(Value v);
};

// attrNames and attrRangeStrs are stored in arrays in order to make it easier
// to add test cases in the future

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

    // Tests value returned by 'AttributeRange::validate()' for true
    Value v = attrRge->validate("true");
    QVERIFY(v.isValid());

    QVERIFY(v.type() == Value::BOOL);
    QVERIFY(v.isBool());

    QVERIFY(!v.isDouble());
    QVERIFY(!v.isChar());
    QVERIFY(!v.isInt());
    QVERIFY(!v.isString());

    // Tests value returned by 'AttributeRange::validate()' for false
    v = attrRge->validate("false");
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

void TestAttributeRange::_tst_int_value(Value v){
    QVERIFY(v.isValid());

    QVERIFY(v.type() == Value::INT);
    QVERIFY(v.isInt());

    QVERIFY(!v.isDouble());
    QVERIFY(!v.isChar());
    QVERIFY(!v.isBool());
    QVERIFY(!v.isString());
}

void TestAttributeRange::tst_int_range(){
    const int TEST_CASES = 3;
    const char* attrNames[TEST_CASES] = {"test0", "test1", "test2"};
    // Case 1: regular range
    // Case 2: negative numbers
    // Case 3: large range
    const char* attrRangeStrs[TEST_CASES] = {"int[0,1]", "int[-10,-5]", "int[-100,100]"};

    AttributeRange* attrRge = AttributeRange::parse(0, attrNames[0], attrRangeStrs[0]);
    AttributeRange* attrRge2 = AttributeRange::parse(1, attrNames[1], attrRangeStrs[1]);
    AttributeRange* attrRge3 = AttributeRange::parse(2, attrNames[2], attrRangeStrs[2]);

    // Tests value returned by 'AttributeRange::validate()'
    Value v = attrRge->validate("0");   //min
    _tst_int_value(v);
    v = attrRge->validate("1");   //max
    _tst_int_value(v);

    v = attrRge2->validate("-10");   //min
    _tst_int_value(v);
    v = attrRge2->validate("-5");   //max
    _tst_int_value(v);
    v = attrRge2->validate("-7");   //other value
    _tst_int_value(v);

    v = attrRge3->validate("-100");   //min
    _tst_int_value(v);
    v = attrRge3->validate("100");   //max
    _tst_int_value(v);
    v = attrRge3->validate("0");   //other value
    _tst_int_value(v);

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), attrNames[0]);
    QCOMPARE(attrRge->attrRangeStr(), attrRangeStrs[0]);

    QVERIFY(attrRge2->isValid());
    QCOMPARE(attrRge2->id(), 1);
    QCOMPARE(attrRge2->attrName(), attrNames[1]);
    QCOMPARE(attrRge2->attrRangeStr(), attrRangeStrs[1]);

    QVERIFY(attrRge3->isValid());
    QCOMPARE(attrRge3->id(), 2);
    QCOMPARE(attrRge3->attrName(), attrNames[2]);
    QCOMPARE(attrRge3->attrRangeStr(), attrRangeStrs[2]);

    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), attrNames[0]);
    QCOMPARE(attrRge->attrRangeStr(), attrRangeStrs[0]);

    QVERIFY(attrRge2->isValid());
    QCOMPARE(attrRge2->id(), 1);
    QCOMPARE(attrRge2->attrName(), attrNames[1]);
    QCOMPARE(attrRge2->attrRangeStr(), attrRangeStrs[1]);

    QVERIFY(attrRge3->isValid());
    QCOMPARE(attrRge3->id(), 2);
    QCOMPARE(attrRge3->attrName(), attrNames[2]);
    QCOMPARE(attrRge3->attrRangeStr(), attrRangeStrs[2]);

    AttributeRange::Type type = AttributeRange::Int_Range;
    QCOMPARE(attrRge->type(), type);
    QCOMPARE(attrRge2->type(), type);
    QCOMPARE(attrRge3->type(), type);



    // Tests min(), max() and rand() functions
    int min, max;
    PRG* prg = new PRG(123);

    min = 0;
    max = 1;
    QCOMPARE(attrRge->min(), min);
    QCOMPARE(attrRge->max(), max);
    v = attrRge->rand(prg);
    _tst_int_value(v);
    QVERIFY(v.toInt() >= min);
    QVERIFY(v.toInt() <= max);

    min = -10;
    max = -5;
    QCOMPARE(attrRge2->min(), min);
    QCOMPARE(attrRge2->max(), max);
    v = attrRge2->rand(prg);
    _tst_int_value(v);
    QVERIFY(v.toInt() >= min);
    QVERIFY(v.toInt() <= max);

    min = -100;
    max = 100;
    QCOMPARE(attrRge3->min(), min);
    QCOMPARE(attrRge3->max(), max);
    v = attrRge3->rand(prg);
    _tst_int_value(v);
    QVERIFY(v.toInt() >= min);
    QVERIFY(v.toInt() <= max);





}

void TestAttributeRange::_tst_double_value(Value v){
    QVERIFY(v.isValid());

    QVERIFY(v.type() == Value::DOUBLE);
    QVERIFY(v.isDouble());

    QVERIFY(!v.isInt());
    QVERIFY(!v.isChar());
    QVERIFY(!v.isBool());
    QVERIFY(!v.isString());
}

void TestAttributeRange::tst_double_range(){
    const int TEST_CASES = 3;
    const char* attrNames[TEST_CASES] = {"test0", "test1", "test2"};
    // Case 1: regular range
    // Case 2: negative numbers
    // Case 3: large range
    const char* attrRangeStrs[TEST_CASES] = {"double[1.1,1.2]", "double[-5.5,-3.3]", "double[-95.7,87.5]"};

    AttributeRange* attrRge = AttributeRange::parse(0, attrNames[0], attrRangeStrs[0]);
    AttributeRange* attrRge2 = AttributeRange::parse(1, attrNames[1], attrRangeStrs[1]);
    AttributeRange* attrRge3 = AttributeRange::parse(2, attrNames[2], attrRangeStrs[2]);

    // Tests value returned by 'AttributeRange::validate()'
    Value v = attrRge->validate("1.1");   //min
    _tst_double_value(v);
    v = attrRge->validate("1.2");   //max
    _tst_double_value(v);
    v = attrRge->validate("1.15");   //other value
    _tst_double_value(v);

    v = attrRge2->validate("-5.5");   //min
    _tst_double_value(v);
    v = attrRge2->validate("-3.3");   //max
    _tst_double_value(v);
    v = attrRge2->validate("-4.1");   //other value
    _tst_double_value(v);

    v = attrRge3->validate("-95.7");   //min
    _tst_double_value(v);
    v = attrRge3->validate("87.5");   //max
    _tst_double_value(v);
    v = attrRge3->validate("5.6");   //other value
    _tst_double_value(v);

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), attrNames[0]);
    QCOMPARE(attrRge->attrRangeStr(), attrRangeStrs[0]);

    QVERIFY(attrRge2->isValid());
    QCOMPARE(attrRge2->id(), 1);
    QCOMPARE(attrRge2->attrName(), attrNames[1]);
    QCOMPARE(attrRge2->attrRangeStr(), attrRangeStrs[1]);

    QVERIFY(attrRge3->isValid());
    QCOMPARE(attrRge3->id(), 2);
    QCOMPARE(attrRge3->attrName(), attrNames[2]);
    QCOMPARE(attrRge3->attrRangeStr(), attrRangeStrs[2]);

    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), attrNames[0]);
    QCOMPARE(attrRge->attrRangeStr(), attrRangeStrs[0]);

    QVERIFY(attrRge2->isValid());
    QCOMPARE(attrRge2->id(), 1);
    QCOMPARE(attrRge2->attrName(), attrNames[1]);
    QCOMPARE(attrRge2->attrRangeStr(), attrRangeStrs[1]);

    QVERIFY(attrRge3->isValid());
    QCOMPARE(attrRge3->id(), 2);
    QCOMPARE(attrRge3->attrName(), attrNames[2]);
    QCOMPARE(attrRge3->attrRangeStr(), attrRangeStrs[2]);

    AttributeRange::Type type = AttributeRange::Double_Range;
    QCOMPARE(attrRge->type(), type);
    QCOMPARE(attrRge2->type(), type);
    QCOMPARE(attrRge3->type(), type);



    // Tests min(), max() and rand() functions
    double min, max;
    PRG* prg = new PRG(123);

    min = 1.1;
    max = 1.2;
    QCOMPARE(attrRge->min(), min);
    QCOMPARE(attrRge->max(), max);
    v = attrRge->rand(prg);
    _tst_double_value(v);
    QVERIFY(v.toDouble() >= min);
    QVERIFY(v.toDouble() <= max);

    min = -5.5;
    max = -3.3;
    QCOMPARE(attrRge2->min(), min);
    QCOMPARE(attrRge2->max(), max);
    v = attrRge2->rand(prg);
    _tst_double_value(v);
    QVERIFY(v.toDouble() >= min);
    QVERIFY(v.toDouble() <= max);

    min = -95.7;
    max = 87.5;
    QCOMPARE(attrRge3->min(), min);
    QCOMPARE(attrRge3->max(), max);
    v = attrRge3->rand(prg);
    _tst_double_value(v);
    QVERIFY(v.toDouble() >= min);
    QVERIFY(v.toDouble() <= max);
}
QTEST_MAIN(TestAttributeRange)
#include "tst_attributerange.moc"
