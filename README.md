<p align="center" style="margin-bottom: 5px">
  <a href="https://evoplex.org">
    <img src="https://evoplex.org/logos/evoplex-large.svg" width="60%">
  </a>
</p>

<p align="center">
<a href="https://gitter.im/EvoplexOrg/evoplex"><img src="https://badges.gitter.im/EvoplexOrg/evoplex.svg" alt="Chat"></a>
<a href="https://travis-ci.org/evoplex/evoplex"><img src="https://travis-ci.org/evoplex/evoplex.svg?branch=master" alt="CI: Linux & MacOS"></a>
<a href="https://ci.appveyor.com/project/cardinot/evoplex/branch/master"><img src="https://ci.appveyor.com/api/projects/status/ulck2seq9rqgawbr/branch/master?svg=true" alt="CI: Windows"></a>
<a href="https://coveralls.io/github/evoplex/evoplex"><img src="https://coveralls.io/repos/github/evoplex/evoplex/badge.svg" alt="Testing"></a>
<a href="https://codecov.io/gh/evoplex/evoplex"><img src="https://codecov.io/gh/evoplex/evoplex/branch/master/graph/badge.svg" alt="Testing"></a>
<a href="https://scan.coverity.com/projects/evoplex-evoplex"><img src="https://scan.coverity.com/projects/16076/badge.svg" alt="Testing"></a>
<a href="https://app.codacy.com/app/cardinot/evoplex"><img src="https://api.codacy.com/project/badge/Grade/f393fa28b9bc443d8847a5e672ec4826" alt="Testing"></a>
<a href="https://github.com/evoplex/evoplex/blob/master/README.md#evoplex"><img src="https://img.shields.io/badge/awesome-yes-green.svg" alt=""></a>
</p>

--------------------------------------------------------------------------------

[Evoplex](https://evoplex.org) is a fast, robust and extensible platform for developing agent-based models (ABM) and multi-agent systems (MAS) on networks. Each agent is represented as a node and interacts with its neighbors, as defined by the network structure.

<p align="center">
  <img alt="Evoplex 0.2.0" src="https://evoplex.org/img/evoplex0.2.0_nowak92_2018-09-03.gif" width="90%">
</p>

**Wait, another multi-agent system?** Why not using something like [MASON](https://cs.gmu.edu/~eclab/projects/mason/) or [NetLogo](https://ccl.northwestern.edu/netlogo/)?
Yes, there are hundreds of options out there, but I bet none of them can do what Evoplex does!

In fact, most of the existing pieces of software are not good enough. Main issues include: really old technologies, unmaintained code, inflexibility, poor performance and others. Thus, over the last years, people often prefer to develop their own solutions from scratch. However, we know that reinventing the wheel is always time-consuming, expensive and usually a terrible strategy.

In this way, we decided to create Evoplex! An extremely fast, multi-threaded, user-friendly, cross-platform and truly modular application.

Evoplex was originally developed to tackle problems in the field of **Evo**lutionary Computation and Com**plex** Systems, and that's why we call it **Evoplex**. However, it has been used in a wide range of scenarios, including:

 * [Evolutionary Graph Theory](https://en.wikipedia.org/wiki/Evolutionary_graph_theory)
 * [Evolutionary Dynamics](https://en.wikipedia.org/wiki/Evolutionary_dynamics)
 * [Game Theory](https://en.wikipedia.org/wiki/Game_theory)
 * [Cellular Automata](https://en.wikipedia.org/wiki/Cellular_automaton)
 * [Complex Adaptive Systems](https://en.wikipedia.org/wiki/Complex_adaptive_system)
 * ... the applications are endless!

## Getting Started

Evoplex is cross-platform and is available for Linux, Windows and macOS.

 * [Download & Install](https://evoplex.org/index#download)
 * [User docs](https://evoplex.org/docs)

## Getting Involved

As an open-source project, we welcome new contributions from the community. There are many ways you can participate in the project, for example:

 * Review the documentation and make pull requests for anything from typos to new content
 * Report bugs and submit feature requests
 * Review source code changes
 * Add a new feature or change an existing one
 * Showcase your plugins

Please see the [CONTRIBUTING](https://github.com/evoplex/evoplex/blob/master/CONTRIBUTING.md) file for details. Be aware that by contributing to the code base, you are agreeing that Evoplex may redistribute your work under our [LICENSE](https://github.com/cardinot/evoplex/blob/master/LICENSE.txt). Please see also our [Code of Conduct](https://github.com/evoplex/evoplex/blob/master/CODE_OF_CONDUCT.md).

## Feedback

- Ask a question in the [mailing list](https://groups.google.com/group/evoplex) (or send it directly to evoplex@googlegroups.com)
- File a bug or request a new feature on [GitHub](https://github.com/evoplex/evoplex/issues)
- Follow us on [Twitter](https://twitter.com/EvoplexOrg)
- Join us on our [Gitter chat channel](https://gitter.im/EvoplexOrg/evoplex)

## How to cite

If you use Evoplex in your publication, you can cite it as:

> Marcos Cardinot, Colm O'Riordan, Josephine Griffith & Matjaz Perc. Evoplex: A platform for agent-based modeling on networks. Zenodo. [https://doi.org/10.5281/zenodo.1340734](https://doi.org/10.5281/zenodo.1340734)

## License

The *EvoplexCore* library is licensed under the [Apache License 2.0](https://github.com/evoplex/evoplex/blob/master/src/core/LICENSE.txt).

The *EvoplexGUI* library is licensed under the [GNU General Public License v3.0](https://github.com/evoplex/evoplex/blob/master/src/gui/LICENSE.txt).
