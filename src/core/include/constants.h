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
#define EVOPLEX_MAX_STEPS 100000000    // maximum number of steps (10^8)
#define EVOPLEX_MAX_TRIALS 1000        // maximum number of times an experiment can be repeated (10^3)
#define EVOPLEX_MAX_PROJECTS 100       // maximum number of opened projects at the same time (10^2)

/******************************************************************************
    These constants hold the name of the properties common to any experiment.
******************************************************************************/
#define GENERAL_ATTR_EXPID "id"                    // the experiment id
#define GENERAL_ATTR_NODES "nodes"                 // a path to a csv file OR a command to AttrsGenerator
#define GENERAL_ATTR_GRAPHID "graphId"             // graph's id
#define GENERAL_ATTR_MODELID "modelId"             // model's id
#define GENERAL_ATTR_GRAPHVS "graphVersion"        // graph's version
#define GENERAL_ATTR_MODELVS "modelVersion"        // model's version
#define GENERAL_ATTR_SEED "seed"                   // seed of the PRG
#define GENERAL_ATTR_STOPAT "stopAt"               // hard stop condition, ie., last simulation step
#define GENERAL_ATTR_TRIALS "trials"               // number of times the experiment has to be repeated
#define GENERAL_ATTR_AUTODELETE "autoDelete"       // automatically deletes the experiment from memory
#define GENERAL_ATTR_GRAPHTYPE "graphType"         // graph type of a graph generator
#define GENERAL_ATTR_EDGEATTRS "edgeAttrs"         // a command to AttrsGenerator

#define OUTPUT_DIR "outputDirectory"              // path to the directory in which the file will be saved
#define OUTPUT_AVGTRIALS "outputAvgTrials"        // 1 to indicate if the output should be done across all trials; 0 otherwise
#define OUTPUT_HEADER "outputHeader"              // valid header
#define OUTPUT_SAVESTEPS "outputSaveSteps"        // n=0 to save all steps; n>0 to save the last n steps

/******************************************************************************
    Plugin stuff
******************************************************************************/
#define PLUGIN_ATTR_TYPE "type"                    // plugin's type (graph or model)
#define PLUGIN_ATTR_UID "uid"                      // plugin's unique id
#define PLUGIN_ATTR_AUTHOR "author"                // authors' name
#define PLUGIN_ATTR_TITLE "title"                  // plugin's title
#define PLUGIN_ATTR_DESCRIPTION "description"      // plugin's description
#define PLUGIN_ATTR_VERSION "version"              // plugin's version [0,UINT16_MAX]
#define PLUGIN_ATTR_ATTRSSCOPE "pluginAttributesScope" // domain of the plugin's attributes
// model (only)
#define PLUGIN_ATTR_NODESCOPE "nodeAttributesScope"    // domain of the node's attributes
#define PLUGIN_ATTR_EDGESCOPE "edgeAttributesScope"    // domain of the edge's attributes
#define PLUGIN_ATTR_CUSTOMOUTPUTS "customOutputs"      // custom outputs defined in the model
#define PLUGIN_ATTR_SUPPORTEDGRAPHS "supportedGraphs"  // graphIds allowed in the model; empty to allow all graphs
// graph (only)
#define PLUGIN_ATTR_VALIDGRAPHTYPES "validGraphTypes"     // valid graph types of a graph generator
#define PLUGIN_ATTR_EDGEATTRSGEN "supportsEdgeAttrsGen"   // true if the graph supports edge attributes generator

#endif // CONSTANTS_H
