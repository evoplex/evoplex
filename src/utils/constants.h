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

// Plugin stuff
#define PLUGIN_ATTRIBUTE_UID "uid"                      // the plugin's unique id
#define PLUGIN_ATTRIBUTE_AUTHOR "author"                // authors' name
#define PLUGIN_ATTRIBUTE_NAME "name"                    // plugin's name
#define PLUGIN_ATTRIBUTE_DESCRIPTION "description"      // plugin's description

#define PLUGIN_ATTRIBUTE_GRAPHSPACE "graphAttributesSpace"  // domain of the graph's attributes

#define PLUGIN_ATTRIBUTE_AGENTSPACE "agentAttributesSpace"  // domain of the agent's attributes
#define PLUGIN_ATTRIBUTE_MODELSPACE "modelAttributesSpace"  // domain of the model's attributes
#define PLUGIN_ATTRIBUTE_SUPPORTEDGRAPHS "supportedGraphs"  // graphIds allowed in the model


#endif // CONSTANTS_H
