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
#include <attributes.h>

using namespace evoplex;

class TestAttributes: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void tst_empty();
    void tst_resize();
    void tst_replace();
    void tst_push_back();
    void tst_setValue();

private: // auxiliary functions
    void _tst_empty(Attributes a);
    void _tst_resize(Attributes a, const int kSize);
};

void TestAttributes::_tst_empty(Attributes a)
{
    QCOMPARE(a.size(), 0);
    QVERIFY(a.isEmpty());

    QCOMPARE(a.indexOf("abc"), -1);
    QCOMPARE(a.indexOf(QString("abc")), -1);

    QVERIFY(!a.contains("abc"));
    QVERIFY(!a.contains(QString("abc")));

    QVERIFY_EXCEPTION_THROWN(a.replace(0, "a", 1), std::out_of_range);

    QVERIFY(a.names().empty());
    QVERIFY_EXCEPTION_THROWN(a.name(0), std::out_of_range);

    QVERIFY(a.values().empty());
    QVERIFY_EXCEPTION_THROWN(a.setValue(0, 'a'), std::out_of_range);
    QVERIFY_EXCEPTION_THROWN(a.value(0), std::out_of_range);
    QVERIFY_EXCEPTION_THROWN(a.value("a"), std::out_of_range);
    QVERIFY_EXCEPTION_THROWN(a.value(QString("a")), std::out_of_range);

    QCOMPARE(a.value(0, 123), Value(123));
    QCOMPARE(a.value("abc", 123), Value(123));
    QCOMPARE(a.value(QString('a'), 123), Value(123));
}

// Tests if an empty 'Attributes' is really empty.
void TestAttributes::tst_empty()
{
    Attributes a1;     // empty constructor
    Attributes a2(0);  // forcing size zero
    _tst_empty(a1);
    _tst_empty(a2);
}

void TestAttributes::_tst_resize(Attributes a, const int kSize)
{
    QCOMPARE(a.size(), kSize);
    QVERIFY(!a.isEmpty());

    QCOMPARE(a.indexOf(""), 0);
    QVERIFY(a.contains(""));

    QVERIFY_EXCEPTION_THROWN(a.name(kSize), std::out_of_range);
    QVERIFY_EXCEPTION_THROWN(a.name(-1), std::out_of_range);
    QCOMPARE(a.names().size(), size_t(kSize));
    for (const QString& name : a.names()) {
        QCOMPARE(name, QString(""));
    }

    QVERIFY_EXCEPTION_THROWN(a.value(kSize), std::out_of_range);
    QVERIFY_EXCEPTION_THROWN(a.value(-1), std::out_of_range);
    QVERIFY_EXCEPTION_THROWN(a.value("a"), std::out_of_range);
    QVERIFY_EXCEPTION_THROWN(a.value(QString("a")), std::out_of_range);
    QCOMPARE(a.values().size(), size_t(kSize));
    for (const Value& value : a.values()) {
        QVERIFY(!value.isValid());
    }
}

// Tests if 'Attributes::resize()' works as expected.
void TestAttributes::tst_resize()
{
    // resize in the construction
    Attributes a1(3);
    _tst_resize(a1, 3);

    // init empty and resize later
    Attributes a2;
    a2.resize(100);
    _tst_resize(a2, 100);

    // resize to 0 again, it should be empty now
    a2.resize(0);
    _tst_empty(a2);
}

void TestAttributes::tst_replace()
{
    // TO DO!
}

void TestAttributes::tst_push_back()
{
    // TO DO!
}

void TestAttributes::tst_setValue()
{
    // TO DO!
}

QTEST_MAIN(TestAttributes)
#include "tst_attributes.moc"
