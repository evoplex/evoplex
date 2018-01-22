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

        Int_Interval,
        Double_Interval,

        Int_Set,        
        Double_Set,

        Bool,
        String,
        DirPath,
        FilePath
    };

    static ValueSpace* parse(int id, const QString& attrName, QString space);

    virtual const Value& min() const = 0;
    virtual const Value& max() const = 0;
    virtual Value rand(PRG* prg) const = 0;

    Value validate(const QString& valueStr) const;
    inline bool isValid() const { return m_type != Invalid; }

    inline int id() const { return m_id; }
    inline const QString& attrName() const { return m_attrName; }
    inline const QString& space() const { return m_space; }
    inline SpaceType type() const { return m_type; }

protected:
    const int m_id;
    const QString m_attrName;
    const QString m_space;
    const SpaceType m_type;

    explicit ValueSpace(int id, const QString& attrName, const QString& space, SpaceType type);
};

class DefaultSpace : public ValueSpace
{
public:
    explicit DefaultSpace();
    explicit DefaultSpace(int id, const QString& attrName,
            const QString& space, SpaceType type, Value validValue);

    virtual const Value& min() const { return m_validValue; }
    virtual const Value& max() const { return m_validValue; }
    virtual Value rand(PRG*) const { return m_validValue; }

private:
    Value m_validValue;
};

class IntervalSpace : public DefaultSpace
{
public:
    IntervalSpace(int id, const QString& attrName,
        const QString& space, SpaceType type, Value min, Value max);

    virtual const Value& min() const { return m_min; }
    virtual const Value& max() const { return m_max; }
    virtual Value rand(PRG* prg) const { return (this->*f_rand)(prg); }

private:
    Value m_min;
    Value m_max;

    Value (evoplex::IntervalSpace::*f_rand)(PRG*) const;
    inline Value randD(PRG* prg) const { return prg->randD(m_min.toDouble, m_max.toDouble); }
    inline Value randI(PRG* prg) const { return prg->randI(m_min.toInt, m_max.toInt); }
};

class SetSpace : public DefaultSpace
{
public:
    SetSpace(int id, const QString& attrName, const QString& space, SpaceType type, Values values);

    virtual const Value& min() const { return m_values.front(); }
    virtual const Value& max() const { return m_values.back(); }
    virtual Value rand(PRG* prg) const { return m_values.at(prg->randI(m_values.size())); }

    inline const Values& values() const { return m_values; }

private:
    Values m_values;
};

typedef QHash<QString, ValueSpace*> AttributesSpace;

} // evoplex
#endif // VALUESPACE_H
