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

#include <stdexcept>
#include <QtTest>
#include <value.h>

using namespace evoplex;

class TestValue: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void tst_value();
    void tst_valueBool();
    void tst_valueDouble();
    void tst_valueInt();
    void tst_valueChar();
    void tst_valueString();
};

void TestValue::tst_value(){
    Value v;

    //VALUE TESTS
    //Testing validity
    QVERIFY(v.isValid() == false);

    //Testing type
    QVERIFY(v.type() == Value::INVALID);

    //Testing value
    QVERIFY_EXCEPTION_THROWN(v.toQString(), std::invalid_argument);

    //COMPARISONS
    // same invalid type
    Value v1, v2;
    QVERIFY_EXCEPTION_THROWN(v1 == v2, std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(v1 != v2, std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(v1 >= v2, std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(v1 <= v2, std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(v1 > v2, std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(v1 < v2, std::invalid_argument);

    // different valid types
    v1 = Value(1), v2 = Value(1.0);
    QCOMPARE(v1 == v2, false);
    QCOMPARE(v1 != v2, true);
    QVERIFY_EXCEPTION_THROWN(v1 >= v2, std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(v1 <= v2, std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(v1 > v2, std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(v1 < v2, std::invalid_argument);
}

void TestValue::tst_valueBool(){
    Value v;

    //VALUE TESTS
    //case where v is set to true
    v = Value(true);

    //Testing validity
    QVERIFY(v.isValid() == true);

    //Testing type
    QVERIFY(v.type() == Value::BOOL);

    //Testing value
    QVERIFY(v.toBool() == true);
    QCOMPARE(v.toQString(), QString("1"));

    //case where v is set to false
    v = Value(false);

    //Testing validity
    QVERIFY(v.isValid() == true);

    //Testing type
    QVERIFY(v.type() == Value::BOOL);

    //Testing value
    QVERIFY(v.toBool() == false);

    //COMPARISON TESTS
    Value v1, v2;
    v1 = Value(true), v2 = Value(false);
    QCOMPARE(v1 == v2, false);
    QCOMPARE(v1 != v2, true);
    QCOMPARE(v1 >= v2, true);
    QCOMPARE(v1 <= v2, false);
    QCOMPARE(v1 >  v2, true);
    QCOMPARE(v1 <  v2, false);
}

void TestValue::tst_valueDouble(){
    Value v;

    //VALUE TESTS
    v = Value(4.5123788);

    //Testing validity
    QVERIFY(v.isValid() == true);

    //Testing type
    QVERIFY(v.type() == Value::DOUBLE);

    //Testing value
    QCOMPARE(v.toDouble(), 4.5123788);
    QCOMPARE(v.toQString(), QString("4.5123788"));

    //COMPARISON TESTS
    Value v1, v2;
    v1 = Value(100.2), v2 = Value(100.1);
    QCOMPARE(v1 == v2, false);
    QCOMPARE(v1 != v2, true);
    QCOMPARE(v1 >= v2, true);
    QCOMPARE(v1 <= v2, false);
    QCOMPARE(v1 >  v2, true);
    QCOMPARE(v1 <  v2, false);
}

void TestValue::tst_valueInt(){
    Value v;

    //VALUE TESTS
    v = Value(-10);

    //Testing validity
    QVERIFY(v.isValid() == true);

    //Testing type
    QVERIFY(v.type() == Value::INT);

    //Testing value
    QCOMPARE(v.toInt(), -10);
    QCOMPARE(v.toQString(), QString("-10"));

    //COMPARISON TESTS
    Value v1, v2;
    v1 = Value(100), v2 = Value(50);
    QCOMPARE(v1 == v2, false);
    QCOMPARE(v1 != v2, true);
    QCOMPARE(v1 >= v2, true);
    QCOMPARE(v1 <= v2, false);
    QCOMPARE(v1 >  v2, true);
    QCOMPARE(v1 <  v2, false);
}
void TestValue::tst_valueChar(){
    Value v;

    //VALUE TESTS
    const char* chr = "abc£ãã&";
    v = Value(chr);

    //Testing validity
    QVERIFY(v.isValid() == true);

    //Testing type
    //Verify that it is a string, as chr is a character pointer.
    QVERIFY(v.type() == Value::STRING);

    //Testing value
    QCOMPARE(v.toString(), chr);

    //COMPARISON TESTS
    Value v1, v2;
    v1 = Value('d'), v2 = Value('c');
    QCOMPARE(v1 == v2, false);
    QCOMPARE(v1 != v2, true);
    QCOMPARE(v1 >= v2, true);
    QCOMPARE(v1 <= v2, false);
    QCOMPARE(v1 >  v2, true);
    QCOMPARE(v1 <  v2, false);
}
void TestValue::tst_valueString(){
    Value v;

    //VALUE TESTS
    QString str("abc£ãã&");
    const char* chr = "abc£ãã&";
    v = Value(str);

    //Testing validity
    QVERIFY(v.isValid() == true);

    //Testing type
    QVERIFY(v.type() == Value::STRING);

    //Testing value
    QCOMPARE(v.toString(), chr);
    QCOMPARE(v.toQString(), str);

    //COMPARISON TESTS
    Value v1, v2;
    v1 = Value("abc$"), v2 = Value("iua^sd");
    QCOMPARE(v1 == v2, false);
    QCOMPARE(v1 != v2, true);
    QVERIFY_EXCEPTION_THROWN(v1 >= v2, std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(v1 <= v2, std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(v1 > v2, std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(v1 < v2, std::invalid_argument);
}

QTEST_MAIN(TestValue)
#include "tst_value.moc"
