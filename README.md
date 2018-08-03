# Evoplex

[![gitter](https://badges.gitter.im/EvoplexMAS/evoplex.svg)](https://gitter.im/EvoplexMAS/evoplex)
[![coverity](https://scan.coverity.com/projects/16076/badge.svg)](https://scan.coverity.com/projects/evoplex-evoplex)
[![awesome](https://img.shields.io/badge/awesome-yes-green.svg)](https://github.com/evoplex/evoplex/blob/master/README.md#evoplex)

Linux & MacOS [![travis-ci](https://travis-ci.org/evoplex/evoplex.svg?branch=master)](https://travis-ci.org/evoplex/evoplex)

Windows [![appveyor](https://ci.appveyor.com/api/projects/status/ulck2seq9rqgawbr/branch/master?svg=true)](https://ci.appveyor.com/project/cardinot/evoplex/branch/master)

Tests [![coveralls](https://coveralls.io/repos/github/evoplex/evoplex/badge.svg)](https://coveralls.io/github/evoplex/evoplex)
[![codecov](https://codecov.io/gh/evoplex/evoplex/branch/master/graph/badge.svg)](https://codecov.io/gh/evoplex/evoplex)

Evoplex is a multi-agent system for [networks (graphs)](https://en.wikipedia.org/wiki/Graph_theory). Here, each agent is represented as a node in the network and is constrained to interact only with its neighbours, which are linked by edges in the network.

<p align="center">
  <img alt="Evoplex 0.0.1" src="https://evoplex.github.io/assets/img/evoplex2018-06-17.gif" width="70%">
</p>

**Wait, another multi-agent system?** Why not using something like [MASON](https://cs.gmu.edu/~eclab/projects/mason/) or [NetLogo](https://ccl.northwestern.edu/netlogo/)?
Yes, there are hundreds of options out there, but I bet none of them can do what Evoplex does!

In fact, most of the existing pieces of software are not good enough. Main issues include: really old technologies, unmaintained code, inflexibility, poor performance and others. Thus, over the last years, people often prefer to develop their own solutions from scratch. However, we know that reinventing the wheel is always time-consuming, expensive and usually a terrible strategy.

In this way, we decided to create Evoplex! An extremely fast, multi-threaded, user-friendly, cross-platform and truly modular application.

Evoplex was originally developed to tackle problems in the field of [Evolutionary Graph Theory](https://en.wikipedia.org/wiki/Evolutionary_graph_theory). However, it has been used in a wide range of scenarios, including:

 * [Evolutionary Dynamics](https://en.wikipedia.org/wiki/Evolutionary_dynamics)
 * [Game Theory](https://en.wikipedia.org/wiki/Game_theory)
 * [Cellular Automata](https://en.wikipedia.org/wiki/Cellular_automaton)
 * ... the applications are endless!

## Getting Started

Evoplex is cross-platform and is available for Linux, Windows and macOS.

For users:

 * [Installation](https://github.com/evoplex/evoplex/releases)
 * [Quick guide](https://github.com/evoplex/evoplex/wiki/Navigation)

For developers:

 * [Building and running Evoplex from source code](https://github.com/evoplex/evoplex/wiki/Build-from-Source-Code)

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
- Follow us on [Twitter](https://twitter.com/EvoplexMAS)
- Join us on our [Gitter chat channel](https://gitter.im/EvoplexMAS/evoplex)

## License

Evoplex is licensed under the [GNU General Public License v3.0](https://github.com/evoplex/evoplex/blob/master/LICENSE.txt).
