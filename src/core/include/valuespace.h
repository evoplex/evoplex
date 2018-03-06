/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef VALUESPACE_H
#define VALUESPACE_H

#include <QHash>
#include <vector>

#include "prg.h"
#include "value.h"

namespace evoplex
{

class ValueSpace
{
public:
    enum SpaceType {
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

    // space can be:
    //   - "bool"               // a boolean
    //   - "dirpath"            // a string containing a valid dirpath
    //   - "filepath"           // a string containing a valid filepath
    //   - "string"             // a string
    //   - "string{aaa,bbb}"    // set of strings
    //   - "int[min,max]"       // integers from min to max (including min and max)
    //   - "int{1,2,3}"         // set of integers
    //   - "double[min,max]     // doubles from min to max (including min and max)
    //   - "double{1.1,1.2}     // set of doubles
    //   * you can use 'max' to take the maximum value for the type
    static ValueSpace* parse(int id, const QString& attrName, const QString& space);

    virtual const Value& min() const = 0;
    virtual const Value& max() const = 0;
    virtual Value rand(PRG* prg) const = 0;

    // Check if the valueStr belongs to the parameter space.
    // If true, return a valid Value with the correct type
    Value validate(const QString& valueStr) const;

    inline bool isValid() const { return m_type != Invalid; }
    inline int id() const { return m_id; }
    inline const QString& attrName() const { return m_attrName; }
    inline const QString& space() const { return m_space; }
    inline SpaceType type() const { return m_type; }

protected:
    const int m_id;
    const QString m_attrName;
    const SpaceType m_type;
    QString m_space;

    explicit ValueSpace(int id, const QString& attrName, SpaceType type);

private:
    // assume that space is equal to 'int{ }' or 'double{ }'
    // return a vector with all elements with the proper type
    static ValueSpace* setSpace(QString space, const int id, const QString& attrName);

    // assume that space is equal to 'int[min,max]' or 'double[min,max]'
    static ValueSpace* rangeSpace(QString space, const int id, const QString& attrName);
};

class DefaultSpace : public ValueSpace
{
public:
    explicit DefaultSpace();
    explicit DefaultSpace(int id, const QString& attrName, SpaceType type, Value validValue);

    virtual const Value& min() const { return m_validValue; }
    virtual const Value& max() const { return m_validValue; }
    virtual Value rand(PRG*) const { return m_validValue; }

private:
    Value m_validValue;
};

class RangeSpace : public DefaultSpace
{
public:
    RangeSpace(int id, const QString& attrName, SpaceType type, Value min, Value max);

    virtual const Value& min() const { return m_min; }
    virtual const Value& max() const { return m_max; }
    virtual Value rand(PRG* prg) const { return (this->*f_rand)(prg); }

private:
    Value m_min;
    Value m_max;

    Value (evoplex::RangeSpace::*f_rand)(PRG*) const;
    inline Value randD(PRG* prg) const { return prg->randD(m_min.toDouble(), m_max.toDouble()); }
    inline Value randI(PRG* prg) const { return prg->randI(m_min.toInt(), m_max.toInt()); }
};

class SetSpace : public DefaultSpace
{
public:
    SetSpace(int id, const QString& attrName, SpaceType type, Values values);

    virtual const Value& min() const { return m_values.front(); }
    virtual const Value& max() const { return m_values.back(); }
    virtual Value rand(PRG* prg) const { return m_values.at(prg->randI(m_values.size()-1)); }

    inline const Values& values() const { return m_values; }

private:
    Values m_values;
};

typedef QHash<QString, ValueSpace*> AttributesSpace;

} // evoplex
#endif // VALUESPACE_H
