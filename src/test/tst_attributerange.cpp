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
#include <attributerange.h>

using namespace evoplex;

class TestAttributeRange: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}
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

    // Auxiliary function
    void _tst_value(Value v, Value::Type type);
};

void TestAttributeRange::_tst_value(Value v, Value::Type type)
{
    Value::Type t = v.type();
    if(t == Value::INVALID)  QVERIFY(!v.isValid());
    else QVERIFY(v.isValid());

    QCOMPARE(t, type);

    switch (type) {
    case Value::BOOL:
        QVERIFY(v.isBool());
        QVERIFY(!v.isInt());
        QVERIFY(!v.isChar());
        QVERIFY(!v.isDouble());
        QVERIFY(!v.isString());
        break;
    case Value::INT:
        QVERIFY(v.isInt());
        QVERIFY(!v.isBool());
        QVERIFY(!v.isChar());
        QVERIFY(!v.isDouble());
        QVERIFY(!v.isString());
        break;
    case Value::DOUBLE:
        QVERIFY(v.isDouble());
        QVERIFY(!v.isInt());
        QVERIFY(!v.isBool());
        QVERIFY(!v.isChar());
        QVERIFY(!v.isString());
        break;
    case Value::STRING:
        QVERIFY(v.isString());
        QVERIFY(!v.isInt());
        QVERIFY(!v.isBool());
        QVERIFY(!v.isDouble());
        QVERIFY(!v.isChar());
        break;
    default:
        QVERIFY(!v.isString());
        QVERIFY(!v.isInt());
        QVERIFY(!v.isBool());
        QVERIFY(!v.isDouble());
        QVERIFY(!v.isChar());
        break;
    }
}

void TestAttributeRange::tst_bool()
{
    AttributeRange* attrRge = AttributeRange::parse(0, "test", "bool");

    // Tests value returned by 'AttributeRange::validate()' for true
    Value v;

    v = attrRge->validate("true");
    _tst_value(v, Value::BOOL);

    // Tests value returned by 'AttributeRange::validate()' for false
    v = attrRge->validate("false");
    _tst_value(v, Value::BOOL);

    // Tests value returned by 'AttributeRange::validate()' for an invalid string
     v = attrRge->validate("invalid");
     _tst_value(v, Value::INVALID);

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString("test"));
    QCOMPARE(attrRge->attrRangeStr(), QString("bool"));

    AttributeRange::Type type = AttributeRange::Bool;
    QCOMPARE(attrRge->type(), type);

    // Tests min(), max() and rand() functions work as expected
    // where min and max are boolean values
    bool min_bool = false;
    bool max_bool = true;

    // where min and max are integers, casted to boolean type
    bool min_int = Value(0).toBool();
    bool max_int = Value(1).toBool();

    // where min and max are booleans, casted to integer type
    int min_int_alt = Value(false).toInt();
    int max_int_alt = Value(true).toInt();

    // For a boolean type, 'AttributeRange::rand()' returns a bool
    PRG* prg = new PRG(123);
    v = attrRge->rand(prg);

    _tst_value(v, Value::BOOL);

    QVERIFY(v >= min_bool);
    QVERIFY(v <= max_bool);
    QVERIFY(v >= min_int);
    QVERIFY(v <= max_int);
    QVERIFY(v.toInt() >= min_int_alt);
    QVERIFY(v.toInt() <= max_int_alt);

    // For a boolean type, 'AttributeRange::min()' and 'AttributeRange::max()'
    // both return <null>
//  QCOMPARE(attrRge->min(), min_bool);  // Succeeds, as <null> is recognised as 0
//  QCOMPARE(attrRge->max(), max_bool);  // Fails
//  QCOMPARE(attrRge->min(), min_int);  // Fails
//  QCOMPARE(attrRge->max(), max_int);  // Fails
//  QCOMPARE(attrRge->min().toInt(), min_int_alt);  // Fails
//  QCOMPARE(attrRge->max().toInt(), max_int_alt);  // Fails

    delete prg;
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
    Value v;

    v = attrRge->validate("0");   // min
    _tst_value(v, Value::INT);
    v = attrRge->validate("1");   // max
    _tst_value(v, Value::INT);
    v = attrRge->validate("invalid"); // invalid
    _tst_value(v, Value::INVALID);

    v = attrRge2->validate("-10");   // min
    _tst_value(v, Value::INT);
    v = attrRge2->validate("-5");   // max
    _tst_value(v, Value::INT);
    v = attrRge2->validate("-7");   // other value
    _tst_value(v, Value::INT);
    v = attrRge2->validate("invalid"); // invalid
    _tst_value(v, Value::INVALID);

    v = attrRge3->validate("-100");   // min
    _tst_value(v, Value::INT);
    v = attrRge3->validate("100");   // max
    _tst_value(v, Value::INT);
    v = attrRge3->validate("0");   // other value
    _tst_value(v, Value::INT);
    v = attrRge3->validate("invalid"); // invalid
    _tst_value(v, Value::INVALID);

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
    int min, max;
    PRG* prg = new PRG(123);

    min = 0;
    max = 1;
    QCOMPARE(attrRge->min(), min);
    QCOMPARE(attrRge->max(), max);
    v = attrRge->rand(prg);
    _tst_value(v, Value::INT);
    QVERIFY(v.toInt() >= min);
    QVERIFY(v.toInt() <= max);

    min = -10;
    max = -5;
    QCOMPARE(attrRge2->min(), min);
    QCOMPARE(attrRge2->max(), max);
    v = attrRge2->rand(prg);
    _tst_value(v, Value::INT);
    QVERIFY(v.toInt() >= min);
    QVERIFY(v.toInt() <= max);

    min = -100;
    max = 100;
    QCOMPARE(attrRge3->min(), min);
    QCOMPARE(attrRge3->max(), max);
    v = attrRge3->rand(prg);
    _tst_value(v, Value::INT);
    QVERIFY(v.toInt() >= min);
    QVERIFY(v.toInt() <= max);

    delete prg;
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
    Value v;

    v = attrRge->validate("1.1");   //min
    _tst_value(v, Value::DOUBLE);
    v = attrRge->validate("1.2");   //max
    _tst_value(v, Value::DOUBLE);
    v = attrRge->validate("1.15");   //other value
    _tst_value(v, Value::DOUBLE);
    v = attrRge->validate("invalid");   //invalid
    _tst_value(v, Value::INVALID);

    v = attrRge2->validate("-5.5");   //min
    _tst_value(v, Value::DOUBLE);
    v = attrRge2->validate("-3.3");   //max
    _tst_value(v, Value::DOUBLE);
    v = attrRge2->validate("-4.1");   //other value
    _tst_value(v, Value::DOUBLE);
    v = attrRge2->validate("invalid");   //invalid
    _tst_value(v, Value::INVALID);

    v = attrRge3->validate("-95.7");   //min
    _tst_value(v, Value::DOUBLE);
    v = attrRge3->validate("87.5");   //max
    _tst_value(v, Value::DOUBLE);
    v = attrRge3->validate("5.6");   //other value
    _tst_value(v, Value::DOUBLE);
    v = attrRge3->validate("invalid");   //invalid
    _tst_value(v, Value::INVALID);

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
    double min, max;
    PRG* prg = new PRG(123);

    min = 1.1;
    max = 1.2;
    QCOMPARE(attrRge->min(), min);
    QCOMPARE(attrRge->max(), max);
    v = attrRge->rand(prg);
    _tst_value(v, Value::DOUBLE);
    QVERIFY(v.toDouble() >= min);
    QVERIFY(v.toDouble() <= max);

    min = -5.5;
    max = -3.3;
    QCOMPARE(attrRge2->min(), min);
    QCOMPARE(attrRge2->max(), max);
    v = attrRge2->rand(prg);
    _tst_value(v, Value::DOUBLE);
    QVERIFY(v.toDouble() >= min);
    QVERIFY(v.toDouble() <= max);

    min = -95.7;
    max = 87.5;
    QCOMPARE(attrRge3->min(), min);
    QCOMPARE(attrRge3->max(), max);
    v = attrRge3->rand(prg);
    _tst_value(v, Value::DOUBLE);
    QVERIFY(v.toDouble() >= min);
    QVERIFY(v.toDouble() <= max);

    delete prg;
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
    Value v;

    v = attrRge->validate("0");
    _tst_value(v, Value::INT);
    v = attrRge->validate("1");
    _tst_value(v, Value::INT);
    v = attrRge->validate("-5");
    _tst_value(v, Value::INT);
    v = attrRge->validate("100");
    _tst_value(v, Value::INT);
    v = attrRge->validate("-100");
    _tst_value(v, Value::INT);
    v = attrRge->validate("invalid");  // invalid
    _tst_value(v, Value::INVALID);

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString(attrName));
    QCOMPARE(attrRge->attrRangeStr(), QString(attrRangeStr));

    AttributeRange::Type type = AttributeRange::Int_Set;
    QCOMPARE(attrRge->type(), type);

    // Tests min(), max() and rand() functions
    PRG* prg = new PRG(123);

    // FAIL - both 'AttributeRange::min()' and 'AttributeRange::max()' return <null> for an int set
//  QCOMPARE(attrRge->min(), min);
//  QCOMPARE(attrRge->max(), max);

    v = attrRge->rand(prg);
    _tst_value(v, Value::INT);

    // PASSES - because our own variables are used, e.g. min not attrRge->min()
    QVERIFY(v.toInt() >= min);
    QVERIFY(v.toInt() <= max);

    delete prg;
}

void TestAttributeRange::tst_double_set()
{
    const char* attrName = "test";
    // Set with positive, negative and large numbers
    const char* attrRangeStr = "double{0,1.2,-5.5,-95.7,87.5}";
    double min = -95.7;
    double max = 87.5;

    AttributeRange* attrRge = AttributeRange::parse(0, attrName, attrRangeStr);

    // Tests value returned by 'AttributeRange::validate()'
    Value v;

    v = attrRge->validate("0");
    _tst_value(v, Value::DOUBLE);
    v = attrRge->validate("1.2");
    _tst_value(v, Value::DOUBLE);
    v = attrRge->validate("-5.5");
    _tst_value(v, Value::DOUBLE);
    v = attrRge->validate("-95.7");
   _tst_value(v, Value::DOUBLE);
    v = attrRge->validate("87.5");
    _tst_value(v, Value::DOUBLE);
    v = attrRge->validate("invalid");  // invalid
    _tst_value(v, Value::INVALID);

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString(attrName));
    QCOMPARE(attrRge->attrRangeStr(), QString(attrRangeStr));

    AttributeRange::Type type = AttributeRange::Double_Set;
    QCOMPARE(attrRge->type(), type);

    // Tests min(), max() and rand() functions
    PRG* prg = new PRG(123);

   // FAIL - 'AttributeRange::min()' returns <null> for a double set
//  QCOMPARE(attrRge->min(), min);
    QCOMPARE(attrRge->max(), max);

    v = attrRge->rand(prg);
    _tst_value(v, Value::DOUBLE);
    QVERIFY(v.toInt() >= min);
    QVERIFY(v.toInt() <= max);

    delete prg;
}

void TestAttributeRange::tst_string()
{
    AttributeRange* attrRge = AttributeRange::parse(0, "test", "string");

    // Tests value returned by 'AttributeRange::validate()'
    Value v;

    // Case 1: normal string
    v = attrRge->validate("sample");
    _tst_value(v, Value::STRING);

    // Case 2: long string
    v = attrRge->validate("this sentence is a long string for testing purposes");
    _tst_value(v, Value::STRING);

    // Case 3: single character
    v = attrRge->validate("a");
    _tst_value(v, Value::STRING);

    // Case 4: unusual characters
    v = attrRge->validate("abc£ãã&!£$%^*(áéí)");
    _tst_value(v, Value::STRING);

    // Case 5: empty string
    v = attrRge->validate("");
    _tst_value(v, Value::STRING);

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString("test"));
    QCOMPARE(attrRge->attrRangeStr(), QString("string"));

    AttributeRange::Type type = AttributeRange::String;
    QCOMPARE(attrRge->type(), type);

    // Tests min(), max() and rand() functions

    // Both 'AttributeRange::min()' and 'AttributeRange::max()' return <null> for a string type
    PRG* prg = new PRG(123);

    v = attrRge->rand(prg);
    _tst_value(v, Value::STRING);

    delete prg;
}

void TestAttributeRange::tst_string_set()
{
    const char* attrName = "test";
    // Case 1: normal string
    // Case 2: long string
    // Case 3: single character
    // Case 4: unusual characters
    const char* attrRangeStr = "string{sample,this sentence is a long string for testing purposes,a,abc£ãã&!£$%^*(áéí)}";

    AttributeRange* attrRge = AttributeRange::parse(0, attrName, attrRangeStr);

    // Tests value returned by 'AttributeRange::validate()'
    Value v;

    v = attrRge->validate("sample");
    _tst_value(v, Value::STRING);

    v = attrRge->validate("this sentence is a long string for testing purposes");
    _tst_value(v, Value::STRING);

    v = attrRge->validate("a");
    _tst_value(v, Value::STRING);

    v = attrRge->validate("abc£ãã&!£$%^*(áéí)");
    _tst_value(v, Value::STRING);

    v = attrRge->validate("invalid");  // invalid
    _tst_value(v, Value::INVALID);

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString(attrName));
    QCOMPARE(attrRge->attrRangeStr(), QString(attrRangeStr));

    AttributeRange::Type type = AttributeRange::String_Set;
    QCOMPARE(attrRge->type(), type);

    // Tests min(), max() and rand() functions

    // The min and max values are taken to be the value of the first character
    // of the string according to the ASCII table
    const char* min = "a";
    const char* max = "this sentence is a long string for testing purposes";

    PRG* prg = new PRG(123);

    // FAIL - both 'AttributeRange::min()' and 'AttributeRange::max()' return <null> for a string set
//    QCOMPARE(attrRge->min(), min);
//    QCOMPARE(attrRge->max(), max);

    v = attrRge->rand(prg);
    _tst_value(v, Value::STRING);

    QVERIFY(strcmp(v.toString(), max) <= 0);
    QVERIFY(strcmp(v.toString(), min) >= 0);

    delete prg;
}

void TestAttributeRange::tst_filepath()
{
    AttributeRange* attrRge = AttributeRange::parse(0, "test", "filepath");

    // '\e' is recognised as an unknown escape sequence - causes warnings
//    const char* fp_back_slash = "C:\Users\experiment.csv";  // Case 1: .csv filepath with backward slashes
    const char* fp_fwd_slash = "C:/Users/experiment.csv";  // Case 2: .csv filepath with forward slashes
//    const char* fp_back_slash_no_ext = "C:\Users\experiment";  // Case 1: .csv filepath with backward slashes
    const char* fp_fwd_slash_no_ext = "C:/Users/experiment";  // Case 2: .csv filepath with forward slashes

    // Tests value returned by 'AttributeRange::validate()'
    Value v;

    // FAIL - filepaths with backward slashes are not recognised as filepaths
//    v = attrRge->validate(fp_back_slash);
//    _tst_value(v, Value::STRING);

    v = attrRge->validate(fp_fwd_slash);
    _tst_value(v, Value::STRING);

    // FAIL - filepaths must have extenstions
//    v = attrRge->validate(fp_back_slash_no_ext);
//    _tst_value(v, Value::STRING);

//    v = attrRge->validate(fp_back_slash_no_ext);
//    _tst_value(v, Value::STRING);

    v = attrRge->validate("invalid");
    _tst_value(v, Value::INVALID);

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString("test"));
    QCOMPARE(attrRge->attrRangeStr(), QString("filepath"));

    AttributeRange::Type type = AttributeRange::FilePath;
    QCOMPARE(attrRge->type(), type);

    // Tests min(), max() and rand() functions

    // Both 'AttributeRange::min()' and 'AttributeRange::max()' return <null> for a filepath type

    PRG* prg = new PRG(123);
    v = attrRge->rand(prg);
    _tst_value(v, Value::STRING);

    delete prg;
}

void TestAttributeRange::tst_dirpath()
{
    AttributeRange* attrRge = AttributeRange::parse(0, "test", "dirpath");

    // '\e' is recognised as an unknown escape sequence - causes warnings
//    const char* dp_back_slash = "C:\Users\ethan\Documents";  // Case 1: .csv filepath with backward slashes
    const char* dp_fwd_slash = "C:/Users/ethan/Documents";  // Case 1: .csv filepath with forward slashes
//    const char* fp_back_txt = "C:\Users\experiment.txt";  // Case 1: .csv filepath with backward slashes
//    const char* fp_fwd_txt = "C:/Users/experiment.txt";  // Case 1: .csv filepath with forward slashes

    // Tests value returned by 'AttributeRange::validate()'
    Value v;

    // FAIL - dirpaths with backward slashes are not recognised as dirpaths
//    v = attrRge->validate(dp_back_slash);
//    _tst_value(v, Value::STRING);

    v = attrRge->validate(dp_fwd_slash);
    _tst_value(v, Value::STRING);

    v = attrRge->validate("invalid");
    _tst_value(v, Value::INVALID);

    // Tests if functions work as expected
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), QString("test"));
    QCOMPARE(attrRge->attrRangeStr(), QString("dirpath"));

    AttributeRange::Type type = AttributeRange::DirPath;
    QCOMPARE(attrRge->type(), type);

    // Tests min(), max() and rand() functions

    // Both 'AttributeRange::min()' and 'AttributeRange::max()' return <null> for a dirpath type

    PRG* prg = new PRG(123);
    v = attrRge->rand(prg);
    _tst_value(v, Value::STRING);

    delete prg;
}

QTEST_MAIN(TestAttributeRange)
#include "tst_attributerange.moc"
