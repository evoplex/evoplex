# Prisoner's Dilemma Game

## What is it?

This is a model plugin for [Evoplex](https://evoplex.org) and is included by default in the software.

It implements the spatial prisoner's dilemma game proposed by Nowak & May (1992) [1].

## How it works

In this model, agents are placed in a grid, where in each round:
- every individual accumulates the payoff obtained by playing the [prisoner's dilemma game](https://en.wikipedia.org/wiki/Prisoner%27s_dilemma) with all its immediate neighbours and itself (only for the current round);
- after this, each agent copies the strategy of the best performing agent in its neighbourhood (including itself).

This simple, and purely deterministic, spatial version of the prisoner's dilemma game, with no memories among players and no strategical elaboration, can generate chaotically changing spatial patterns, including kaleidoscopes, dynamic fractals, gliders, and so on.

## Examples

<p align="center">
<a href="http://www.youtube.com/watch?feature=player_embedded&v=79YewPo6do0" target="_blank">
<img src="http://img.youtube.com/vi/79YewPo6do0/0.jpg" alt="Evoplex" width="240" height="180" border="10" />
</a>
</p>

The figure below shows a screenshot of an experiment in Evoplex using this model. In this experiment, the model is initialized with a population of 99x99 cooperators (i.e., all agents with <i>strategy=0</i>); after this, we place one defector (<i>strategy=1</i>) in the middle of the grid. In a few steps, it is possible to observe the formation of different patterns.

<p align="center">
<img src="example.jpg" alt="Example" width="70%">
</p>

## References
- [1] Nowak, M. A., & May, R. M. (1992). Evolutionary games and spatial chaos. Nature, 359(6398), 826. DOI: [http://dx.doi.org/10.1038/359826a0](http://dx.doi.org/10.1038/359826a0)
- [2] Nowak, M. A., & Sigmund, K. (2004). Evolutionary dynamics of biological games. science, 303(5659), 793-799. DOI: [https://doi.org/10.1126/science.1093411](https://doi.org/10.1126/science.1093411)

## How to cite
If you mention this model or the Evoplex software in a publication, please cite it as:

> Cardinot, M., O’Riordan, C., Griffith, J., & Perc, M. (2019). Evoplex: A platform for agent-based modeling on networks. SoftwareX, 9, 199-204. https://doi.org/10.1016/j.softx.2019.02.009

```
@article{Evoplex,
author = "Marcos Cardinot and Colm O’Riordan and Josephine Griffith and Matjaž Perc",
title = "Evoplex: A platform for agent-based modeling on networks",
journal = "SoftwareX",
volume = "9",
pages = "199 - 204",
year = "2019",
issn = "2352-7110",
doi = "10.1016/j.softx.2019.02.009",
url = "http://www.sciencedirect.com/science/article/pii/S2352711018302437"
}
```

## License
This plugin is licensed under the [MIT License](https://opensource.org/licenses/MIT) terms.
