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

    using Validate = std::vector<std::pair<QString, Value>>;

private slots:
    void initTestCase();
    void cleanupTestCase() {}
    void tst_bool();
    void tst_int_range();
    void tst_double_range();
    void tst_int_set();
    void tst_string_set();
    void tst_double_set();
    void tst_filepath();
    void tst_dirpath();
    void tst_string() { _tst_string(true); }
    void tst_nonEmptyString() { _tst_string(false); }

private:
    void _tst_string(bool acceptEmpty);

    void _tst_parserInputs(AttributeRangePtr attrRge, const QString& attrName,
            const QString& attrRangeStr, AttributeRange::Type type, const Validate& vals);

    void _tst_validate(const std::vector<std::pair<QString, Value>>& values);

private:
    std::unique_ptr<PRG> m_prg;
};

void TestAttributeRange::initTestCase()
{
    m_prg = std::unique_ptr<PRG>(new PRG(123));
}

void TestAttributeRange::_tst_parserInputs(AttributeRangePtr attrRge, const QString& attrName,
        const QString& attrRangeStr, AttributeRange::Type type, const Validate& vals)
{
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), 0);
    QCOMPARE(attrRge->attrName(), attrName);
    QCOMPARE(attrRge->attrRangeStr(), attrRangeStr);
    QCOMPARE(attrRge->type(), type);

    for (auto const& v : vals) {
        QCOMPARE(attrRge->validate(v.first), v.second);
    }
}

void TestAttributeRange::tst_bool()
{
    const QString attrName("test");
    const QString attrRangeStr("bool");
    const AttributeRange::Type type = AttributeRange::Bool;
    auto attrRge = AttributeRange::parse(0, attrName, attrRangeStr);

    std::vector<std::pair<QString, Value>> values = {
        { "True", Value(true) },
        { "true", Value(true) },
        { "TRUE", Value(true) },
        { "TrUe", Value(true) },
        { "false", Value(false) },
        { "FalsE", Value(false) },
        { "FALSE", Value(false) },
        { "trrruuueee", Value() },
        { "8.084", Value() },
        { "falsr", Value() },
        { "false ", Value() },
        { "1", Value(true) },
        { "0", Value(false) }
    };

    _tst_parserInputs(attrRge, attrName, attrRangeStr, type, values);

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
    QCOMPARE(attrRge->max().toBool(), max);

    // Tests 'AttributeRange::prev()'
    QCOMPARE(attrRge->prev(true).toBool(), false);
    QCOMPARE(attrRge->prev(false).toBool(), true);
    QCOMPARE(attrRge->prev(1475.85), Value(1475.85));

    // Tests 'AttributeRange::next()'
    QCOMPARE(attrRge->next(true).toBool(), false);
    QCOMPARE(attrRge->next(false).toBool(), true);
    QCOMPARE(attrRge->next("true"), Value("true"));
}

void TestAttributeRange::tst_int_range()
{
    QStringList attrNames = {"test0", "test1", "test2"};
    // Case 1: regular range
    // Case 2: negative numbers
    // Case 3: large range
    QStringList attrRangeStrs = {"int[0,1]", "int[-10,-5]", "int[-100,100]"};

    const auto attrRge = AttributeRange::parse(0, attrNames[0], attrRangeStrs[0]);
    const auto attrRge2 = AttributeRange::parse(1, attrNames[1], attrRangeStrs[1]);
    const auto attrRge3 = AttributeRange::parse(2, attrNames[2], attrRangeStrs[2]);

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

    // Tests 'AttributeRange::prev()'
    // int[0,1]
    QCOMPARE(attrRge->prev(1475.85), Value(1475.85)); // invalid case: different type
    QCOMPARE(attrRge->prev(2).toInt(), 2); // invalid case: not in the range
    QCOMPARE(attrRge->prev(0).toInt(), 1);
    QCOMPARE(attrRge->prev(1).toInt(), 0);
    // int[-10,-5]
    QCOMPARE(attrRge2->prev(-10).toInt(), -5); // first
    QCOMPARE(attrRge2->prev(-7).toInt(), -8); // middle
    QCOMPARE(attrRge2->prev(-5).toInt(), -6); // last
    // int[-100,100]
    QCOMPARE(attrRge3->prev(-100).toInt(), 100); // first
    QCOMPARE(attrRge3->prev(0).toInt(), -1); // middle
    QCOMPARE(attrRge3->prev(100).toInt(), 99); // last

    // Tests 'AttributeRange::next()'
    // int[0,1]
    QCOMPARE(attrRge->next(1475.85), Value(1475.85)); // invalid case: different type
    QCOMPARE(attrRge->next(2), Value(2)); // invalid case: not in the range
    QCOMPARE(attrRge->next(0).toInt(), 1);
    QCOMPARE(attrRge->next(1).toInt(), 0);
    // int[-10,-5]
    QCOMPARE(attrRge2->next(-10).toInt(), -9); // first
    QCOMPARE(attrRge2->next(-7).toInt(), -6); // middle
    QCOMPARE(attrRge2->next(-5).toInt(), -10); // last
    // int[-100,100]
    QCOMPARE(attrRge3->next(-100).toInt(), -99); // first
    QCOMPARE(attrRge3->next(0).toInt(), 1); // middle
    QCOMPARE(attrRge3->next(100).toInt(), -100); // last
}

void TestAttributeRange::tst_double_range()
{
    QStringList attrNames = {"test0", "test1", "test2"};
    // Case 1: regular range
    // Case 2: negative numbers
    // Case 3: large range
    QStringList attrRangeStrs = {"double[1.1,1.2]", "double[-5.5,-3.3]", "double[-95.7,87.5]"};

    const auto attrRge = AttributeRange::parse(0, attrNames.at(0), attrRangeStrs.at(0));
    const auto attrRge2 = AttributeRange::parse(1, attrNames.at(1), attrRangeStrs.at(1));
    const auto attrRge3 = AttributeRange::parse(2, attrNames.at(2), attrRangeStrs.at(2));

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

    // Tests 'AttributeRange::prev()'
    // double[1.1,1.2]
    QCOMPARE(attrRge->prev("abc"), Value("abc")); // invalid case: different type
    QCOMPARE(attrRge->prev(2.0), Value(2.0)); // invalid case: not in the range
    QCOMPARE(attrRge->prev(1.1).toDouble(), 1.2);
    QCOMPARE(attrRge->prev(1.2).toDouble(), 1.1);
    // double[-5.5,-3.3]
    QCOMPARE(attrRge2->prev(-5.5).toDouble(), -3.3); // first
    QCOMPARE(attrRge2->prev(-4.235).toDouble(), -5.235); // middle
    QCOMPARE(attrRge2->prev(-3.3).toDouble(), -4.3); // last
    // double[-95.7,87.5]
    QCOMPARE(attrRge3->prev(-95.7).toDouble(), 87.5); // first
    QCOMPARE(attrRge3->prev(0.089).toDouble(), -0.911); // middle
    QCOMPARE(attrRge3->prev(87.5).toDouble(), 86.5); // last

    // Tests 'AttributeRange::next()'
    // double[1.1,1.2]
    QCOMPARE(attrRge->next("abc"), Value("abc")); // invalid case: different type
    QCOMPARE(attrRge->next(2.0), Value(2.0)); // invalid case: not in the range
    QCOMPARE(attrRge->next(1.1).toDouble(), 1.2);
    QCOMPARE(attrRge->next(1.2).toDouble(), 1.1);
    // double[-5.5,-3.3]
    QCOMPARE(attrRge2->next(-5.5).toDouble(), -4.5); // first
    QCOMPARE(attrRge2->next(-4.4875299).toDouble(), -3.4875299); // middle
    QCOMPARE(attrRge2->next(-3.3).toDouble(), -5.5); // last
    // double[-95.7,87.5]
    QCOMPARE(attrRge3->next(-95.7).toDouble(), -94.7); // first
    QCOMPARE(attrRge3->next(0.089).toDouble(), 1.089); // middle
    QCOMPARE(attrRge3->next(87.5).toDouble(), -95.7); // last
}

void TestAttributeRange::tst_int_set()
{
    const char* attrName = "test";
    // Set with positive, negative and large numbers
    const char* attrRangeStr = "int{0,1,-5,100,-100}";
    int min = -100;
    int max = 100;

    auto attrRge = AttributeRange::parse(0, attrName, attrRangeStr);

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

    // Tests 'AttributeRange::prev()'
    // int{0,1,-5,100,-100}
    QCOMPARE(attrRge->prev("abc"), Value("abc")); // invalid case: different type
    QCOMPARE(attrRge->prev(2).toInt(), 2); // invalid case: not in the range
    QCOMPARE(attrRge->prev(0).toInt(), -100); // first
    QCOMPARE(attrRge->prev(-5).toInt(), 1); // middle
    QCOMPARE(attrRge->prev(-100).toInt(), 100); // last

    // Tests 'AttributeRange::next()'
    // int{0,1,-5,100,-100}
    QCOMPARE(attrRge->next(1475.85), Value(1475.85)); // invalid case: different type
    QCOMPARE(attrRge->next(2), Value(2)); // invalid case: not in the range
    QCOMPARE(attrRge->next(0).toInt(), 1); // first
    QCOMPARE(attrRge->next(-5).toInt(), 100); // middle
    QCOMPARE(attrRge->next(-100).toInt(), 0); // last
}

void TestAttributeRange::tst_double_set()
{
    const char* attrName = "test";
    // Set with positive, negative and large numbers
    const char* attrRangeStr = "double{0,1.2,-5.5,-95.7,87.5,12}";
    double min = -95.7;
    double max = 87.5;

    auto attrRge = AttributeRange::parse(0, attrName, attrRangeStr);

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

    // Tests 'AttributeRange::prev()'
    // double{0,1.2,-5.5,-95.7,87.5,12}
    QCOMPARE(attrRge->prev("abc"), Value("abc")); // invalid case: different type
    QCOMPARE(attrRge->prev(2.5).toDouble(), 2.5); // invalid case: not in the range
    QCOMPARE(attrRge->prev(0.0).toDouble(), 12.0); // first
    QCOMPARE(attrRge->prev(-95.7).toDouble(), -5.5); // middle
    QCOMPARE(attrRge->prev(12.0).toDouble(), 87.5); // last

    // Tests 'AttributeRange::next()'
    // double{0,1.2,-5.5,-95.7,87.5,12}
    QCOMPARE(attrRge->next(true), Value(true)); // invalid case: different type
    QCOMPARE(attrRge->next(2.5), Value(2.5)); // invalid case: not in the range
    QCOMPARE(attrRge->next(0.0).toDouble(), 1.2); // first
    QCOMPARE(attrRge->next(-95.7).toDouble(), 87.5); // middle
    QCOMPARE(attrRge->next(12.0).toDouble(), 0.0); // last
}

void TestAttributeRange::_tst_string(bool acceptEmpty)
{
    const QString attrName("test");
    const QString attrRangeStr = acceptEmpty ? "string" : "non-empty-string";
    const AttributeRange::Type type = acceptEmpty ? AttributeRange::String : AttributeRange::NonEmptyString;
    auto attrRge = AttributeRange::parse(0, attrName, attrRangeStr);

    // to tests value returned by 'AttributeRange::validate()'
    QStringList _strs = {
        // Case 1: normal string
        "sample",

        // Case 2: long string
        "this sentence is a long string for testing purposes",

        // Case 3: single character
        "a",

        // Case 4: unusual characters
        "abc£ãã&!£$%^*(áéí)",

        // Case 5: number
        "123",

        // Case 6: boolean
        "true",
    };
    Validate vals;
    vals.reserve(static_cast<size_t>(_strs.size()));
    for (const QString& v : _strs) {
        vals.push_back({ v, Value(v) });
    }

    // Tests if functions work as expected
    _tst_parserInputs(attrRge, attrName, attrRangeStr, type, vals);

    // Tests the empty/non-empty case
    if (acceptEmpty) {
        QCOMPARE(attrRge->validate(""), Value(""));
    } else {
        QCOMPARE(attrRge->validate(""), Value());
    }

    // Tests min(), max() and rand() functions -- all should return a empty string
    QCOMPARE(attrRge->rand(m_prg.get()).toString(), "");
    QCOMPARE(attrRge->min().toString(), "");
    QCOMPARE(attrRge->min().toString(), "");
    QCOMPARE(attrRge->min().toQString(), QString());
    QCOMPARE(attrRge->min().toQString(), QString());

    // Tests 'AttributeRange::prev()'
    QCOMPARE(attrRge->prev(1235), Value(1235)); // invalid case: different type
    QCOMPARE(attrRge->prev("abc"), Value("abc"));

    // Tests 'AttributeRange::next()'
    QCOMPARE(attrRge->next(true), Value(true)); // invalid case: different type
    QCOMPARE(attrRge->next("abc"), Value("abc"));
}

void TestAttributeRange::tst_string_set()
{
    // Case 1: normal string
    // Case 2: long string
    // Case 3: single character
    // Case 4: unusual characters
    // Case 5: number
    const QString _values("sample,this sentence is a long string for testing purposes,a,abc£ãã&!£$%^*(áéí),123");

    const QString attrName("test");
    const QString attrRangeStr = "string{" + _values + "}";
    const AttributeRange::Type type = AttributeRange::String_Set;
    auto attrRge = AttributeRange::parse(0, attrName, attrRangeStr);

    Validate vals;
    QStringList values = _values.split(",");
    vals.reserve(static_cast<size_t>(values.size()));
    for (const QString& v : values) {
        vals.push_back({v, Value(v)});
    }

    // Tests if functions work as expected
    _tst_parserInputs(attrRge, attrName, attrRangeStr, type, vals);

    // The min and max values are taken to be the value of the first character
    // of the string according to the ASCII table
    const char* min = "123";
    const char* max = "this sentence is a long string for testing purposes";

    QCOMPARE(attrRge->min().toString(), min);
    QCOMPARE(attrRge->max().toString(), max);
    QCOMPARE(attrRge->min().toQString(), QString(min));
    QCOMPARE(attrRge->max().toQString(), QString(max));
    QVERIFY(values.contains(attrRge->rand(m_prg.get()).toString()));

    // Tests 'AttributeRange::prev()'
    // sample,this sentence is a long string for testing purposes,a,abc£ãã&!£$%^*(áéí),123
    QCOMPARE(attrRge->prev(true), Value(true)); // invalid case: different type
    QCOMPARE(attrRge->prev("abc"), Value("abc")); // invalid case: not in the range
    QCOMPARE(attrRge->prev("sample").toString(), "123"); // first
    QCOMPARE(attrRge->prev("this sentence is a long string for testing purposes").toString(), "sample"); // middle
    QCOMPARE(attrRge->prev("123").toString(), "abc£ãã&!£$%^*(áéí)"); // last

    // Tests 'AttributeRange::next()'
    QCOMPARE(attrRge->next(1475.85), Value(1475.85)); // invalid case: different type
    QCOMPARE(attrRge->next("abc"), Value("abc")); // invalid case: not in the range
    QCOMPARE(attrRge->next("sample").toString(), "this sentence is a long string for testing purposes"); // first
    QCOMPARE(attrRge->next("a").toString(), "abc£ãã&!£$%^*(áéí)"); // middle
    QCOMPARE(attrRge->next("123").toString(), "sample"); // last
}

void TestAttributeRange::tst_filepath()
{
    const QString attrName("test");
    const QString attrRangeStr("filepath");
    const AttributeRange::Type type = AttributeRange::FilePath;
    auto attrRge = AttributeRange::parse(0, attrName, attrRangeStr);

    const QString existing = QCoreApplication::applicationFilePath();
    const QString missing = QDir::current().absoluteFilePath("none123");

    std::vector<std::pair<QString, Value>> vals = {
        { existing, Value(existing) },
        { missing, Value() }
    };

    // Tests if functions work as expected
    _tst_parserInputs(attrRge, attrName, attrRangeStr, type, vals);

    // Tests min(), max() and rand() functions -- all should return a empty string
    QCOMPARE(attrRge->rand(m_prg.get()).toString(), "");
    QCOMPARE(attrRge->min().toString(), "");
    QCOMPARE(attrRge->min().toString(), "");

    // Tests 'AttributeRange::prev()'
    QCOMPARE(attrRge->prev(1235), Value(1235)); // invalid case: different type
    QCOMPARE(attrRge->prev("abc"), Value("abc"));

    // Tests 'AttributeRange::next()'
    QCOMPARE(attrRge->next(true), Value(true)); // invalid case: different type
    QCOMPARE(attrRge->next("abc"), Value("abc"));
}

void TestAttributeRange::tst_dirpath()
{
    const QString attrName("test");
    const QString attrRangeStr("dirpath");
    const AttributeRange::Type type = AttributeRange::DirPath;
    auto attrRge = AttributeRange::parse(0, attrName, attrRangeStr);

    std::vector<std::pair<QString, Value>> vals = {
        { QDir::currentPath(), Value(QDir::currentPath())},
        { "/invalid/", Value()}
    };

    // Tests if functions work as expected
    _tst_parserInputs(attrRge, attrName, attrRangeStr, type, vals);

    // Tests min(), max() and rand() functions -- all should return a empty string
    QCOMPARE(attrRge->rand(m_prg.get()).toString(), "");
    QCOMPARE(attrRge->min().toString(), "");
    QCOMPARE(attrRge->min().toString(), "");

    // Tests 'AttributeRange::prev()'
    QCOMPARE(attrRge->prev(1235), Value(1235)); // invalid case: different type
    QCOMPARE(attrRge->prev("abc"), Value("abc"));

    // Tests 'AttributeRange::next()'
    QCOMPARE(attrRge->next(true), Value(true)); // invalid case: different type
    QCOMPARE(attrRge->next("abc"), Value("abc"));
}

QTEST_MAIN(TestAttributeRange)
#include "tst_attributerange.moc"
