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
    void tst_build();
    void tst_compare();
};

void TestValue::tst_build()
{
    Value v;
    QVERIFY(v.type() == Value::INVALID);
    QVERIFY(v.isValid() == false);
    QVERIFY_EXCEPTION_THROWN(v.toQString(), std::invalid_argument);

    v = Value(true);
    QVERIFY(v.type() == Value::BOOL);
    QVERIFY(v.toBool() == true);
    QCOMPARE(v.toQString(), QString("1"));

    v = Value(4.5123788);
    QVERIFY(v.type() == Value::DOUBLE);
    QCOMPARE(v.toDouble(), 4.5123788);
    QCOMPARE(v.toQString(), QString("4.5123788"));

    v = Value(-10);
    QVERIFY(v.type() == Value::INT);
    QCOMPARE(v.toInt(), -10);
    QCOMPARE(v.toQString(), QString("-10"));

    const char* chr = "abc£ãã&";
    QString str("abc£ãã&");
    v = Value(chr);
    QVERIFY(v.type() == Value::STRING);
    QCOMPARE(v.toString(), chr);
    QCOMPARE(v.toQString(), str);

    v = Value(str);
    QVERIFY(v.type() == Value::STRING);
    QCOMPARE(v.toString(), chr);
    QCOMPARE(v.toQString(), str);
}

void TestValue::tst_compare()
{
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

    // compare bool
    v1 = Value(true), v2 = Value(false);
    QCOMPARE(v1 == v2, false);
    QCOMPARE(v1 != v2, true);
    QCOMPARE(v1 >= v2, true);
    QCOMPARE(v1 <= v2, false);
    QCOMPARE(v1 >  v2, true);
    QCOMPARE(v1 <  v2, false);

    // compare char
    v1 = Value('d'), v2 = Value('c');
    QCOMPARE(v1 == v2, false);
    QCOMPARE(v1 != v2, true);
    QCOMPARE(v1 >= v2, true);
    QCOMPARE(v1 <= v2, false);
    QCOMPARE(v1 >  v2, true);
    QCOMPARE(v1 <  v2, false);

    // compare double
    v1 = Value(100.2), v2 = Value(100.1);
    QCOMPARE(v1 == v2, false);
    QCOMPARE(v1 != v2, true);
    QCOMPARE(v1 >= v2, true);
    QCOMPARE(v1 <= v2, false);
    QCOMPARE(v1 >  v2, true);
    QCOMPARE(v1 <  v2, false);

    // compare int
    v1 = Value(100), v2 = Value(50);
    QCOMPARE(v1 == v2, false);
    QCOMPARE(v1 != v2, true);
    QCOMPARE(v1 >= v2, true);
    QCOMPARE(v1 <= v2, false);
    QCOMPARE(v1 >  v2, true);
    QCOMPARE(v1 <  v2, false);

    // compare string
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
