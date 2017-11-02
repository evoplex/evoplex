/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtTest/QtTest>

class EvoplexTest: public QObject
{
    Q_OBJECT

private slots:
    void toUpper();
};

void EvoplexTest::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

QTEST_MAIN(EvoplexTest)
#include "evoplex.moc"
