/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2016 - Marcos Cardinot <marcos@cardinot.net>
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

#ifndef CONSTANTS_H
#define CONSTANTS_H

/******************************************************************************
    Impose limits for sanity
******************************************************************************/
#define EVOPLEX_MAX_NODES 100000000    // maximum number of nodes (10^8)
#define EVOPLEX_MAX_STEPS 100000000     // maximum number of steps (10^8)
#define EVOPLEX_MAX_TRIALS 1000         // maximum number of times an experiment can be repeated (10^3)

/******************************************************************************
    These constants hold the name of the properties common to any experiment.
******************************************************************************/
#define GENERAL_ATTRIBUTE_EXPID "id"                    // the experiment id
#define GENERAL_ATTRIBUTE_NODES "nodes"                 // a path to a csv file OR an integer with the number of nodes
#define GENERAL_ATTRIBUTE_GRAPHID "graphId"             // uid of the graph used
#define GENERAL_ATTRIBUTE_MODELID "modelId"             // uid of the model used
#define GENERAL_ATTRIBUTE_SEED "seed"                   // seed of the PRG
#define GENERAL_ATTRIBUTE_STOPAT "stopAt"               // hard stop condition, ie., last simulation step
#define GENERAL_ATTRIBUTE_TRIALS "trials"               // number of times the experiment has to be repeated
#define GENERAL_ATTRIBUTE_AUTODELETE "autoDelete"       // automatically deletes the experiment from memory
#define GENERAL_ATTRIBUTE_GRAPHTYPE "graphType"         // graph type of a graph generator

#define OUTPUT_DIR "directory"              // path to the directory in which the file will be saved
#define OUTPUT_AVGTRIALS "avgTrials"        // 1 to indicate if the output should be done across all trials; 0 otherwise
#define OUTPUT_HEADER "header"              // valid header
#define OUTPUT_SAVESTEPS "saveSteps"        // n=0 to save all steps; n>0 to save the last n steps

/******************************************************************************
    Plugin stuff
******************************************************************************/
#define PLUGIN_ATTRIBUTE_TYPE "type"                    // plugin's type
#define PLUGIN_ATTRIBUTE_UID "uid"                      // plugin's unique id
#define PLUGIN_ATTRIBUTE_AUTHOR "author"                // authors' name
#define PLUGIN_ATTRIBUTE_NAME "name"                    // plugin's name
#define PLUGIN_ATTRIBUTE_DESCRIPTION "description"      // plugin's description
#define PLUGIN_ATTRIBUTES_SCOPE "pluginAttributesScope" // domain of the plugin's attributes
// model (only)
#define PLUGIN_ATTRIBUTE_NODESCOPE "nodeAttributesScope"    // domain of the node's attributes
#define PLUGIN_ATTRIBUTE_EDGESCOPE "edgeAttributesScope"    // domain of the edge's attributes
#define PLUGIN_ATTRIBUTE_CUSTOMOUTPUTS "customOutputs"      // custom outputs defined in the model
#define PLUGIN_ATTRIBUTE_SUPPORTEDGRAPHS "supportedGraphs"  // graphIds allowed in the model
// graph (only)
#define PLUGIN_ATTRIBUTE_VALIDGRAPHTYPES "validGraphTypes"  // valid graph types of a graph generator

/******************************************************************************
    Compiler version and architecture
******************************************************************************/
// stringfy intergers
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
// version
#if defined __clang__
    #define COMPILER_VERSION "Clang " STR(__clang_major__) "." STR(__clang_minor__) "." STR(__clang_patchlevel__)
# elif defined __GNUC__
    #ifdef __MINGW32__
        #define COMPILER_NAME "MinGW GCC "
    #else
        #define COMPILER_NAME "GCC "
    #endif
    #define COMPILER_VERSION COMPILER_NAME STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__)
#elif defined _MSC_VER
    #if _MSC_VER == 1600
        #define COMPILER_VERSION "MSVC++ 10.0 (Visual Studio 2010)"
    #elif _MSC_VER == 1700
        #define COMPILER_VERSION "MSVC++ 11.0 (Visual Studio 2012)"
    #elif _MSC_VER == 1800
        #define COMPILER_VERSION "MSVC++ 12.0 (Visual Studio 2013)"
    #elif _MSC_VER == 1900
        #define COMPILER_VERSION "MSVC++ 14.0 (Visual Studio 2015)"
    #elif _MSC_VER == 1910
        #define COMPILER_VERSION "MSVC++ 14.1 (Visual Studio 2017 v15.0)"
    #elif _MSC_VER == 1911
        #define COMPILER_VERSION "MSVC++ 14.11 (Visual Studio 2017 v15.3)"
    #elif _MSC_VER == 1912
        #define COMPILER_VERSION "MSVC++ 14.12 (Visual Studio 2017 v15.5)"
    #elif _MSC_VER == 1913
        #define COMPILER_VERSION "MSVC++ 14.13 (Visual Studio 2017 v15.6)"
    #elif _MSC_VER == 1914
        #define COMPILER_VERSION "MSVC++ 14.14 (Visual Studio 2017 v15.7)"
    #else
        #define COMPILER_VERSION "Unknown MSVC++ version " STR(_MSC_VER)
    #endif
#else
    #define COMPILER_VERSION "Unknown"
#endif
// architecture
#if defined(__LP64__) || defined(_WIN64)
    #define COMPILER_ARCHITECTURE "64-bit"
#else
    #define COMPILER_ARCHITECTURE "32-bit"
#endif

#endif // CONSTANTS_H
