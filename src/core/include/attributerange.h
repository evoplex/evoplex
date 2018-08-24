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

#ifndef ATTRIBUTE_RANGE_H
#define ATTRIBUTE_RANGE_H

#include <functional>
#include <memory>
#include <vector>
#include <QHash>

#include "prg.h"
#include "value.h"

namespace evoplex {

class AttributeRange;
using AttributeRangePtr = std::shared_ptr<AttributeRange>;
using AttributesScope = QHash<QString, AttributeRangePtr>;

class AttributeRangeInterface
{
public:
    virtual ~AttributeRangeInterface() = default;
    virtual Value rand(PRG* prg) const = 0;
};

class AttributeRange : public AttributeRangeInterface
{
public:
    enum Type {
        Invalid,

        Double_Range,
        Int_Range,

        Double_Set,
        Int_Set,
        String_Set,

        Bool,
        String,
        NonEmptyString,
        DirPath,
        FilePath
    };

    // an attrRangeStr can be:
    //   - "bool"               // a boolean
    //   - "dirpath"            // a string containing a valid dirpath (use forward slashes)
    //   - "filepath"           // a string containing a valid filepath (use forward slashes)
    //   - "string"             // a string (empty string is also valid)
    //   - "non-empty-string"   // a non-empty string
    //   - "string{aaa,bbb}"    // set of strings
    //   - "int[min,max]"       // integers from min to max (including min and max)
    //   - "int{1,2,3}"         // set of integers
    //   - "double[min,max]     // doubles from min to max (including min and max)
    //   - "double{1.1,1.2}     // set of doubles
    //   * you can use 'max' to take the maximum value for the type
    //   * do NOT add spaces before/after the commas
    static AttributeRangePtr parse(int attrId, const QString& attrName,
                                   const QString& attrRangeStr);

    ~AttributeRange() override = default;

    // Check if the valueStr belongs to this attribute range.
    // If true, return a valid Value with the correct type
    Value validate(const QString& valueStr) const;

    inline bool isValid() const;
    inline int id() const;
    inline const QString& attrName() const;
    inline const QString& attrRangeStr() const;
    inline Type type() const;
    inline const Value& min() const;
    inline const Value& max() const;

protected:
    const int m_id;
    const QString m_attrName;
    const Type m_type;
    QString m_attrRangeStr;
    Value m_min;
    Value m_max;

    explicit AttributeRange(int id, const QString& attrName, Type type);

private:
    // assume that attrRangeStr is equal to 'int{ }' or 'double{ }'
    static AttributeRangePtr setOfValues(QString attrRangeStr, const int id,
                                         const QString& attrName);

    // assume that attrRangeStr is equal to 'int[min,max]' or 'double[min,max]'
    static AttributeRangePtr intervalOfValues(QString attrRangeStr, const int id,
                                              const QString& attrName);
};

class SingleValue : public AttributeRange
{
public:
    explicit SingleValue();
    explicit SingleValue(int id, const QString& attrName, Type type);

    ~SingleValue() override = default;

    inline Value rand(PRG*) const override;
};

class IntervalOfValues : public AttributeRange
{
public:
    IntervalOfValues(int id, const QString& attrName, Type type,
                     const Value &min, const Value &max);

    ~IntervalOfValues() override = default;

    inline Value rand(PRG* prg) const override;

private:
    std::function<Value(PRG*)> f_rand;
};

class SetOfValues : public AttributeRange
{
public:
    SetOfValues(int id, const QString& attrName, Type type, Values values);

    ~SetOfValues() override = default;

    inline Value rand(PRG* prg) const override;

    inline const Values& values() const;

private:
    Values m_values;
};

/***********************/

inline bool AttributeRange::isValid() const
{ return m_type != Invalid; }

inline int AttributeRange::id() const
{ return m_id; }

inline const QString& AttributeRange::attrName() const
{ return m_attrName; }

inline const QString& AttributeRange::attrRangeStr() const
{ return m_attrRangeStr; }

inline AttributeRange::Type AttributeRange::type() const
{ return m_type; }

inline const Value& AttributeRange::min() const
{ return m_min; }

inline const Value& AttributeRange::max() const
{ return m_max; }

/***********************/

inline Value SingleValue::rand(PRG* prg) const
{ return prg->bernoulli() ? m_max : m_min; }

/***********************/

inline Value IntervalOfValues::rand(PRG* prg) const
{ return f_rand(prg); }

/***********************/

inline Value SetOfValues::rand(PRG* prg) const {
    if (m_values.empty()) return Value();
    return m_values.at(prg->uniform(m_values.size()-1));
}

inline const Values& SetOfValues::values() const
{ return m_values; }

} // evoplex
#endif // ATTRIBUTE_RANGE_H
