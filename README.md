
# DLWigglz plugin

DLWigglz provides a range of experimental, stack-level extensions to VCV Rack that
are workflow enhancements.

The first module in the DLwigglz family is **r4xH4x**, a cataloging module.

Usage is simple: 1) click the `catalog (partial)` button and it will spin through
all the installed plugin modules and generates a
JSON file named `catalog.partial.json` with everything derivable from your
installed plugin folder less width data, or 2) click the `patch` button to save
the current patch as JSON with width data as `patch.json`. Both files are useful with [rackcli's](https://github.com/dirkleas/rackcli) cataloging services (see
**Example Usage -- Live Features**). The extra input jack is for testing and does
nothing interesting (yet).

![DLwigglz r4xH4x](https://user-images.githubusercontent.com/52076/41833055-c6941908-781c-11e8-8305-0a281394ee92.png){: height="50%" width="50%"}

Many thanks to Xavier Belmont for the new r4xH4x panel design!

--

**Licenses**

All concepts and source code in this repository is licensed under [BSD-3-Clause](LICENSE) by Dirk Leas.
