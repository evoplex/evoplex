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

#ifndef VALUE_H
#define VALUE_H

#include <functional>
#include <vector>
#include <QString>

namespace evoplex {

class Value;
using Values = std::vector<Value>;

/**
 * @brief A class for variant data types (tagged union).
 *
 * This is a very important class in Evoplex, it defines a data type that can
 * behave like an integer, double, char, bool or string in a very efficient way.
 *
 * A Value object holds a single value of a single type() at a time.
 * You can find out what type (T) the Value holds and get its value using one
 * of the toT() functions (e.g., toInt()).
 *
 * The functions named toT() (e.g., toInt(), toString()) are const. If you ask
 * for the stored type, they return a copy of the stored object.
 *
 * Here is some example code to demonstrate the use of Value:
 * @code{.cpp}
 * Value v(123);                // The Value now contains an int
 * int x = v.toInt();           // x = 123
 * v = Value("hello");          // The Value now contains a string
 * int y = v.toInt();           // throws an exception as v cannot be converted to an int
 * QString s = v.toQString();   // s = "hello"
 * ...
 * v = 1.1;                     // v.type() is equal to DOUBLE
 * v = v.toDouble() + 1.2;      // The Value now holds 2.3
 * @endcode
 *
 * @ingroup PublicAPI
 */
class Value
{
    friend struct std::hash<Value>;

public:
    /**
     * @brief This enum defines the types of variable that a Value can contain.
     */
    enum Type { BOOL, CHAR, DOUBLE, INT, STRING, INVALID };

    /**
     * @brief Constructs an invalid Value.
     * @see isValid()
     */
    Value();

    /**
     * @brief Constructs a copy of the @p value passed as
     *        the argument to this constructor.
     */
    Value(const Value& value);

    /**
     * @brief Constructs a new Value with a boolean value.
     */
    Value(bool value);

    /**
     * @brief Constructs a new Value with a char value.
     */
    Value(char value);

    /**
     * @brief Constructs a new Value with a double value.
     */
    Value(double value);

    /**
     * @brief Constructs a new Value with an integer value.
     */
    Value(int value);

    /**
     * @brief Constructs a new Value with a string value.
     */
    Value(const char* value);

    /**
     * @brief Constructs a new Value with a string value.
     */
    Value(const QString& value);

    /**
     * @brief Constructs a new Value with a boolean value.
     * This constructor is available just to accept the specialized vector<bool>.
     * http://www.cplusplus.com/reference/vector/vector-bool
     */
    Value(std::vector<bool>::reference value);

    /**
     * @brief Destroys this Value.
     */
    ~Value();

    /**
     * @brief Returns the storage type of the data stored in the Value.
     */
    inline Type type() const;

    /**
     * @brief Returns true if the storage type of this
     *        Value is not Type::INVALID.
     */
    inline bool isValid() const;

    /**
     * @brief Returns true if the storage type of this
     *        Value is equal to Type::BOOL.
     */
    inline bool isBool() const;

    /**
     * @brief Returns true if the storage type of this
     *        Value is equal to Type::CHAR.
     */
    inline bool isChar() const;

    /**
     * @brief Returns true if the storage type of this
     *        Value is equal to Type::DOUBLE.
     */
    inline bool isDouble() const;

    /**
     * @brief Returns true if the storage type of this
     *        Value is equal to Type::INT.
     */
    inline bool isInt() const;

    /**
     * @brief Returns true if the storage type of this
     *        Value is equal to Type::STRING.
     */
    inline bool isString() const;

    /**
     * @brief Returns the Value as a boolean.
     * It returns a boolean if the data type() is equal to Type::BOOL.
     * Otherwise, it throws an expection.
     * @throw std::logic_error.
     */
    inline bool toBool() const;

    /**
     * @brief Returns the Value as a char.
     * It returns a char if the data type() is equal to Type::CHAR.
     * Otherwise, it throws an expection.
     * @throw std::logic_error.
     */
    inline char toChar() const;

    /**
     * @brief Returns the Value as a double.
     * It returns a double if the data type() is equal to Type::DOUBLE.
     * Otherwise, it throws an expection.
     * @throw std::logic_error.
     */
    inline double toDouble() const;

    /**
     * @brief Returns the Value as an int.
     * It returns an int if the data type() is equal to Type::INT.
     * Otherwise, it throws an expection.
     * @throw std::logic_error.
     */
    inline int toInt() const;

    /**
     * @brief Returns the Value as an unsigned integer.
     * It returns an unsigned integer if the data type() is equal to Type::INT
     * and the value is >= 0. Otherwise, it throws an expection.
     * @throw std::logic_error.
     */
    inline quint32 toUInt() const;

    /**
     * @brief Returns the Value as a string.
     * It returns a string if the data type() is equal to Type::STRING.
     * Otherwise, it throws an expection.
     * @throw std::logic_error.
     */
    inline const char* toString() const;

    /**
     * @brief Returns the Value as a QString.
     * It returns a QString for any data type().
     * For example:
     * @code{.cpp}
     * Value v = 123;                // v is now an integer equal to 123
     * QString s = v.toQString();    // s = "123"
     * v = true;                     // v is now a boolean
     * s = v.toQString();            // s = "1"
     * v = "hello";                  // v is now a string
     * s = v.toQString();            // s = "hello"
     * @endcode
     */
    QString toQString(char format = 'g', int precision = 8) const;

    /**
     * @brief Assigns the value @p v to this Value.
     */
    Value& operator=(const Value& v);

    /**
     * @brief Checks if this Value is equal to @p v.
     */
    bool operator==(const Value& v) const;

    /**
     * @brief Checks if this Value is different from @p v.
     */
    bool operator!=(const Value& v) const;

    /**
     * @brief Checks if this Value is less than @p v.
     */
    bool operator<(const Value& v) const;

    /**
     * @brief Checks if this Value is greater than @p v.
     */
    bool operator>(const Value& v) const;

    /**
     * @brief Checks if this Value is less or equal to @p v.
     */
    bool operator<=(const Value& v) const;

    /**
     * @brief Checks if this Value is greater or equal to @p v.
     */
    bool operator>=(const Value& v) const;

private:
    union { bool b; char c; double d; int i; const char* s; } m_data;
    Type m_type;

    std::logic_error throwError() const;
};

/************************************************************************
   Value: Inline member functions
 ************************************************************************/

inline Value::Type Value::type() const
{ return m_type; }

inline bool Value::isValid() const
{ return m_type != INVALID; }

inline bool Value::isBool() const
{ return m_type == BOOL; }

inline bool Value::isChar() const
{ return m_type == CHAR; }

inline bool Value::isDouble() const
{ return m_type == DOUBLE; }

inline bool Value::isInt() const
{ return m_type == INT; }

inline bool Value::isString() const
{ return m_type == STRING; }

inline bool Value::toBool() const
{ if (m_type == BOOL) { return m_data.b; } throw throwError(); }

inline char Value::toChar() const
{ if (m_type == CHAR) { return m_data.c; } throw throwError(); }

inline double Value::toDouble() const
{ if (m_type == DOUBLE) { return m_data.d; } throw throwError(); }

inline int Value::toInt() const
{ if (m_type == INT) { return m_data.i; } throw throwError(); }

inline const char* Value::toString() const
{ if (m_type == STRING) { return m_data.s; } throw throwError(); }

inline quint32 Value::toUInt() const {
    if (m_type == INT && m_data.i >= 0) { return static_cast<quint32>(m_data.i); }
    throw throwError();
}

} // evoplex


namespace std
{
/**
 * @brief A hash function for char*.
 * http://www.cse.yorku.ca/~oz/hash.html
 */
template <>
struct hash<char*>
{
    size_t operator()(const char *s) const {
        size_t h = 5381;
        int c;
        while ((c = *s++)) {
            h = ((h << 5) + h) + c;
        }
        return h;
    }
};

/**
 * @brief Implements a hash function for the Value class.
 */
template <>
struct hash<evoplex::Value>
{
    size_t operator()(const evoplex::Value& v) const {
        switch (v.type()) {
        case evoplex::Value::INT: return std::hash<int>()(v.m_data.i);
        case evoplex::Value::DOUBLE: return std::hash<double>()(v.m_data.d);
        case evoplex::Value::BOOL: return std::hash<bool>()(v.m_data.b);
        case evoplex::Value::CHAR: return std::hash<char>()(v.m_data.c);
        case evoplex::Value::STRING: return std::hash<char*>()(v.m_data.s);
        default: throw std::invalid_argument("invalid type of Value");
        }
    }
};
} // std
#endif // VALUE_H
