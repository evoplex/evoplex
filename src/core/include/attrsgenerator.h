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

#ifndef ATTRS_GENERATOR_H
#define ATTRS_GENERATOR_H

#include <functional>
#include <memory>

#include "attributes.h"
#include "attributerange.h"
#include "enum.h"

namespace evoplex {

class AGSameFuncForAll;
class AGDiffFunctions;
class AttrsGenerator;
using AttrsGeneratorPtr = std::unique_ptr<AttrsGenerator>;

/**
 * @brief A common interface for the attributes' generators classes.
 * @see AttrsGenerator, AGSameFuncForAll, AGDiffFunctions
 */
class AttrsGeneratorInterface
{
public:
    //! Destructor.
    virtual ~AttrsGeneratorInterface() = default;

    /**
     * @brief Creates a set of attributes.
     * @param size The number of Atributes objects.
     * @param progress A callback std::function that can be used to track
     *                 the progress, which is an integer from 0 to @p size.
     * @return A vector of Attributes.
     */
    virtual SetOfAttributes create(int size=-1, std::function<void(int)> progress = [](int){}) = 0;
};

/**
 * @brief Generates a set of Attributes
 * @see AGSameFuncForAll, AGDiffFunctions
 */
class AttrsGenerator : public AttrsGeneratorInterface
{
public:
    /**
     * @brief Creates a AttrsGenerator object from a command.
     * @param[in] attrsScope The attribute's scope.
     * @param[in] cmd The command.
     * @param[out] error The error message.
     *
     * The valid commands (@p cmd) are described below. In those commands,
     * the 'integer' corresponds to the @p size of the set of attributes to
     * be returned.
     * - integer
     *     - "*integer;min" alias
     * - same function for all attributes:
     *     - "*integer;min"
     *     - "*integer;max"
     *     - "*integer;rand_seed" - seed is an integer
     *     - "*min" - alias for "*1;min"
     *     - "*max" - alias for "*1;max"
     *     - "*rand_seed" - alias for "*1;rand_seed", where seed is an integer
     * - specific function for each attribute:
     *     - "#integer;attrName1_func;...;attrNameN_func"
     *         - <b>attrName:</b> The attribute name.
     *         - <b>func:</b> min, max, rand_seed or value_val (val is the input).
     *     - "#attrName_func;..." - alias for "#1;attrName_func;..."
     *         - <b>func:</b> min, max, rand_seed or value_val (val is the input).
     */
    static AttrsGeneratorPtr parse(const AttributesScope& attrsScope,
                                   const QString &cmd, QString& error);

    //! Destructor.
    virtual ~AttrsGenerator() = default;

    /**
     * @brief Gets the attribute's scope.
     */
    inline AttributesScope attrsScope() const { return m_attrsScope; }

    /**
     * @brief Gets the source command for the AttrsGenerator object.
     */
    inline const QString& command() { return m_command; }

    /**
     * @brief Gets the size of the set of attributes.
     */
    inline int size() const { return m_size; }

    /**
     * @brief Parse a 'rand_seed' command
     * @param seedStr A 'rand_seed' command, where seed is a positive integer.
     * If seed is a positive int, it returns this number as a Value object.
     * Else, it returns an invalid Value.
     */
    static Value parseRandSeed(QString seedStr);

protected:
    const AttributesScope m_attrsScope;
    const int m_size;
    QString m_command;

    /**
     * @brief Constructor.
     * @param attrsScope The attribute's scope.
     * @param size The size of the generated set of attributes.
     */
    explicit AttrsGenerator(const AttributesScope& attrsScope, const int size);

private:
    // auxiliar parser for commands starting with '*'
    static std::unique_ptr<AGSameFuncForAll> parseStarCmd(
            const AttributesScope& attrsScope, const int size,
            const QStringList& cmds, QString& error);

    // auxiliar parser for commands starting with '#'
    static std::unique_ptr<AGDiffFunctions> parseHashCmd(
            const AttributesScope& attrsScope, const int size,
            const QStringList& cmds, QString& error);
};

/**
 * @brief This class is used to generate a set of Attributes
 *        using the same function for all attributes.
 * @see AttrsGenerator, AGSameFuncForAll
 */
class AGSameFuncForAll : public AttrsGenerator
{
public:
    /**
     * @brief Constructor.
     * @param attrsScope The attribute's scope.
     * @param size The size of the generated set of attributes.
     * @param func The attributes' function.
     * @param funcInput The function's input.
     * @see AttrsGenerator::parse
     */
    explicit AGSameFuncForAll(const AttributesScope& attrsScope, const int size,
                              const Function& func, const Value& funcInput);
    ~AGSameFuncForAll() override;

    SetOfAttributes create(int size=-1,
            std::function<void(int)> progress = [](int){}) override;

    /**
     * @brief Gets the function used by this attributes generator.
     */
    inline Function function() const { return m_function; }
    /**
     * @brief Gets the input of the function used by this attributes generator.
     */
    inline const Value& functionInput() const { return m_functionInput; }

private:
    const Function m_function;
    const Value m_functionInput;

    std::function<Value(AttributeRangePtr)> f_value;
    PRG* m_prg;
};

/**
 * @brief This class is used to generate a set of Attributes
 *        using different functions for each attribute.
 * @see AttrsGenerator, AGDiffFunctions
 */
class AGDiffFunctions : public AttrsGenerator
{
public:
    /**
     * @brief The attribute's command struct.
     */
    struct AttrCmd {
        int attrId;        //!< attribute's id
        QString attrName;  //!< attribute's name
        Function func;     //!< attribute's function
        Value funcInput;   //!< function's input
    };

    /**
     * @brief Constructor.
     * @param attrsScope The attribute's scope.
     * @param size The size of the generated set of attributes.
     * @param attrCmds The attribute's commands.
     * @see AttrsGenerator::parse
     */
    explicit AGDiffFunctions(const AttributesScope& attrsScope, const int size,
                             const std::vector<AttrCmd>& attrCmds);
    ~AGDiffFunctions() override = default;

    SetOfAttributes create(int size=-1,
            std::function<void(int)> progress = [](int){}) override;

    /**
     * @brief Gets the attribute's commands.
     */
    inline const std::vector<AttrCmd>& attrCmds() const { return m_attrCmds; }

private:
    const std::vector<AttrCmd> m_attrCmds;
};

} // evoplex
#endif // ATTRS_GENERATOR_H
