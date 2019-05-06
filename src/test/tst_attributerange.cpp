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
    void tst_invalidCases();

private:
    void _tst_string(bool acceptEmpty);

    void _tst_singleValue(AttributeRangePtr attrRge) const;

    void _tst_validity(AttributeRangePtr attrRge,
            const QString& attrRangeStr, AttributeRange::Type type) const;

    void _tst_parserInputs(AttributeRangePtr attrRge, const QString& attrRangeStr,
                           AttributeRange::Type type, const Validate& vals);

    void _tst_validate(const std::vector<std::pair<QString, Value>>& values);

private:
    std::unique_ptr<PRG> m_prg;
    int m_attrId;
    QString m_attrName;

    // convenient struct to test intervals and sets of ints and doubles
    template<class T>
    struct RgeInput {
        QString expRgeStr, inRgeStr;
        std::vector<T> vals;
        Value min, max, mid, valNotIn;
        Value pMid, nMid, pMax, nMin;
        AttributeRange::Type type;
        RgeInput(QString in, const std::vector<T>& vs, Value valNotIn, AttributeRange::Type t)
            : inRgeStr(in), vals(vs), valNotIn(valNotIn), type(t)
        {
            // build expected attrRange string
            expRgeStr = std::is_same<T, int>::value ? "int" : "double";
            const char sep = in.contains("{") ? '{' : '[';
            expRgeStr += sep;
            for (T v : vals) { expRgeStr += Value(v).toQString() + ","; }
            expRgeStr.remove(expRgeStr.size()-1,1);
            expRgeStr += (sep+2);
            // get expected min, max and mid values
            if (sep == '[') {
                T _fst, _mid, _lst;
                min = _fst = vals.front();
                max = _lst = vals.back();
                mid = _mid = vals.front()+std::abs((vals.back()-vals.front())/2);
                // expected pMid, nMid, pMax and nMin
                pMid = _mid-1 < _fst ? _lst : _mid-1;
                nMid = _mid+1 > _lst ? _fst : _mid+1;
                pMax = _lst-1 < _fst ? _lst : _lst-1;
                nMin = _fst+1 > _lst ? _fst : _fst+1;
            } else {
                const int _fst = 0;
                const int _mid = vals.size() / 2;
                const int _lst = vals.size()-1;
                auto r = std::minmax_element(vals.begin(), vals.end());
                min = *r.first;
                max = *r.second;
                mid = vals[_mid];
                // expected pMid, nMid, pMax and nMin
                pMid = vals[_mid-1 < _fst ? _lst : _mid-1];
                nMid = vals[_mid+1 > _lst ? _fst : _mid+1];
                pMax = vals[_lst-1 < _fst ? _lst : _lst-1];
                nMin = vals[_fst+1 > _lst ? _fst : _fst+1];
            }
        }
    };

    template<class T>
    void _tst_range(RgeInput<T> in) const;
};

void TestAttributeRange::initTestCase()
{
    m_prg = std::unique_ptr<PRG>(new PRG(123));
    m_attrId = 85462;
    m_attrName = "test";
}

void TestAttributeRange::_tst_validity(AttributeRangePtr attrRge,
        const QString& attrRangeStr, AttributeRange::Type type) const
{
    QVERIFY(attrRge->isValid());
    QCOMPARE(attrRge->id(), m_attrId);
    QCOMPARE(attrRge->attrName(), m_attrName);
    QCOMPARE(attrRge->attrRangeStr(), attrRangeStr);
    QCOMPARE(attrRge->type(), type);
}

void TestAttributeRange::_tst_parserInputs(AttributeRangePtr attrRge,
        const QString& attrRangeStr, AttributeRange::Type type, const Validate& vals)
{
    _tst_validity(attrRge, attrRangeStr, type);
    for (auto const& v : vals) {
        QCOMPARE(attrRge->validate(v.first), v.second);
    }
}

template<class T>
void TestAttributeRange::_tst_range(RgeInput<T> in) const
{
    const auto attrRge = AttributeRange::parse(m_attrId, m_attrName, in.inRgeStr);
    // general
    _tst_validity(attrRge, in.expRgeStr, in.type);
    // min() and max()
    QCOMPARE(attrRge->min(), in.min);
    QCOMPARE(attrRge->max(), in.max);
    // rand()
    Value randV = attrRge->rand(m_prg.get());
    QCOMPARE(randV.type(), in.min.type());
    QVERIFY(randV >= in.min);
    QVERIFY(randV <= in.max);
    // validate()
    QCOMPARE(attrRge->validate(in.min.toQString()).toQString(), in.min.toQString());
    QCOMPARE(attrRge->validate(in.max.toQString()).toQString(), in.max.toQString());
    QCOMPARE(attrRge->validate(in.mid.toQString()).toQString(), in.mid.toQString());
    for (T v : in.vals) {
        QCOMPARE(attrRge->validate(Value(v).toQString()).toQString(), Value(v).toQString());
    }
    // prev()
    QCOMPARE(attrRge->prev(in.vals.front()), Value(in.vals.back()));
    QCOMPARE(attrRge->prev(in.mid), in.pMid);
    QCOMPARE(attrRge->prev(in.vals.back()), in.pMax);
    // next()
    QCOMPARE(attrRge->next(in.vals.front()), in.nMin);
    QCOMPARE(attrRge->next(in.mid), in.nMid);
    QCOMPARE(attrRge->next(in.vals.back()), Value(in.vals.front()));
    // invalid case: different type
    Value itype = std::is_same<T, QString>::value ? Value(123) : Value("invalid");
    QCOMPARE(attrRge->validate(itype.toQString()), Value());
    QCOMPARE(attrRge->prev(itype.toQString()), Value());
    QCOMPARE(attrRge->next(itype.toQString()), Value());
    // invalid case: not in the range
    QCOMPARE(attrRge->validate(in.valNotIn.toQString()), Value());
    QCOMPARE(attrRge->prev(in.valNotIn.toQString()), Value());
    QCOMPARE(attrRge->next(in.valNotIn.toQString()), Value());
}

void TestAttributeRange::tst_int_range()
{
    const int min = std::numeric_limits<int>::min();
    const int max = std::numeric_limits<int>::max();

    auto t = AttributeRange::Int_Range;
    std::vector<RgeInput<int>> inputs {
        {"int [0,0]", {0,0}, 10, t},
        {"int[0,1 ]", {0,1}, 12, t},
        {"int[-10, -5]", {-10,-5}, 55, t},
        {"int  [  3  ,  8  ] ", {3,8}, 9, t},
        {"int[min, max]", {min,max}, Value(), t},
        {"int[0,max ]", {0,max}, -10, t},
        {"int [ min,0] ", {min,0}, 10, t},
    };

    for (auto input : inputs) {
        _tst_range<int>(input);
    }
}

void TestAttributeRange::tst_double_range()
{
    const double min = std::numeric_limits<double>::min();
    const double max = std::numeric_limits<double>::max();

    auto t = AttributeRange::Double_Range;
    std::vector<RgeInput<double>> inputs {
        {"double [0.0,0.]", {0.,0.}, .5, t},
        {"double[0,.5 ]", {0.,.5}, -1., t},
        {"double[-10.5, -5.777]", {-10.5,-5.777}, 1.1, t},
        {"double  [  0.3  ,  8  ] ", {0.3,8.}, 9., t},
        {"double[-95.7,87.5]", {-95.7,87.5}, 91.1, t},
        {"double [ min,0] ", {min,0.}, 0.1, t},
        {"double[0,max ]", {0.,max}, -1.1, t},
        {"double[min, max]", {min,max}, Value(), t},

    };

    for (auto input : inputs) {
        _tst_range<double>(input);
    }
}

void TestAttributeRange::tst_int_set()
{
    const int min = std::numeric_limits<int>::min();
    const int max = std::numeric_limits<int>::max();

    auto t = AttributeRange::Int_Set;
    std::vector<RgeInput<int>> inputs {
        {"int{0,1,-5,100,-100}", {0,1,-5,100,-100}, 88, t},
        {"int { 0, 3, -4   ,   8, -5 } ", {0,3,-4,8,-5}, 88, t},
        {"int {3,-1} ", {3,-1}, 88, t},
        {"int {1,2} ", {1,2}, 88, t},
        {"int {3} ", {3}, 88, t},
        {"int{min,max} ", {min, max}, Value(), t},
        {"int{min} ", {min}, Value(), t},
    };

    for (auto i : inputs) {
        _tst_range(i);
    }
}

void TestAttributeRange::tst_double_set()
{
    auto t = AttributeRange::Double_Set;
    std::vector<RgeInput<double>> inputs {
        {"double{0,1.2,-5.5,-95.7,87.5,12}", {0.0,1.2,-5.5,-95.7,87.5,12.0}, 1.1, t},
        {"double{  0.0 , 3.9 , -4.3 , 2.2 } ", {0.0,3.9,-4.3,2.2}, 1.1, t},
        {"double{3.9,-1.2} ", {3.9,-1.2}, 1.1, t},
        {"double{3.9} ", {3.9}, 1.1, t},
        {"double{min} ", {std::numeric_limits<double>::min()}, Value(), t},
    };

    for (auto i : inputs) {
        _tst_range(i);
    }
}

void TestAttributeRange::tst_string_set()
{
    const QStringList strs = {
        // Case 1: normal string
        "sample",

        // Case 2: long string
        "this sentence is a long string for testing purposes",

        // Case 3: single character
        "a",

        // Case 4: unusual characters
        "abc£ãã&!£$%^*(áéí)",

        // Case 5: number
        "123"
    };

    // add cases with whitespaces between commas
    QString arStr;
    for (int i = 3; i < strs.size(); ++i) {
        arStr += QString(",%1").arg(strs[i]);
    }
    arStr = QString("string {  %1,%2  ,  %3  %4 } ")
            .arg(strs[0]).arg(strs[1]).arg(strs[2]).arg(arStr);

    const AttributeRange::Type type = AttributeRange::String_Set;
    auto attrRge = AttributeRange::parse(m_attrId, m_attrName, arStr);

    // Tests if functions work as expected
    Validate vals(strs.size());
    for (auto s : strs) { vals.push_back({s, Value(s)}); }
    auto _arStr = QString("string{%1}").arg(strs.join(","));
    _tst_parserInputs(attrRge, _arStr, type, vals);

    // The min and max values are taken to be the value of the first character
    // of the string according to the ASCII table
    const char* min = "123";
    const char* max = "this sentence is a long string for testing purposes";

    QCOMPARE(attrRge->min().toString(), min);
    QCOMPARE(attrRge->max().toString(), max);
    QCOMPARE(attrRge->min().toQString(), QString(min));
    QCOMPARE(attrRge->max().toQString(), QString(max));
    QVERIFY(strs.contains(attrRge->rand(m_prg.get()).toString()));

    // Tests 'AttributeRange::prev()'
    // sample,this sentence is a long string for testing purposes,a,abc£ãã&!£$%^*(áéí),123
    QCOMPARE(attrRge->prev(true), Value()); // invalid case: different type
    QCOMPARE(attrRge->prev("abc"), Value()); // invalid case: not in the range
    QCOMPARE(attrRge->prev("sample").toString(), "123"); // first
    QCOMPARE(attrRge->prev("this sentence is a long string for testing purposes").toString(), "sample"); // middle
    QCOMPARE(attrRge->prev("123").toString(), "abc£ãã&!£$%^*(áéí)"); // last

    // Tests 'AttributeRange::next()'
    QCOMPARE(attrRge->next(1475.85), Value()); // invalid case: different type
    QCOMPARE(attrRge->next("abc"), Value()); // invalid case: not in the range
    QCOMPARE(attrRge->next("sample").toString(), "this sentence is a long string for testing purposes"); // first
    QCOMPARE(attrRge->next("a").toString(), "abc£ãã&!£$%^*(áéí)"); // middle
    QCOMPARE(attrRge->next("123").toString(), "sample"); // last
}

void TestAttributeRange::tst_bool()
{
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

    const QString attrRangeStr("bool");
    auto attrRge = AttributeRange::parse(m_attrId, m_attrName, attrRangeStr);
    _tst_parserInputs(attrRge, attrRangeStr, AttributeRange::Bool, values);

    // rand(), min() and max()
    QVERIFY(attrRge->rand(m_prg.get()).isBool());
    QCOMPARE(attrRge->min().toBool(), false);
    QCOMPARE(attrRge->max().toBool(), true);
    // prev()
    QCOMPARE(attrRge->prev(true).toBool(), false);
    QCOMPARE(attrRge->prev(false).toBool(), true);
    QCOMPARE(attrRge->prev(1475.85), Value());
    // next()
    QCOMPARE(attrRge->next(true).toBool(), false);
    QCOMPARE(attrRge->next(false).toBool(), true);
    QCOMPARE(attrRge->next("true"), Value());
}

void TestAttributeRange::_tst_singleValue(AttributeRangePtr attrRge) const
{
    // Tests min(), max() and rand() functions -- all should return a empty string
    QCOMPARE(attrRge->rand(m_prg.get()), Value());
    QCOMPARE(attrRge->min(), Value());
    QCOMPARE(attrRge->max(), Value());

    // Tests 'AttributeRange::prev()'
    QCOMPARE(attrRge->prev(1235), Value(1235)); // invalid case: different type
    QCOMPARE(attrRge->prev("abc"), Value("abc"));

    // Tests 'AttributeRange::next()'
    QCOMPARE(attrRge->next(true), Value(true)); // invalid case: different type
    QCOMPARE(attrRge->next("abc"), Value("abc"));
}

void TestAttributeRange::_tst_string(bool acceptEmpty)
{
    const QString attrRangeStr = acceptEmpty ? "string" : "non-empty-string";
    const auto type = acceptEmpty ? AttributeRange::String : AttributeRange::NonEmptyString;
    auto attrRge = AttributeRange::parse(m_attrId, m_attrName, attrRangeStr);

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

    _tst_parserInputs(attrRge, attrRangeStr, type, vals);
    _tst_singleValue(attrRge);

    if (acceptEmpty) {
        QCOMPARE(attrRge->validate(""), Value(""));
    } else {
        QCOMPARE(attrRge->validate(""), Value());
    }
}

void TestAttributeRange::tst_filepath()
{
    const QString attrRangeStr("filepath");
    const auto type = AttributeRange::FilePath;
    auto attrRge = AttributeRange::parse(m_attrId, m_attrName, attrRangeStr);

    const QString existing = QCoreApplication::applicationFilePath();
    const QString missing = QDir::current().absoluteFilePath("none123");

    std::vector<std::pair<QString, Value>> vals = {
        { existing, Value(existing) },
        { missing, Value() }
    };

    _tst_parserInputs(attrRge, attrRangeStr, type, vals);
    _tst_singleValue(attrRge);
}

void TestAttributeRange::tst_dirpath()
{
    const QString attrRangeStr("dirpath");
    const auto type = AttributeRange::DirPath;
    auto attrRge = AttributeRange::parse(m_attrId, m_attrName, attrRangeStr);

    std::vector<std::pair<QString, Value>> vals = {
        { QDir::currentPath(), Value(QDir::currentPath())},
        { "/invalid/", Value()}
    };

    _tst_parserInputs(attrRge, attrRangeStr, type, vals);
    _tst_singleValue(attrRge);
}

void TestAttributeRange::tst_invalidCases()
{
    QStringList attrRgeStrs = {
        "hhh", "stringg", "intt[1,2]", "int[1;2]", " double[1,2]", "int i[1,2]"
        "int{1,2]", "int[1,2}", "int[100]", "double[1.1]", "string[]",
        "int[]", "double[]", "string[]", "int{}", "double{}", "string{}",
        "int[,]", "double[,]", "string[,]", "int{,}", "double{,}", "string{,}",
        "bool[false,true]", "bool{false,true}", "bool ", "true",
        "string{   }", "string{ ,a}", "string{a,a,b}", "string[a,b]",
        "int{1,1}", "int{1, }", "int{a,b}",
        "double{1,1.0000000000000001}", "double{1,}", "double{a,b}",
        "double{min,min}", "double{1,}", "double{a,b}",
        "int[10,0]", "int[max,min]", "int[0,0.5]", "int[0,1.]",
        "double[10,0]", "double[max,min]", "double[0,-0.5]",
    };

    for (auto ars : attrRgeStrs) {
        const auto attrRge = AttributeRange::parse(m_attrId, m_attrName, ars);
        QCOMPARE(attrRge->id(), -1);
        QVERIFY(!attrRge->isValid());
        QCOMPARE(attrRge->attrName(), QString(""));
        QCOMPARE(attrRge->attrRangeStr(), QString(""));
        QCOMPARE(attrRge->type(), AttributeRange::Invalid);
        QCOMPARE(attrRge->validate("abc"), Value());
        QCOMPARE(attrRge->min(), Value());
        QCOMPARE(attrRge->max(), Value());
        QCOMPARE(attrRge->rand(m_prg.get()), Value());
        QCOMPARE(attrRge->prev(123), Value(123));
        QCOMPARE(attrRge->next(123), Value(123));
    }
}

QTEST_MAIN(TestAttributeRange)
#include "tst_attributerange.moc"
