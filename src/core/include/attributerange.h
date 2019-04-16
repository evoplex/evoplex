/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

/**
 * @brief A common interface for the Attribute Range classes.
 * @see AttributeRange, SingleValue, IntervalOfValues, SetOfValues
 */
class AttributeRangeInterface
{
public:
    //! Destructor.
    virtual ~AttributeRangeInterface() = default;

    /**
     * @brief Gets a random value in the attribute range.
     * @param prg A pointer to a valid PRG.
     */
    virtual Value rand(PRG* prg) const = 0;

    /**
     * @brief Gets the value after @p v.
     * @param v A value in the attribute range.
     * If @p v is the last, it returns the first value in the attrRange.
     * if @p v is not in the attrRange, it returns @p v.
     */
    virtual Value next(const Value& v) const = 0;

    /**
     * @brief Gets the value before @p v.
     * @param v A value in the attribute range.
     * If @p v is the first, it returns the last value in the attrRange.
     * if @p v is not in the attrRange, it returns @p v.
     */
    virtual Value prev(const Value& v) const = 0;
};

/**
 * @brief The AttributeRange base class.
 * @see SingleValue, IntervalOfValues, SetOfValues
 */
class AttributeRange : public AttributeRangeInterface
{
public:
    /**
     * @brief An enum for the attribute range types.
     */
    enum Type {
        Invalid,        //!< Invalid type
        // IntervalOfValues
        Double_Range,   //!< A range of real numbers
        Int_Range,      //!< A range of integers
        Bool,           //!< A boolean
        // SetOfValues
        Double_Set,     //!< A set of real numbers
        Int_Set,        //!< A set of intergers
        String_Set,     //!< A set of strings
        // SingleValue
        String,         //!< A string
        NonEmptyString, //!< A non-empty string
        DirPath,        //!< A directory path
        FilePath,       //!< A file path
    };

    /**
     * @brief Creates an AttributeRange object from a valid @p attrRangeStr string.
     * @param attrId The attribute id.
     * @param attrName The attribute name.
     * @param attrRangeStr The attribute range string.
     * @return Returns a null object if @p attrRangeStr is invalid.
     *
     * <b>The accepted @p 'attrRangeStr' strings are listed in the table below.</b>
     *  Attribute Range    | Description
     *  ------------------ | -------------
     *  "bool"             | a boolean
     *  "dirpath"          | a string containing a valid dirpath (use forward slashes)
     *  "filepath"         | a string containing a valid filepath (use forward slashes)
     *  "string"           | a string (empty string is also valid)
     *  "non-empty-string" | a non-empty string
     *  "string{aaa,bbb}"  | set of strings
     *  "int[min,max]"     | integers from min to max (including min and max)
     *  "int{1,2,3}"       | set of integers
     *  "double[min,max]"  | doubles from min to max (including min and max)
     *  "double{1.1,1.2}"  | set of doubles
     *
     * @note You can use 'max' to take the maximum value for the type.
     *       For instance, 'int[0,max]' corresponds to an integer from
     *       0 to 2147483647, which is the largest integer.
     */
    static AttributeRangePtr parse(int attrId, const QString& attrName, QString attrRangeStr);

    //! Destructor.
    ~AttributeRange() override = default;

    /**
     * @brief Checks if the @p valueStr belongs to this attribute range.
     * @param valueStr A value as a string.
     * @return An empty/invalid Value if @p valueStr is not within this range.
     */
    Value validate(const QString& valueStr) const;

    /**
     * @brief Checks if this AttributeRange is valid.
     * @returns true if it is valid.
     */
    inline bool isValid() const;
    /**
     * @brief Gets the attribute id.
     */
    inline int id() const;
    /**
     * @brief Gets the attribute name.
     */
    inline const QString& attrName() const;
    /**
     * @brief Gets the original attribute's range string.
     * @see AttributeRange::parse
     */
    inline const QString& attrRangeStr() const;
    /**
     * @brief Gets the current attribute's range type.
     */
    inline Type type() const;
    /**
     * @brief Gets the minimum value in the range.
     */
    inline const Value& min() const;
    /**
     * @brief Gets the maximum value in the range.
     */
    inline const Value& max() const;

protected:
    const int m_id;
    const QString m_attrName;
    const Type m_type;
    QString m_attrRangeStr;
    Value m_min;
    Value m_max;

    /**
     * @brief AttributeRange constructor
     * @param id The attribute's id.
     * @param attrName The attribute's name.
     * @param type The attribute's type.
     * @see AttributeRange::parse
     */
    explicit AttributeRange(int id, const QString& attrName, Type type);

private:
    // assume that attrRangeStr is equal to 'int{ }' or 'double{ }'
    static AttributeRangePtr setOfValues(QString attrRangeStr, const int id,
                                         const QString& attrName);

    // assume that attrRangeStr is equal to 'int[min,max]' or 'double[min,max]'
    static AttributeRangePtr intervalOfValues(QString attrRangeStr, const int id,
                                              const QString& attrName);
};

/**
 * @brief The SingleValue class.
 * It handles the 'attribute ranges' defined by a single value,
 * e.g., AttributeRange::String, AttributeRange::NonEmptyString,
 * AttributeRange::DirPath and AttributeRange::FilePath.
 * @see AttributeRange, IntervalOfValues, SetOfValues
 */
class SingleValue : public AttributeRange
{
public:
    /**
     * @brief Constructor.
     * @param id The attribute id.
     * @param attrName The attribute name.
     * @param type The attribute type.
     */
    explicit SingleValue(int id, const QString& attrName, Type type);
    explicit SingleValue();

    //! Destructor.
    ~SingleValue() override = default;

    inline Value rand(PRG*) const override;
    inline Value next(const Value& v) const override;
    inline Value prev(const Value& v) const override;
};

/**
 * @brief The IntervalOfValues class.
 * It handles the 'attribute ranges' defined by an interval of values,
 * e.g., AttributeRange::Double_Range, AttributeRange::Int_Range and
 *       AttributeRange::Bool.
 * @see AttributeRange, SingleValue, SetOfValues
 */
class IntervalOfValues : public AttributeRange
{
public:
    /**
     * @brief Constructor.
     * @param id The attribute id.
     * @param attrName The attribute name.
     * @param type The range type.
     * @param min The smallest value in the range.
     * @param max The largest value in the range.
     * @see AttributeRange::parse
     */
    IntervalOfValues(int id, const QString& attrName, Type type,
                     const Value &min, const Value &max);

    //! Destructor.
    ~IntervalOfValues() override = default;

    inline Value rand(PRG* prg) const override;
    inline Value next(const Value& v) const override;
    inline Value prev(const Value& v) const override;

private:
    std::function<Value(PRG*)> f_rand;
    std::function<Value(const Value&)> f_next;
    std::function<Value(const Value&)> f_prev;
};

/**
 * @brief The SetOfValues class.
 * It handles the 'attribute ranges' defined by a set of values,
 * e.g., AttributeRange::Double_Set, AttributeRange::Int_Set and
 *       AttributeRange::String_Set.
 * @see AttributeRange, SingleValue, IntervalOfValues
 */
class SetOfValues : public AttributeRange
{
public:
    /**
     * @brief Constructor.
     * @param id The attribute id.
     * @param attrName The attribute name.
     * @param type The range type.
     * @param values A vector of Value.
     * @see AttributeRange::parse
     */
    SetOfValues(int id, const QString& attrName, Type type, Values values);

    //! Destructor.
    ~SetOfValues() override = default;

    /**
     * @brief Gets the vector of values.
     */
    inline const Values& values() const;

    inline Value rand(PRG* prg) const override;
    Value next(const Value& v) const override;
    Value prev(const Value& v) const override;

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

inline Value SingleValue::next(const Value& val) const
{ return val; }

inline Value SingleValue::prev(const Value& val) const
{ return val; }

/***********************/

inline Value IntervalOfValues::rand(PRG* prg) const
{ return f_rand(prg); }

inline Value IntervalOfValues::next(const Value& val) const
{ return f_next(val); }

inline Value IntervalOfValues::prev(const Value& val) const
{ return f_prev(val); }

/***********************/

inline Value SetOfValues::rand(PRG* prg) const {
    if (m_values.empty()) return Value();
    return m_values.at(prg->uniform(m_values.size()-1));
}

inline const Values& SetOfValues::values() const
{ return m_values; }

} // evoplex
#endif // ATTRIBUTE_RANGE_H
