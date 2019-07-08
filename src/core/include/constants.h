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

#ifndef CONSTANTS_H
#define CONSTANTS_H
//! @file

/******************************************************************************
    Impose limits for sanity
******************************************************************************/
//! maximum number of nodes (10^8)
#define EVOPLEX_MAX_NODES 100000000
//! maximum number of steps (10^8)
#define EVOPLEX_MAX_STEPS 100000000
//! maximum number of times an experiment can be repeated (10^3)
#define EVOPLEX_MAX_TRIALS 1000
//! maximum number of opened projects at the same time (10^2)
#define EVOPLEX_MAX_PROJECTS 100

/******************************************************************************
    These constants hold the name of the properties common to any experiment.
******************************************************************************/
//! the experiment id
#define GENERAL_ATTR_EXPID "id"
//! a path to a csv file OR a command to AttrsGenerator
#define GENERAL_ATTR_NODES "nodes"
//! graph's id
#define GENERAL_ATTR_GRAPHID "graphId"
//! model's id
#define GENERAL_ATTR_MODELID "modelId"
//! graph's version
#define GENERAL_ATTR_GRAPHVS "graphVersion"
//! model's version
#define GENERAL_ATTR_MODELVS "modelVersion"
//! seed of the PRG
#define GENERAL_ATTR_SEED "seed"
//! hard stop condition, ie., last simulation step
#define GENERAL_ATTR_STOPAT "stopAt"
//! number of times the experiment has to be repeated
#define GENERAL_ATTR_TRIALS "trials"
//! automatically deletes the experiment from memory
#define GENERAL_ATTR_AUTODELETE "autoDelete"
//! graph type of a graph generator
#define GENERAL_ATTR_GRAPHTYPE "graphType"
//! a command to AttrsGenerator
#define GENERAL_ATTR_EDGEATTRS "edgeAttrs"
//! node attributes
#define GENERAL_ATTR_NODEATTRS "nodeAttrs"
//! number of edge attributes
#define GENERAL_ATTR_EDGE_NUM "edgeAttrsNumber"
//! number of node attributes
#define GENERAL_ATTR_NODE_NUM "nodeAttrsNumber"

//! path to the directory in which the file will be saved
#define OUTPUT_DIR "outputDirectory"
//! 1 to indicate if the output should be done across all trials; 0 otherwise
#define OUTPUT_AVGTRIALS "outputAvgTrials"
//! valid header
#define OUTPUT_HEADER "outputHeader"
//! n=0 to save all steps; n>0 to save the last n steps
#define OUTPUT_SAVESTEPS "outputSaveSteps"

/******************************************************************************
    Plugin stuff
******************************************************************************/
//! plugin's type (graph or model)
#define PLUGIN_ATTR_TYPE "type"
//! plugin's unique id
#define PLUGIN_ATTR_UID "uid"
//! authors' name
#define PLUGIN_ATTR_AUTHOR "author"
//! plugin's title
#define PLUGIN_ATTR_TITLE "title"
//! plugin's description
#define PLUGIN_ATTR_DESCRIPTION "description"
//! plugin's version [0,UINT16_MAX]
#define PLUGIN_ATTR_VERSION "version"
//! domain of the plugin's attributes
#define PLUGIN_ATTR_ATTRSSCOPE "pluginAttributesScope"

// model (only)

//! domain of the node's attributes
#define PLUGIN_ATTR_NODESCOPE "nodeAttributesScope"
//! domain of the edge's attributes
#define PLUGIN_ATTR_EDGESCOPE "edgeAttributesScope"
//! custom outputs defined in the model
#define PLUGIN_ATTR_CUSTOMOUTPUTS "customOutputs"
//! graphIds allowed in the model; empty to allow all graphs
#define PLUGIN_ATTR_SUPPORTEDGRAPHS "supportedGraphs"

// graph (only)

//! valid graph types of a graph generator
#define PLUGIN_ATTR_VALIDGRAPHTYPES "validGraphTypes"
//! true if the graph supports edge attributes generator
#define PLUGIN_ATTR_EDGEATTRSGEN "supportsEdgeAttrsGen"

//! @}
#endif // CONSTANTS_H
