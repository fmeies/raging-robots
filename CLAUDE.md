# Working on RagingRobots

A 3D multiplayer game from 2000, kept building and running on current systems.
Build instructions and controls are in README.md; this file covers what the code
does not show.

## Nothing runs before `make install`

`DATA_DIR_PREFIX` is baked in at compile time, and the game reads its textures
and `rooms.data` from `$PREFIX/share/ragingrobots`. A freshly built binary in
`ragingrobots/Darwin/` will not start. To try a build without writing to
`/usr/local`:

    make PREFIX=/some/scratch/dir apps install

Start `rwserver` first (port 7986), then up to three clients.

## The build system is load-bearing and old

- `./configure` at the top is a wrapper: it creates `config-$(uname)/`, symlinks
  `config/*` into it, and runs the real `configure` there. That directory is
  generated and gitignored, as are the `Darwin/` and `Linux/` object directories.
- `rules/` is the actual engine — deleting any of it breaks the build.
  `needs.mk` turns `NEEDS = GLUT OPENGL GLU` into compiler and linker flags.
- `machine/GNU-C++` holds compiler-family flags and is only included when
  configure detects a GNU-compatible compiler. `-Wall` lives there as
  `CXX_WARN_FLAGS`; `rules/compile.mk` pulls it into `ALL_CXXFLAGS`.
- `config/configure` is generated but committed on purpose, so nobody needs
  autoconf. If you ever regenerate it, check `config/standard.m4` first — the two
  have drifted apart before.
- `AC_FIND_LIB` expects `<prefix>/include` next to `<prefix>/lib`. It does not
  know about Debian multiarch directories, which is why CI assembles a small tree
  of symlinks and passes `--with-opengl=` and friends.

## The wire protocol is fragile by design

Packets are raw structs written straight to the socket: no byte order handling,
no packing. Both ends must be built for the same ABI.

- The enum values in `ragingrobots/network.h` and `rwserver/rwserver.h` are
  written out explicitly because they travel on the wire. Do not renumber them.
- Those two headers duplicate the protocol. Change one, change the other.
- `MAXPLAYERS` is likewise defined twice, in `ragingrobots/gamemodel.h` and
  `rwserver/rwserver.h`.
- The server overwrites the `player` field when it relays, so a client has no
  meaningful number of its own to send.

## There is no test suite

Verification means building and running the real thing. A few things that are
awkward and worth knowing before you rediscover them:

- The client needs an X server; on macOS that is XQuartz.
- There is no `xdotool`. Synthetic key events work through a small Xlib program
  using `XSendEvent`, and freeglut accepts them.
- Neither ImageMagick nor netpbm is installed, so screenshots mean `xwd` plus a
  hand-written XWD-to-PNG converter.
- To drive the game without a second human: connect a plain socket to the server
  and send `c_position` packets. Field offsets come from `offsetof`, not guesses.
- To measure how often the client acts, connect a passive socket and count the
  position packets the server relays — one per movement step.

## Style

The code is C++ as it was written in 2000, and the point of the project is to
keep it alive rather than to modernise it. Match the surrounding style; when a
fix and a rewrite are both possible, take the fix.

The socket layer on both sides was adapted from a third-party C example whose
origin is no longer known — see the Credits section in README.md.
