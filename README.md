# Evoplex: Minimal Model

This is a minimum example of a model plugin for [Evoplex](https://evoplex.github.io). It has the essential files for you to create a new model plugin:
``` bash
├── CMakeLists.txt
├── metadata.json
├── plugin.cpp
└── plugin.h
```
You do not need to touch the `CMakeLists.txt` file. Also, you must NOT rename any of those files.

:point_right: **Note:** you *DO NOT* need to compile Evoplex from source to be able to create plugins.

### Installing dependencies
* If you compiled Evoplex from source, you already have all dependencies to compile a plugin and *DO NOT* have to install anything else.
* If you installed Evoplex from the binary packages, you will have to install the dependencies as follows:
    * [Instructions for Linux](https://github.com/evoplex/evoplex/wiki/Building-on-Linux#installing-dependencies)
    * [Instructions for MacOS](https://github.com/evoplex/evoplex/wiki/Building-on-MacOS#installing-dependencies)
    * [Instructions for Windows](https://github.com/evoplex/evoplex/wiki/Building-on-Windows#installing-dependencies)

### Compiling this plugin

:point_right: if you compiled Evoplex from source in Debug mode, you should also compile your plugin in Debug mode.

:point_right: the plugin must be compiled with the same architecture (32/64 bits) of Evoplex.

#### from QtCreator
* Open the `CMakeLists.txt`
* In the projects page, make sure the `EvoplexCore_DIR` is set. If it shows `EvoplexCore_DIR-NOTFOUND` and you compiled Evoplex from source (eg., at `~/evoplex/build`), set it to `~/evoplex/build/src/core/EvoplexCore/` as shown [here](https://i.imgur.com/hyKuFR3.png).
* Build

#### from command line
Assuming you placed this repository at `~/evoplex/minimal-model` and you are at `~/evoplex`, just run the commands below:
``` bash
mkdir build-plugin
cd build-plugin
cmake ../minimal-model
cmake --build .
```
When you run the `cmake` command, you might get an **error** like `FindEvoplexCore.cmake` not found. If you compiled Evoplex from source (eg., at `~/evoplex/build`), just run the command below:
``` bash
export EvoplexCore_DIR=~/evoplex/build/src/core/EvoplexCore/
```


## Running this plugin
After compiling the plugin, open Evoplex, go to the `Plugins` page, click on `import` and select the `plugin_minimal-model` file at `~/evoplex/minimal-model/build/plugin/`. The plugin will now be available in the `Projects` page.

## Support
- Ask a question in the [mailing list](https://groups.google.com/group/evoplex) (or send it directly to evoplex@googlegroups.com)
- Follow us on [Twitter](https://twitter.com/EvoplexMAS)
- Join us on our [Gitter chat channel](https://gitter.im/EvoplexMAS/evoplex)

## Licensing
This plugin is available freely under the [MIT license](https://opensource.org/licenses/MIT).
