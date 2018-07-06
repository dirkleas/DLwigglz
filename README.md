
# DLWigglz plugin

DLWigglz provides a range of experimental, stack-level extensions to VCV Rack that
are workflow enhancements.

The first module in the DLwigglz family is **r4xH4x**, a cataloging module.

Usage is simple: 1) click the `catalog (partial)` button and it will spin through
all the installed plugin modules and generates a
JSON file named `catalog.partial.json` with everything derivable from your
installed plugin folder less width data, or 2) click the `patch` button to save
the current patch as JSON with width data as `patch.json`. Both files are useful with [rackcli's](https://github.com/dirkleas/rackcli) cataloging services (see
**Example Usage -- Live Features**).

The input jack at the bottom can be used to trigger the buttons with CV as follows:

1. 1-5V triggers `patch`
1. \>5-10V triggers `catalog (partial)` and `patch`

I like to use **Frozen Wasteland's Seriously Slow LFO** set to 100.0 minutes wired to
**Bogaudio's OFFSET** to adjust the CV above/below 5V wired into **r4xh4x** input jack. This
is what I use to iterate through all my installed plugins and generate screenshots,
capture width data without using my fork, etc..

<img src="https://user-images.githubusercontent.com/52076/41833055-c6941908-781c-11e8-8305-0a281394ee92.png" width="15%" height="15%"></img>

Many thanks to Xavier Belmont for the new r4xH4x panel design!

--

**Licenses**

All concepts and source code in this repository is licensed under [BSD-3-Clause](LICENSE) by Dirk Leas.
