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
#define GENERAL_ATTRIBUTE_EXPID "id"                    // the experiment id
#define GENERAL_ATTRIBUTE_NODES "nodes"                 // a path to a csv file OR a command to AttrsGenerator
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
#define PLUGIN_ATTRIBUTE_TYPE "type"                    // plugin's type (graph or model)
#define PLUGIN_ATTRIBUTE_UID "uid"                      // plugin's unique id
#define PLUGIN_ATTRIBUTE_AUTHOR "author"                // authors' name
#define PLUGIN_ATTRIBUTE_NAME "name"                    // plugin's name
#define PLUGIN_ATTRIBUTE_DESCRIPTION "description"      // plugin's description
#define PLUGIN_ATTRIBUTES_SCOPE "pluginAttributesScope" // domain of the plugin's attributes
// model (only)
#define PLUGIN_ATTRIBUTE_NODESCOPE "nodeAttributesScope"    // domain of the node's attributes
#define PLUGIN_ATTRIBUTE_EDGESCOPE "edgeAttributesScope"    // domain of the edge's attributes
#define PLUGIN_ATTRIBUTE_CUSTOMOUTPUTS "customOutputs"      // custom outputs defined in the model
#define PLUGIN_ATTRIBUTE_SUPPORTEDGRAPHS "supportedGraphs"  // graphIds allowed in the model; empty to allow all graphs
// graph (only)
#define PLUGIN_ATTRIBUTE_VALIDGRAPHTYPES "validGraphTypes"  // valid graph types of a graph generator

#endif // CONSTANTS_H
