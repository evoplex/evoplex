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
<a href="https://github.com/evoplex/evoplex/blob/master/README.md#evoplex"><img src="https://img.shields.io/badge/awesome-yes-green.svg" alt=""></a>
</p>

--------------------------------------------------------------------------------

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f393fa28b9bc443d8847a5e672ec4826)](https://app.codacy.com/app/cardinot/evoplex?utm_source=github.com&utm_medium=referral&utm_content=evoplex/evoplex&utm_campaign=Badge_Grade_Dashboard)

Evoplex is a platform for implementing Agent-Based Models (ABM) and Multi-Agent Systems (MAS) imposing a [theoretical-graph approach](https://en.wikipedia.org/wiki/Graph_theory). Here, each agent is represented as a node in the network and is constrained to interact only with its neighbours, which are linked by edges in the network.

<p align="center">
  <img alt="Evoplex 0.1.0" src="https://evoplex.org/img/evoplex2018-06-17.gif" width="70%">
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

 * [Download & Install](https://evoplex.org/#download)

 * [Quick guide](https://evoplex.org/docs/user-interface)

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

## Citing

If you use Evoplex in your publication, you can cite it as:

> Marcos Cardinot, Colm O'Riordan, & Josephine Griffith. Evoplex: a powerful multi-agent system for networks. Zenodo. [http://doi.org/10.5281/zenodo.1340735](http://doi.org/10.5281/zenodo.1340735)

## License

Evoplex is licensed under the [GNU General Public License v3.0](https://github.com/evoplex/evoplex/blob/master/LICENSE.txt).
