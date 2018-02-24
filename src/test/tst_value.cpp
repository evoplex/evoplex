/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

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

    v = Value(true);
    QVERIFY(v.type() == Value::BOOL);
    QVERIFY(v.toBool() == true);

    v = Value(4.5123788);
    QVERIFY(v.type() == Value::DOUBLE);
    QCOMPARE(v.toDouble(), 4.5123788);

    v = Value(-10);
    QVERIFY(v.type() == Value::INT);
    QCOMPARE(v.toInt(), -10);

    v = Value("abc£ãã&");
    QVERIFY(v.type() == Value::STRING);
    QCOMPARE(v.toString(), "abc£ãã&");

    v = Value(QString("abc£ãã&"));
    QVERIFY(v.type() == Value::STRING);
    QCOMPARE(v.toString(), "abc£ãã&");
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
