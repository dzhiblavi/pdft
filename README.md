# PDFT: A Terminal PDF reader

## Rationale

As a terminal enjoyer I always wanted to be able to edit and View LaTeX documents
inside my terminal. It would be event better if that could be done inside tmux and over SSH.

To my surprise, a few projects already exist that solve the task of displaying images
inside the terminal. To list a few:

- [termpdf][termpdf] (written in b\*sh) and its cousin [termpdf-py][termpdf-py] (written in P\*thon)
- [tdf][tdf], which is written in Rust (my respect and condolences)

For me personally, these solutions lack some features as well as extensibility,
maintanability. This is however absolutely normal for any ad-hoc project (which PDFT also is).

I also saw an opportunity to get myself busy working on a reasonably complex and maybe
even useful project. I definitely learned a thing or two since I started this project.
I will be happy if anyone finds this useful.

## Features

This section describes the features that this application provides.
Most of the features are configurable.

### General

- Works with or without tmux, over SSH, in any combination
- Provides ability to open multiple docuents at once
- Saves and loads state across launches. One can launch a standalone instance keeping the state unchanged
- Filesystem changes trigger hot document reload. This may be useful when editing LaTeX documents

### Appearance

- One-page, two-pages, one- and two-page-continuous viewing layouts
- On-demand color inversion and color palettes for prettier and less tiring appearance
- On-demand text "boldification" for more comfortable reading
- Configurable image and text render quality for custom tradeoff between performance and quality

### Performance

- Incremental, on-demand document loading
- Multithreaded background image rendering
- Configurable pre-rendering of nearby images
- Both Terminal and application (RAM) LRU image caches

## Supported environments

Given a terminal that supports Kitty graphics protocol, PDFT will run in plain
terminal, over SSH, with or without tmux. Such flexibility is achieved via
using [Unicode placeholders][unicode-placeholders] feature in "unusual" environments
such as tmux and/or SSH.

### Supported OS

The following table describes OS support statuses.

| OS | Supported | Tested |
| --- | --- | --- |
| Linux | + | + |
| MacOs | + | + |
| Windows | ? | - |

### Supported terminals

Any terminal that supports [Kitty image protocol][kitty-graphics-protocol]
(Kitty version at least 0.28.0). The list of terminals can be found on Kitty website.

### Building and installation

TODO: I haven't yet figured out a way to build this in cross-platform manner.
You can try to run

```bash
make BUILD_TYPE=Release PROFILE="<choose profile in ./conan>" build
```

and see what happens next.

### Configuration

The default location of PDFT configuration is `~/.local/config/pdft/config.json`.
This can be altered via command line argument (see `pdft --help`).

The only section that might be interesting to a first-time user is `.modules.controller.config.key_mappings`.
It is a list of key mappings. It should look like this:

```json
"key_mappings": [
    ...
    {
        "command": "CloseDocument",
        "keys": "x"
    },
    ...
]
```

Feel free to modify these mappings to your liking. Current implementation supports
key mappings consisting of one or more printable characters
(ones that appear in the terminal when you type).

Commands starting with `Num` and having
`abcd` key mapping will trigger when `<number>abcd` is pressed. The number will affect
the amount of times the command will be applied, i.e. `10j` means "go 10 pages down".

### Usage

To get a description of all available command line arguments, invoke

```bash
pdft --help  # or pdft -h
```

To open documents `a.pdf` and `b.pdf`, invoke

```bash
pdft --path a.pdf --path b.pdf  # or pdft -p a.pdf -p b.pdf
```

To launch PDFT in standalone mode, i.e. in stateless mode, invoke

```bash
pdft -S Standalone
```

### Troubleshooting

By default PDFT writes logs to `~/.local/share/pdft/*.log` files. Reading the logs
of your last run looks like a good place to start investigating an issue.

PDFT is built with debug symbols present even in release configuration. So, coredumps
and stack traces can also be generated and will simplify troubleshooting greatly.

### Planned features

| Feature | Status |
| --- | --- |
| iTerm image protocol support | N/A |
| Windows OS support and testing | N/A |

### Dependencies

| Library | Why |
| --- | --- |
| [asio/1.32.0][asio] | Self-explanatory |
| [saf][saf] | Scheduler-aware futures |
| [mupdf/1.24.10][mupdf] | A powerful library for working with PDFs and other document formats |
| [ftxui/v5.0.0][ftxui] | A Terminal User Interface library |
| [fswatch/1.17.1][fswatch] | A cross-platform filesystem event watching library |
| [reflect-cpp/v0.16.0][reflect-cpp] | Serialization, Deserialization and Reflection library |
| [spdlog/1.14.1][spdlog] | Logging library |
| [zlib/1.2.11][zlib] | Used to compress images before sending images |
| [base64/0.5.2][base64] | Required to send image data via Kitty graphics protocol |
| [cpptrace/0.7.2][cpptrace] | Helps to generate stacktraces |
| [magic_enum/0.9.6][magic_enum] | Static reflection for enums |
| [tclap/1.2.5][tclap]| Command line argument parser |
| [gtest/1.15.0][gtest] | Unit testing & mocking framework |
| [catch2/3.6.0][catch2] | Unit testing framework |

[kitty-graphics-protocol]: https://sw.kovidgoyal.net/kitty/graphics-protocol/
[unicode-placeholders]: https://sw.kovidgoyal.net/kitty/graphics-protocol/#unicode-placeholders
[asio]: https://think-async.com/Asio/
[saf]: https://github.com/ashtum/saf
[mupdf]: https://mupdf.readthedocs.io/en/latest/index.html
[ftxui]: https://github.com/ArthurSonzogni/FTXUI
[fswatch]: https://github.com/emcrisostomo/fswatch
[reflect-cpp]: https://github.com/getml/reflect-cpp
[spdlog]: https://github.com/gabime/spdlog
[zlib]: https://www.zlib.net/
[base64]: https://github.com/aklomp/base64
[cpptrace]: https://github.com/jeremy-rifkin/cpptrace
[magic_enum]: https://github.com/Neargye/magic_enum
[tclap]: https://tclap.sourceforge.net/
[gtest]: https://github.com/google/googletest
[catch2]: https://github.com/catchorg/Catch2
[termpdf]: https://github.com/dsanson/termpdf
[termpdf-py]: https://github.com/dsanson/termpdf.py
[tdf]: https://github.com/itsjunetime/tdf
