/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef VALUESPACE_H
#define VALUESPACE_H

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

        Int_Interval,
        Int_Set,
        Double_Interval,
        Double_Set,

        Bool,
        String,
        DirPath,
        FilePath
    };

    static ValueSpace* parse(int id, const QString& attrName, QString space);

    explicit ValueSpace();
    explicit ValueSpace(int id, const QString& attrName, const QString& space, SpaceType type, Value validValue);
    virtual ~ValueSpace() {}

    inline bool isValid() const { return m_type != Invalid; }

    inline int id() const { return m_id; }
    inline const QString& attrName() const { return m_attrName; }
    inline const QString& space() const { return m_space; }
    inline Value validValue() const { return m_validValue; }

    Value validate(const QString& valueStr);

private:
    const int m_id;
    const QString m_attrName;
    const QString m_space;
    const SpaceType m_type;

    Value m_validValue;
};

class IntervalSpace : public ValueSpace
{
public:
    IntervalSpace(int id, const QString& attrName, const QString& space, SpaceType type, Value min, Value max)
        : ValueSpace(id, attrName, space, type, min), m_min(min), m_max(max) {}

    virtual ~IntervalSpace() {}

    Value rand(PRG* prg);
    inline Value min() const { return m_min; }
    inline Value max() const { return m_max; }

private:
    Value m_min;
    Value m_max;
};

class SetSpace : public ValueSpace
{
public:
    SetSpace(int id, const QString& attrName, const QString& space, SpaceType type, Values values)
        : ValueSpace(id, attrName, space, type, values.front()), m_values(values) {}

    virtual ~SetSpace() {}

    Value rand(PRG* prg);
    inline Values values() const { return m_values; }

private:
    Values m_values;
};

typedef QHash<QString, ValueSpace*> AttributesSpace;

} // evoplex
#endif // VALUESPACE_H
