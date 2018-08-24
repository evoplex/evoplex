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
#include <prg.h>
#include <QtTest>

using namespace evoplex;

class TestPRG: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void tst_prg();
    void tst_bernoulli();
    void tst_uniformDouble();
    void tst_uniformInt();
    void tst_uniformSizeT();
    void tst_uniformFloat();
};

void TestPRG::tst_prg()
{
    PRG* prg1 = new PRG(921);
    PRG* prg2 = new PRG(921);

    QCOMPARE(prg1->uniform(), prg2->uniform(0.0, 1.0));
    QCOMPARE(prg1->uniform(), prg2->uniform(1.0));
    QCOMPARE(prg1->uniform(123), prg2->uniform(0, 123));

    double d = prg1->uniform();
    int i = prg1->uniform(123);
    QCOMPARE(d, prg2->uniform());
    QCOMPARE(i, prg2->uniform(123));
    QVERIFY(!qFuzzyCompare(d, prg2->uniform()));
    QVERIFY(i != prg2->uniform(123));

    delete prg1;
    prg1 = new PRG(922);
    QVERIFY(!qFuzzyCompare(d, prg1->uniform()));
    QVERIFY(i != prg1->uniform(123));

    delete prg1;
    delete prg2;

    prg1 = new PRG(0);
    for (int i = 0; i < 1000; ++i) prg1->uniform();
    prg2 = new PRG(0);
    for (int i = 0; i < 1000; ++i) prg2->uniform();
    QCOMPARE(prg1->uniform(), prg2->uniform());
    QCOMPARE(prg1->uniform(10), prg2->uniform(10));

    delete prg1;
    delete prg2;
}

void TestPRG::tst_bernoulli()
{
    auto prg = std::unique_ptr<PRG>(new PRG(0));
    const int size = 1000;

    // p=0.5 cases
    int trues1 = 0;
    int trues2 = 0;
    for (size_t i = 0; i < size; ++i) {
        if (prg->bernoulli()) ++trues1;
        if (prg->bernoulli(.5)) ++trues2;
    }
    QVERIFY(trues1 > 400 && trues1 < 600);
    QVERIFY(trues2 > 400 && trues2 < 600);

    // general cases
    auto tst = [size, &prg](double p) {
        int trues1 = 0;
        for (size_t i = 0; i < size; ++i) {
            if (prg->bernoulli(p)) ++trues1;
        }
        QVERIFY(trues1 > size*(p-0.1)); // 10% margin
        QVERIFY(trues1 < size*(p+0.1));
    };
    for (double p = 0.0; p <= 1.0; p+=0.1) {
        tst(p);
    }
}

void TestPRG::tst_uniformDouble()
{
    PRG* prg = new PRG(0);
    double v, min, max;

    // [0, 1)
    min = 0.0;
    max = 1.0;
    v = prg->uniform(min, max);
    QVERIFY(v >= min && v < max);
    v = prg->uniform();
    QVERIFY(v >= min && v < max);

    // negative
    min = -100.0;
    max = -50.0;
    v = prg->uniform(min, max);
    QVERIFY(v >= min && v < max);

    // same number
    min = 123.45;
    max = min;
    v = prg->uniform(min, max);
    QVERIFY(v == min);

    delete prg;
}

void TestPRG::tst_uniformInt()
{
    PRG* prg = new PRG(0);
    int v, min, max;

    // [0, 10)
    min = 0;
    max = 10;
    v = prg->uniform(min, max);
    QVERIFY(v >= min && v <= max);
    v = prg->uniform(max);
    QVERIFY(v >= min && v <= max);

    // negative
    min = -100;
    max = -50;
    v = prg->uniform(min, max);
    QVERIFY(v >= min && v <= max);

    // same number
    min = 123;
    max = min;
    v = prg->uniform(min, max);
    QVERIFY(v == min);

    delete prg;
}

void TestPRG::tst_uniformSizeT()
{
    PRG* prg = new PRG(0);
    size_t v, min, max;

    // [0, 10)
    min = 0;
    max = 10;
    v = prg->uniform(min, max);
    QVERIFY(v >= min && v <= max);
    v = prg->uniform(max);
    QVERIFY(v >= min && v <= max);

    // same number
    min = 123;
    max = min;
    v = prg->uniform(min, max);
    QVERIFY(v == min);

    // large numbers
    min = UINT64_MAX - 10;
    max = UINT64_MAX;
    v = prg->uniform(min, max);
    QVERIFY(v >= min && v <= max);
    v = prg->uniform(max);
    QVERIFY(v >= 0 && v <= max);

    delete prg;
}

void TestPRG::tst_uniformFloat()
{
    PRG* prg = new PRG(0);
    float v, min, max;

    // [0, 10)
    min = 0;
    max = 10;
    v = prg->uniform(min, max);
    QVERIFY(v >= min && v <= max);
    v = prg->uniform(max);
    QVERIFY(v >= min && v <= max);

    // negative
    min = -100;
    max = -50;
    v = prg->uniform(min, max);
    QVERIFY(v >= min && v <= max);

    // same number
    min = 123;
    max = min;
    v = prg->uniform(min, max);
    QVERIFY(v == min);

    delete prg;
}
QTEST_MAIN(TestPRG)
#include "tst_prg.moc"
