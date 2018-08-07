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
#include <core/attrsgenerator.h>

namespace evoplex {
class TestAttrsGenerator: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void tst_parseStarCmd();
    void tst_parseHashCmd();

private:
    void _tst_attrs(const SetOfAttributes& res, const Attributes& attrs,  const bool testValues);
    void _tst_mode(const SetOfAttributes& res, const QString& mode, const AttributesScope& ascope, const int& numOfAttrRges);
    void _tst_parseStarCmd(const QString& func, const AttributesScope& attrsScope, const int& sizeOfAG, const Attributes& attrs);
    QString _makeCmd(const QString& func, const Values& values, const QStringList& names, const int sizeOfAG);
    AttributesScope _newAttrsScope(const QStringList& names, const QStringList& attrRges);
};

void TestAttrsGenerator::_tst_attrs(const SetOfAttributes& res, const Attributes& attrs,  const bool testValues)
{
    for (const Attributes& a : res) {
        QCOMPARE(a.names(), attrs.names());
        QCOMPARE(a.size(), attrs.size());

        if (testValues) {
            QCOMPARE(a.values(), attrs.values());
            for (int j = 0; j < attrs.size(); ++j) {
                QCOMPARE(a.value(j), attrs.value(j));
            }
        }
    }
}

void TestAttrsGenerator::_tst_mode(const SetOfAttributes& res, const QString& mode, const AttributesScope& ascope, const int& numOfAttrRges)
{
    if (mode == "min") {
        for (const auto& attrs : res) {
            for (int i = 0; i < attrs.size(); ++i) {
                auto attrRge = ascope.value(attrs.name(i));
                QCOMPARE(attrs.value(i), attrRge->min());
            }
        }
    } else if (mode == "max") {
        for (const auto& attrs : res) {
            for (int i = 0; i < attrs.size(); ++i) {
                auto attrRge = ascope.value(attrs.name(i));
                QCOMPARE(attrs.value(i), attrRge->max());
            }
        }
    } else if (mode == "rand") {
        for (const auto& attrs : res) {
            for (int i = 0; i < attrs.size(); ++i) {
                auto attrRge = ascope.value(attrs.name(i));
                QVERIFY(attrs.value(i) >= attrRge->min());
                QVERIFY(attrs.value(i) <= attrRge->max());
            }
        }
    } else {
        qFatal("the mode is invalid!");
    }
}

AttributesScope TestAttrsGenerator::_newAttrsScope(const QStringList& names, const QStringList& attrRges)
{
    Q_ASSERT(names.size() == attrRges.size());
    AttributesScope attrsScope;
    attrsScope.reserve(names.size());
    for (int i = 0; i  < names.size(); ++i) {
        AttributeRange* a = AttributeRange::parse(i, names.at(i), attrRges.at(i));
        attrsScope.insert(a->attrName(), a);
    }
    return attrsScope;
}

QString TestAttrsGenerator::_makeCmd(const QString& func, const Values& values, const QStringList& names, const int sizeOfAG)
{
    if (func == "value") {
        QString cmd = QString("#%1").arg(Value(sizeOfAG).toQString());

        for (int i = 0; i < values.size(); i++) {
            cmd += QString(";%1_%2_%3").arg(names.at(i)).arg(func).arg(values[i].toQString());
        }
        return cmd;
    } else if (func == "mixed") {
        QStringList cmdList = {
            Value(sizeOfAG).toQString(),
            QString("%1_min").arg(names.at(0)),
            QString("%1_max").arg(names.at(1)),
            QString("%1_rand_123").arg(names.at(2)),
            QString("%1_value_%2").arg(names.at(3)).arg(values[3].toQString())
        };
        return QString("#%1;%2;%3;%4;%5").arg(cmdList.at(0)).arg(cmdList.at(1)).arg(cmdList.at(2)).arg(cmdList.at(3)).arg(cmdList.at(4));
    } else if (func == "min" || func == "max" || func.startsWith("rand")) {
        QString cmd = QString("#%1").arg(Value(sizeOfAG).toQString());

        for (const QString& name : names) {
            cmd += QString(";%1_%2").arg(name).arg(func);
        }
        return cmd;
    } else {
        qFatal("the function is invalid!");
    }
}

void TestAttrsGenerator::_tst_parseStarCmd(const QString& func, const AttributesScope& attrsScope, const int& sizeOfAG, const Attributes& attrs)
{
    const Attributes emptyAttrs;
    const QString cmd = QString("*%1;%2").arg(Value(sizeOfAG).toQString()).arg(func);
    QString error;
    auto agen = AttrsGenerator::parse(attrsScope, cmd, error);

    if (agen) { // If the command is valid (parse did not return a null pointer)
        QCOMPARE(agen->command(), cmd);
        QCOMPARE(agen->size(), sizeOfAG);

        const SetOfAttributes res = agen->create();

        if (attrsScope.isEmpty()) { // Valid * command with empty attrScope
            _tst_attrs(res, emptyAttrs, true);
        } else { // Valid * command with non-empty attrScope
            if (func == "min" || func == "max") {
                _tst_mode(res, func, attrsScope, attrsScope.size());
            } else if (func.split("_").startsWith("rand")) {
                _tst_mode(res, func.split("_").first(), attrsScope, attrsScope.size());
                _tst_attrs(res, attrs, false);
            }
        }
    }
}

void TestAttrsGenerator::tst_parseStarCmd()
{
    const QStringList names = {"test0", "test1", "test2"};
    const QStringList attrRgeStrs = {"int[0,2]", "double[2.3,7.8]", "int{-2,0,2,4,6}"};
    const AttributesScope validAScope = _newAttrsScope(names, attrRgeStrs);
    const AttributesScope emptyAScope;

    Attributes attrs(3);
    for (int i = 0; i < attrs.size(); ++i) {
        attrs.replace(i, names.at(i), 0);
    }

    const QStringList funcs = {
        // valid functions
        "min", "max", "rand_0", "rand_10",
        // invalid functions
        "mim", "mn", "", "rand_-10", "_10", "rand_notInt", "invalid"
    };

    // Sizes <= 0 cause ASSERT failures
//    const int sizes[] = {-100, -1, 0, 1, 3};
    const int sizes[] = {1, 3, 100};

    for (const auto& func : funcs) {
        for (int size : sizes) {
            _tst_parseStarCmd(func, validAScope, size, attrs);
            _tst_parseStarCmd(func, emptyAScope, size, attrs);
        }
    }
}

void TestAttrsGenerator::tst_parseHashCmd()
{
    const int sizeOfAgen = 3;
    const QStringList names = {"test0", "test1", "test2", "test3"};
    const QStringList attrRgeStrs = {"int[0,2]", "double[2.3,7.8]", "int{-2,0,2,4,6}", "double{-2.2, -1.1, 0, 2.3}"};
    const Values values = {Value(1), Value(3.6), Value(4), Value(-1.1)};
    QString error;

    const QStringList funcs = {"min", "max", "rand_123", "value", "mixed"};

    for (const QString& func : funcs) {
       const QString cmd = _makeCmd(func, values, names, sizeOfAgen);
       const AttributesScope attrsScope = _newAttrsScope(names, attrRgeStrs);
       auto agen = AttrsGenerator::parse(attrsScope, cmd, error);

       QCOMPARE(agen->command(), cmd);
       QCOMPARE(agen->size(), sizeOfAgen);

       const SetOfAttributes res = agen->create();

       Attributes attrs(4);

       if (func == "min") {
           for (int i = 0; i < attrs.size(); ++i) {
               attrs.replace(i, names.at(i), attrsScope.value(names.at(i))->min());
           }
           _tst_attrs(res, attrs, true);
       } else if (func == "max") {
           for (int i = 0; i < attrs.size(); ++i) {
               attrs.replace(i, names.at(i), attrsScope.value(names.at(i))->max());
           }
           _tst_attrs(res, attrs, true);
       } else if (func.split("_").first() == "rand") {
           for (int i = 0; i < attrs.size(); ++i) {
               attrs.replace(i, names.at(i), 0);
           }
            _tst_attrs(res, attrs, false);
            _tst_mode(res, "rand", attrsScope, 4);
       } else if (func == "value") {
           for (int i = 0; i < attrs.size(); ++i) {
               attrs.replace(i, names.at(i), values[i]);
           }
           _tst_attrs(res, attrs, true);
       } else if (func == "mixed") {
           for (int i = 0; i < (attrs.size()-1); ++i) {
               attrs.replace(i, names.at(i), 0);
           }
            attrs.replace(3, names.at(3), values[3]);
            _tst_attrs(res, attrs, false);

            for (const Attributes& a : res){
                QCOMPARE(a.value(0), attrsScope.value(names.at(0))->min());
                QCOMPARE(a.value(1), attrsScope.value(names.at(1))->max());
                QVERIFY(a.value(2) <= attrsScope.value(names.at(2))->max());
                QVERIFY(a.value(2) >= attrsScope.value(names.at(2))->min());
                QCOMPARE(a.value(3), values[3]);
            }
       }
    }
}

} // evoplex
QTEST_MAIN(evoplex::TestAttrsGenerator)
#include "tst_attrsgenerator.moc"
