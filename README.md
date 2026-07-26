# RagingRobots — 3D Multiplayer Game

(c) 2000 by Frank Meies

---

RagingRobots was written in 2000 and has not been developed further since. It is
merely kept alive: the sources are adjusted from time to time so that they still
build and run on current systems. Nothing beyond that is planned — no new
features, no rewrite. What you find here is the game as it was, still working.

RagingRobots requires an implementation of OpenGL, plus GLU and GLUT.

On macOS with Homebrew:

    brew install mesa mesa-glu freeglut

Then build both the game and the server:

    CXXFLAGS="-O3" CFLAGS="-O3" ./configure \
        --with-opengl=/opt/homebrew \
        --with-glut=/opt/homebrew \
        --with-glu=/opt/homebrew \
        --x-includes=/opt/homebrew/include \
        --x-libraries=/opt/homebrew/lib
    make apps

The game looks for its data files under `$PREFIX/share/ragingrobots`, so it has
to be installed before it will start:

    make install

This puts `ragingrobots` and `rwserver` into `$PREFIX/bin` (`/usr/local` by
default) and the textures and scene data next to them.

First you have to start the server, then you can join the game with up to three
clients (increase MAXPLAYERS in the sources to get more possible clients — it is
defined twice, in `ragingrobots/gamemodel.h` and in `rwserver/rwserver.h`, and
both have to agree):

    rwserver &
    ragingrobots [hostname]

The server listens on port 7986. The client connects to localhost if no host is
given on the command line.

Hint: Reduce window size if performance is poor.

## Control Keys

| Key       | Action                 |
|-----------|------------------------|
| `i` / `m` | Move forward, backward |
| `j` / `k` | Turn left, right       |
| `y` / `x` | Slide left, right      |
| Space     | Fire                   |

## Scenes

The `rooms.data` file can be modified to create new scenes. The syntax is quite
simple:

    total number of rooms
    center of first room x-coordinate
    center of first room z-coordinate
    number of neighbor rooms
    index of first neighbor
    direction of first neighbor (0 = North, 1 = South, 2 = West, 3 = East)
    index of second neighbor
    direction of second neighbor
    ..
    center of second room x-coordinate
    center of second room z-coordinate
    number of neighbor rooms
    index of first neighbor
    direction of first neighbor
    index of second neighbor
    direction of second neighbor
    ..
    ..

## historic/

The `historic/` directory keeps two files from back then unchanged: the original
README, and the Linux Software Map entry of the last release, 0.3.7. The game was
called RoboWar until 2001 or 2002, which is where the server binary gets its name.

## Credits

The socket handling on both sides — `rwserver/rwserver.cpp` and
`ragingrobots/network.cpp` — started out as someone else's C example that I ported
to C++ and built on. I can no longer identify where it came from. If you recognise
it, I would be glad to give proper credit.

The wall and ground textures are Bricks097 and Tiles141 from
[ambientCG](https://ambientcg.com/), licensed CC0 1.0 Universal. They replace the
photographic textures the game shipped with in 2000, whose origin I could no
longer establish.
