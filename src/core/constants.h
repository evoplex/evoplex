/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Impose limits for sanity
#define EVOPLEX_MAX_AGENTS 100000000    // maximum number of agents (10^8)
#define EVOPLEX_MAX_STEPS 100000000     // maximum number of steps (10^8)
#define EVOPLEX_MAX_TRIALS 1000         // maximum number of times an experiment can be repeated (10^3)

// These constants hold the name of the properties common to any experiment.
#define GENERAL_ATTRIBUTE_AGENTS "agents"               // a path to a csv file OR an integer with the number of agents
#define GENERAL_ATTRIBUTE_GRAPHID "graphId"             // uid of the graph used
#define GENERAL_ATTRIBUTE_MODELID "modelId"             // uid of the model used
#define GENERAL_ATTRIBUTE_SEED "seed"                   // seed of the PRG
#define GENERAL_ATTRIBUTE_STOPAT "stopAt"               // hard stop condition, ie., last simulation step
#define GENERAL_ATTRIBUTE_TRIALS "trials"               // number of times the experiment has to be repeated
#define GENERAL_ATTRIBUTE_AUTODELETE "autoDelete"       // automatically deletes the experiment from memory

#define OUTPUT_DIR "directory"              // path to the directory in which the file will be saved
#define OUTPUT_AVGTRIALS "avgTrials"        // 1 to indicate if the output should be done across all trials; 0 otherwise
#define OUTPUT_HEADER "header"              // valid header
#define OUTPUT_SAVESTEPS "saveSteps"        // n=0 to save all steps; n>0 to save the last n steps

// Plugin stuff
#define PLUGIN_ATTRIBUTE_TYPE "type"                    // plugin's type
#define PLUGIN_ATTRIBUTE_UID "uid"                      // plugin's unique id
#define PLUGIN_ATTRIBUTE_AUTHOR "author"                // authors' name
#define PLUGIN_ATTRIBUTE_NAME "name"                    // plugin's name
#define PLUGIN_ATTRIBUTE_DESCRIPTION "description"      // plugin's description
#define PLUGIN_ATTRIBUTE_SPACE "pluginAttributesSpace"  // domain of the plugin's attributes
// model (only)
#define PLUGIN_ATTRIBUTE_AGENTSPACE "agentAttributesSpace"  // domain of the agent's attributes
#define PLUGIN_ATTRIBUTE_EDGESPACE "edgeAttributesSpace"    // domain of the edge's attributes
#define PLUGIN_ATTRIBUTE_CUSTOMOUTPUTS "customOutputs"      // custom outputs defined in the model
#define PLUGIN_ATTRIBUTE_SUPPORTEDGRAPHS "supportedGraphs"  // graphIds allowed in the model
// graph (only)
#define PLUGIN_ATTRIBUTE_VALIDGRAPHTYPES "validGraphTypes"  // valid graph types of a graph generator


#endif // CONSTANTS_H
