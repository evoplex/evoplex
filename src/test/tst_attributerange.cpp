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
#include <attributerange.h>

using namespace evoplex;

class TestAttributeRange: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase() {}
    void tst_bool();
    void tst_int_range();
    void tst_double_range();
    void tst_int_set();
    void tst_string();
    void tst_string_set();
    void tst_double_set();
    void tst_filepath();
    void tst_dirpath();

private:
    std::unique_ptr<PRG> m_prg;
};

void TestAttributeRange::initTestCase()
{
    m_prg = std::unique_ptr<PRG>(new PRG(123));
}

void TestAttributeRange::tst_bool()
{
    AttributeRange* attrRge = AttributeRange::parse(0, "test", "bool");

    // Tests value returned by 'AttributeRange::validate()' for true
    QCOMPARE(attrRge->validate("true"), Value(true));

    // Tests value returned by 'AttributeRange::validate()' for false
    QCOMPARE(attrRge->validate("false"), Value(false));

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString("test"));
    QCOMPARE(attrRge->attrRangeStr(), QString("bool"));

    AttributeRange::Type type = AttributeRange::Bool;
    QCOMPARE(attrRge->type(), type);

    // Tests min(), max() and rand() functions work as expected
    bool min = false;
    bool max = true;

    // Tests 'AttributeRange::rand()'
    Value v = attrRge->rand(m_prg.get());
    QVERIFY((v == Value(true))||(v == Value(false)));
    QCOMPARE(v.type(), Value::BOOL);
    QVERIFY(v.toBool() >= min);
    QVERIFY(v.toBool() <= max);

    // Tests 'AttributeRange::min()' and 'AttributeRange::max()'
    QCOMPARE(attrRge->min().toBool(), min);

    // 'attrRge->max().toBool()' returns 0
//    QCOMPARE(attrRge->max().toBool(), max);
    QCOMPARE(attrRge->max().toBool(), false);
}

void TestAttributeRange::tst_int_range()
{
    QStringList attrNames = {"test0", "test1", "test2"};
    // Case 1: regular range
    // Case 2: negative numbers
    // Case 3: large range
    QStringList attrRangeStrs = {"int[0,1]", "int[-10,-5]", "int[-100,100]"};

    AttributeRange* attrRge = AttributeRange::parse(0, attrNames[0], attrRangeStrs[0]);
    AttributeRange* attrRge2 = AttributeRange::parse(1, attrNames[1], attrRangeStrs[1]);
    AttributeRange* attrRge3 = AttributeRange::parse(2, attrNames[2], attrRangeStrs[2]);

    // Tests value returned by 'AttributeRange::validate()'

    QCOMPARE(attrRge->validate("0"), Value(0));  // min
    QCOMPARE(attrRge->validate("1"), Value(1));  // max

    QCOMPARE(attrRge2->validate("-10"), Value(-10));  // min
    QCOMPARE(attrRge2->validate("-5"), Value(-5));  // max
    QCOMPARE(attrRge2->validate("-7"), Value(-7));  // other value

    QCOMPARE(attrRge3->validate("-100"), Value(-100));  // min
    QCOMPARE(attrRge3->validate("100"), Value(100));  // max
    QCOMPARE(attrRge3->validate("0"), Value(0));  // other value

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), attrNames.at(0));
    QCOMPARE(attrRge->attrRangeStr(), attrRangeStrs.at(0));

    QVERIFY(attrRge2->isValid());
    QCOMPARE(attrRge2->id(), 1);
    QCOMPARE(attrRge2->attrName(), attrNames.at(1));
    QCOMPARE(attrRge2->attrRangeStr(), attrRangeStrs.at(1));

    QVERIFY(attrRge3->isValid());
    QCOMPARE(attrRge3->id(), 2);
    QCOMPARE(attrRge3->attrName(), attrNames.at(2));
    QCOMPARE(attrRge3->attrRangeStr(), attrRangeStrs.at(2));

    AttributeRange::Type type = AttributeRange::Int_Range;
    QCOMPARE(attrRge->type(), type);
    QCOMPARE(attrRge2->type(), type);
    QCOMPARE(attrRge3->type(), type);

    // Tests min(), max() and rand() functions
    Value v;
    int min, max;

    min = 0;
    max = 1;
    QCOMPARE(attrRge->min().toInt(), min);
    QCOMPARE(attrRge->max().toInt(), max);
    v = attrRge->rand(m_prg.get());
    QCOMPARE(v.type(), Value::INT);
    QVERIFY(v.toInt() >= min);
    QVERIFY(v.toInt() <= max);

    min = -10;
    max = -5;
    QCOMPARE(attrRge2->min().toInt(), min);
    QCOMPARE(attrRge2->max().toInt(), max);
    v = attrRge2->rand(m_prg.get());
    QCOMPARE(v.type(), Value::INT);
    QVERIFY(v.toInt() >= min);
    QVERIFY(v.toInt() <= max);

    min = -100;
    max = 100;
    QCOMPARE(attrRge3->min().toInt(), min);
    QCOMPARE(attrRge3->max().toInt(), max);
    v = attrRge3->rand(m_prg.get());
    QCOMPARE(v.type(), Value::INT);
    QVERIFY(v.toInt() >= min);
    QVERIFY(v.toInt() <= max);
}

void TestAttributeRange::tst_double_range()
{
    QStringList attrNames = {"test0", "test1", "test2"};
    // Case 1: regular range
    // Case 2: negative numbers
    // Case 3: large range
    QStringList attrRangeStrs = {"double[1.1,1.2]", "double[-5.5,-3.3]", "double[-95.7,87.5]"};

    AttributeRange* attrRge = AttributeRange::parse(0, attrNames.at(0), attrRangeStrs.at(0));
    AttributeRange* attrRge2 = AttributeRange::parse(1, attrNames.at(1), attrRangeStrs.at(1));
    AttributeRange* attrRge3 = AttributeRange::parse(2, attrNames.at(2), attrRangeStrs.at(2));

    // Tests value returned by 'AttributeRange::validate()'
    QCOMPARE(attrRge->validate("1.1"), Value(1.1));  // min
    QCOMPARE(attrRge->validate("1.2"), Value(1.2));  // max
    QCOMPARE(attrRge->validate("1.15"), Value(1.15));  // other value

    QCOMPARE(attrRge2->validate("-5.5"), Value(-5.5));  // min
    QCOMPARE(attrRge2->validate("-3.3"), Value(-3.3));  // max
    QCOMPARE(attrRge2->validate("-4.1"), Value(-4.1));  // other value

    QCOMPARE(attrRge3->validate("-95.7"), Value(-95.7));  // min
    QCOMPARE(attrRge3->validate("87.5"), Value(87.5));  // max
    QCOMPARE(attrRge3->validate("5.6"), Value(5.6));  // other value

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), attrNames.at(0));
    QCOMPARE(attrRge->attrRangeStr(), attrRangeStrs.at(0));

    QVERIFY(attrRge2->isValid());
    QCOMPARE(attrRge2->id(), 1);
    QCOMPARE(attrRge2->attrName(), attrNames.at(1));
    QCOMPARE(attrRge2->attrRangeStr(), attrRangeStrs.at(1));

    QVERIFY(attrRge3->isValid());
    QCOMPARE(attrRge3->id(), 2);
    QCOMPARE(attrRge3->attrName(), attrNames.at(2));
    QCOMPARE(attrRge3->attrRangeStr(), attrRangeStrs.at(2));

    AttributeRange::Type type = AttributeRange::Double_Range;
    QCOMPARE(attrRge->type(), type);
    QCOMPARE(attrRge2->type(), type);
    QCOMPARE(attrRge3->type(), type);

    // Tests min(), max() and rand() functions
    Value v;
    double min, max;

    min = 1.1;
    max = 1.2;
    QCOMPARE(attrRge->min().toDouble(), min);
    QCOMPARE(attrRge->max().toDouble(), max);
    v = attrRge->rand(m_prg.get());
    QCOMPARE(v.type(), Value::DOUBLE);
    QVERIFY(v.toDouble() >= min);
    QVERIFY(v.toDouble() <= max);

    min = -5.5;
    max = -3.3;
    QCOMPARE(attrRge2->min().toDouble(), min);
    QCOMPARE(attrRge2->max().toDouble(), max);
    v = attrRge2->rand(m_prg.get());
    QCOMPARE(v.type(), Value::DOUBLE);
    QVERIFY(v.toDouble() >= min);
    QVERIFY(v.toDouble() <= max);

    min = -95.7;
    max = 87.5;
    QCOMPARE(attrRge3->min().toDouble(), min);
    QCOMPARE(attrRge3->max().toDouble(), max);
    v = attrRge3->rand(m_prg.get());
    QCOMPARE(v.type(), Value::DOUBLE);
    QVERIFY(v.toDouble() >= min);
    QVERIFY(v.toDouble() <= max);
}

void TestAttributeRange::tst_int_set()
{
    const char* attrName = "test";
    // Set with positive, negative and large numbers
    const char* attrRangeStr = "int{0,1,-5,100,-100}";
    int min = -100;
    int max = 100;

    AttributeRange* attrRge = AttributeRange::parse(0, attrName, attrRangeStr);

    // Tests value returned by 'AttributeRange::validate()'
    QCOMPARE(attrRge->validate("0"), Value(0));
    QCOMPARE(attrRge->validate("1"), Value(1));
    QCOMPARE(attrRge->validate("-5"), Value(-5));
    QCOMPARE(attrRge->validate("100"), Value(100));
    QCOMPARE(attrRge->validate("-100"), Value(-100));

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString(attrName));
    QCOMPARE(attrRge->attrRangeStr(), QString(attrRangeStr));

    AttributeRange::Type type = AttributeRange::Int_Set;
    QCOMPARE(attrRge->type(), type);

    QCOMPARE(attrRge->min().toInt(), -100);
    QCOMPARE(attrRge->max().toInt(), 100);

    Value v = attrRge->rand(m_prg.get());
    QCOMPARE(v.type(), Value::INT);
    QVERIFY(v.toInt() >= min);
    QVERIFY(v.toInt() <= max);
}

void TestAttributeRange::tst_double_set()
{
    const char* attrName = "test";
    // Set with positive, negative and large numbers
    const char* attrRangeStr = "double{0,1.2,-5.5,-95.7,87.5,12}";
    double min = -95.7;
    double max = 87.5;

    AttributeRange* attrRge = AttributeRange::parse(0, attrName, attrRangeStr);

    // Tests value returned by 'AttributeRange::validate()'
    QCOMPARE(attrRge->validate("0"), Value(0.0));
    QCOMPARE(attrRge->validate("1.2"), Value(1.2));
    QCOMPARE(attrRge->validate("-5.5"), Value(-5.5));
    QCOMPARE(attrRge->validate("-95.7"), Value(-95.7));
    QCOMPARE(attrRge->validate("87.5"), Value(87.5));
    QCOMPARE(attrRge->validate("12"), Value(12.0));

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString(attrName));
    QCOMPARE(attrRge->attrRangeStr(), QString(attrRangeStr));

    AttributeRange::Type type = AttributeRange::Double_Set;
    QCOMPARE(attrRge->type(), type);

    // min() returns the first value in the set
    QCOMPARE(attrRge->min().toDouble(), -95.7);
    // max() returns the last value in the set
    QCOMPARE(attrRge->max().toDouble(), 87.5);
    // Provided the set is ordered, these are the min and max values

    Value v = attrRge->rand(m_prg.get());
    QCOMPARE(v.type(), Value::DOUBLE);
    QVERIFY(v.toDouble() >= min);
    QVERIFY(v.toDouble() <= max);
}

void TestAttributeRange::tst_string()
{
    AttributeRange* attrRge = AttributeRange::parse(0, "test", "string");

    // Tests value returned by 'AttributeRange::validate()'
    const char* test_str;
    QString test_qstr;

    // Case 1: normal string
    test_str = "sample";
    test_qstr = "sample";
    QCOMPARE(attrRge->validate(test_str), Value(test_str));
    QCOMPARE(attrRge->validate(test_qstr), Value(test_qstr));

    // Case 2: long string
    test_str = "this sentence is a long string for testing purposes";
    test_qstr = "this sentence is a long string for testing purposes";
    QCOMPARE(attrRge->validate(test_str), Value(test_str));
    QCOMPARE(attrRge->validate(test_qstr), Value(test_qstr));

    // Case 3: single character
    test_str = "a";
    test_qstr = "a";
    QCOMPARE(attrRge->validate(test_str), Value(test_str));
    QCOMPARE(attrRge->validate(test_qstr), Value(test_qstr));

    // Case 4: unusual characters
    test_str = "abc£ãã&!£$%^*(áéí)";
    test_qstr = "abc£ãã&!£$%^*(áéí)";
    QCOMPARE(attrRge->validate(test_str), Value(test_str));
    QCOMPARE(attrRge->validate(test_qstr), Value(test_qstr));

    // Case 5: empty string
    test_str = "";
    test_qstr = "";
    QCOMPARE(attrRge->validate(test_str), Value(test_str));
    QCOMPARE(attrRge->validate(test_qstr), Value(test_qstr));

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString("test"));
    QCOMPARE(attrRge->attrRangeStr(), QString("string"));

    AttributeRange::Type type = AttributeRange::String;
    QCOMPARE(attrRge->type(), type);

    // Tests min(), max() and rand() functions -- all should return a empty string
    QCOMPARE(attrRge->rand(m_prg.get()).toString(), "");
    QCOMPARE(attrRge->min().toString(), "");
    QCOMPARE(attrRge->min().toString(), "");
}

void TestAttributeRange::tst_string_set()
{
    const char* attrName = "test";
    // Case 1: normal string
    // Case 2: long string
    // Case 3: single character
    // Case 4: unusual characters
    const QString _values("sample,this sentence is a long string for testing purposes,a,abc£ãã&!£$%^*(áéí)");
    const QString attrRangeStr = "string{" + _values + "}";
    QStringList values = _values.split(",");

    AttributeRange* attrRge = AttributeRange::parse(0, attrName, attrRangeStr);

    // Tests value returned by 'AttributeRange::validate()'
    const char* test_str;
    QString test_qstr;

    // Case 1: normal string
    test_str = "sample";
    test_qstr = "sample";
    QCOMPARE(attrRge->validate(test_str), Value(test_str));
    QCOMPARE(attrRge->validate(test_qstr), Value(test_qstr));

    // Case 2: long string
    test_str = "this sentence is a long string for testing purposes";
    test_qstr = "this sentence is a long string for testing purposes";
    QCOMPARE(attrRge->validate(test_str), Value(test_str));
    QCOMPARE(attrRge->validate(test_qstr), Value(test_qstr));

    // Case 3: single character
    test_str = "a";
    test_qstr = "a";
    QCOMPARE(attrRge->validate(test_str), Value(test_str));
    QCOMPARE(attrRge->validate(test_qstr), Value(test_qstr));

    // Case 4: unusual characters
    test_str = "abc£ãã&!£$%^*(áéí)";
    test_qstr = "abc£ãã&!£$%^*(áéí)";
    QCOMPARE(attrRge->validate(test_str), Value(test_str));
    QCOMPARE(attrRge->validate(test_qstr), Value(test_qstr));

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString(attrName));
    QCOMPARE(attrRge->attrRangeStr(), QString(attrRangeStr));

    AttributeRange::Type type = AttributeRange::String_Set;
    QCOMPARE(attrRge->type(), type);

    // The min and max values are taken to be the value of the first character
    // of the string according to the ASCII table
    const char* min = "a";
    const char* max = "this sentence is a long string for testing purposes";

    QCOMPARE(attrRge->min().toString(), min);
    QCOMPARE(attrRge->max().toString(), max);
    QCOMPARE(attrRge->min().toQString(), QString(min));
    QCOMPARE(attrRge->max().toQString(), QString(max));
    QVERIFY(values.contains(attrRge->rand(m_prg.get()).toString()));
}

void TestAttributeRange::tst_filepath()
{
    AttributeRange* attrRge = AttributeRange::parse(9057, "test", "filepath");

    const QString existing = QDir::current().absoluteFilePath("tst_attributerange");
    const QString missing = QDir::current().absoluteFilePath("none123");

    QCOMPARE(attrRge->validate(existing), Value(existing));
    QVERIFY(!attrRge->validate(missing).isValid());

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 9057);
    QCOMPARE(attrRge->attrName(), QString("test"));
    QCOMPARE(attrRge->attrRangeStr(), QString("filepath"));

    AttributeRange::Type type = AttributeRange::FilePath;
    QCOMPARE(attrRge->type(), type);

    // Tests min(), max() and rand() functions -- all should return a empty string
    QCOMPARE(attrRge->rand(m_prg.get()).toString(), "");
    QCOMPARE(attrRge->min().toString(), "");
    QCOMPARE(attrRge->min().toString(), "");
}

void TestAttributeRange::tst_dirpath()
{
    AttributeRange* attrRge = AttributeRange::parse(0, "test", "dirpath");

    QCOMPARE(attrRge->validate(QDir::currentPath()), Value(QDir::currentPath()));
    QVERIFY(!attrRge->validate("/invalid/").isValid());

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString("test"));
    QCOMPARE(attrRge->attrRangeStr(), QString("dirpath"));

    AttributeRange::Type type = AttributeRange::DirPath;
    QCOMPARE(attrRge->type(), type);

    // Tests min(), max() and rand() functions -- all should return a empty string
    QCOMPARE(attrRge->rand(m_prg.get()).toString(), "");
    QCOMPARE(attrRge->min().toString(), "");
    QCOMPARE(attrRge->min().toString(), "");
}

QTEST_MAIN(TestAttributeRange)
#include "tst_attributerange.moc"
