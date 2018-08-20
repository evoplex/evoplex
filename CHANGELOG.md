# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- Adds support to edges attributes
- Increases unit-tests coverage of core components
- Implements the Bernoulli distribution in the PRG class
- Plugins can now be reloaded
- Adds button to maximize graph widgets
- Adds button to pause all experiments
- New model plugin: population growth
- New model plugin: Conway's Game of Life cellular automaton

### Changed
- Node's coordinates are now a float
- Graph widgets now use an OpenGL back end
- Improved look and feel of graph widgets
- Improved GraphView performance by avoiding caching invisible elements
- Attributes' inspector of the graph widgets now shows a proper field for each type of attribute
- Plugins page: minor GUI improvements
- Renamed the 'customGraph' plugin to 'edgesFromCSV'
- 'edgesFromCSV' plugin now supports edge attributes
- 'squareGuid' plugin now supports edge attributes

### Fixed
- Fixed bug when pausing an experiment with queued trials
- Close the ExperimentDesigner if there is no project opened
- `bool` attribute should act like a range and not as a single value
- Attrs generator should accept literal boolean, i.e., 'true' and 'false'
- Node::randNeighbour crashes when neighbourhood is empty

## [0.1.0] - 2018-08-06
### Added
- Fixes #8 - Allows deleting an experiment
- Value class now accepts utf8 string
- Model plugins can now add and remove nodes and edges during the simulation
- Model plugins: added beforeLoop() and afterLoop() methods
- Plugins can now be versioned
- Increases unit-tests coverage of core components
- Adds new attribute range: "non-empty-string"
- ExpDesigner is now able to recognize "filepath" and "dirpath" attributes

### Changed
- Improves plugins initialization
- Improves and unify plugins interfaces
- Improves threads management
- Improves TableWidget performance

### Fixed
- Fixes packaging of all OSs
- Fixes logger on Windows
- Fixes #6 - Don't overwrite projects by default
- Fixes #7 - Checks for running experiments before closing a project
- Fixes #9 - Removes question mark from titlebar on Windows

## [0.1.0-alpha] - 2018-06-21
### Added
- First public release
