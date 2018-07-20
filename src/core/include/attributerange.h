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

#include <vector>
#include <QHash>

#include "prg.h"
#include "value.h"

namespace evoplex {

class AttributeRange;
using AttributesScope = QHash<QString, AttributeRange*>;

class AttributeRangeInterface
{
public:
    virtual ~AttributeRangeInterface() = default;
    virtual const Value& min() const = 0;
    virtual const Value& max() const = 0;
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
        DirPath,
        FilePath
    };

    // an attrRangeStr can be:
    //   - "bool"               // a boolean
    //   - "dirpath"            // a string containing a valid dirpath (use forward slashes)
    //   - "filepath"           // a string containing a valid filepath (use forward slashes)
    //   - "string"             // a string
    //   - "string{aaa,bbb}"    // set of strings
    //   - "int[min,max]"       // integers from min to max (including min and max)
    //   - "int{1,2,3}"         // set of integers
    //   - "double[min,max]     // doubles from min to max (including min and max)
    //   - "double{1.1,1.2}     // set of doubles
    //   * you can use 'max' to take the maximum value for the type
    //   * do NOT add spaces before/after the commas
    static AttributeRange* parse(int attrId, const QString& attrName,
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

protected:
    const int m_id;
    const QString m_attrName;
    const Type m_type;
    QString m_attrRangeStr;

    explicit AttributeRange(int id, const QString& attrName, Type type);

private:
    // assume that attrRangeStr is equal to 'int{ }' or 'double{ }'
    static AttributeRange* setOfValues(QString attrRangeStr, const int id,
                                       const QString& attrName);

    // assume that attrRangeStr is equal to 'int[min,max]' or 'double[min,max]'
    static AttributeRange* intervalOfValues(QString attrRangeStr, const int id,
                                            const QString& attrName);
};

class SingleValue : public AttributeRange
{
public:
    explicit SingleValue();
    explicit SingleValue(int id, const QString& attrName, Type type,
                         const Value& validValue);

    ~SingleValue() override = default;

    inline const Value& min() const override;
    inline const Value& max() const override;
    inline Value rand(PRG*) const override;

private:
    Value m_validValue;
};

class IntervalOfValues : public AttributeRange
{
public:
    IntervalOfValues(int id, const QString& attrName, Type type,
                     const Value &min, const Value &max);

    ~IntervalOfValues() override = default;

    inline const Value& min() const override;
    inline const Value& max() const override;
    inline Value rand(PRG* prg) const override;

private:
    Value m_min;
    Value m_max;

    Value (evoplex::IntervalOfValues::*f_rand)(PRG*) const;
    inline Value randD(PRG* prg) const;
    inline Value randI(PRG* prg) const;
};

class SetOfValues : public AttributeRange
{
public:
    SetOfValues(int id, const QString& attrName, Type type, Values values);

    ~SetOfValues() override = default;

    // returns the smallest element in the set
    inline const Value& min() const override;
    // returns the largest element in the set
    inline const Value& max() const override;
    inline Value rand(PRG* prg) const override;

    inline const Values& values() const;

private:
    Values m_values;
    Value m_min;
    Value m_max;
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

/***********************/

inline const Value& SingleValue::min() const
{ return m_validValue; }

inline const Value& SingleValue::max() const
{ return m_validValue; }

inline Value SingleValue::rand(PRG*) const
{ return m_validValue; }

/***********************/

inline const Value& IntervalOfValues::min() const
{ return m_min; }

inline const Value& IntervalOfValues::max() const
{ return m_max; }

inline Value IntervalOfValues::rand(PRG* prg) const
{ return (this->*f_rand)(prg); }

inline Value IntervalOfValues::randD(PRG* prg) const
{ return prg->randD(m_min.toDouble(), m_max.toDouble()); }

inline Value IntervalOfValues::randI(PRG* prg) const
{ return prg->randI(m_min.toInt(), m_max.toInt()); }

/***********************/

inline const Value& SetOfValues::min() const
{ return m_min; }

inline const Value& SetOfValues::max() const
{ return m_max; }

inline Value SetOfValues::rand(PRG* prg) const
{ return m_values.at(prg->randI(m_values.size()-1)); }

inline const Values& SetOfValues::values() const
{ return m_values; }

} // evoplex
#endif // ATTRIBUTE_RANGE_H
