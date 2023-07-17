# Bos Wars

## About

Bos Wars is a futuristic real-time strategy game featuring a dynamic
rate-based economy. Resources are continuously produced while also being
consumed by creating buildings and training new units. Bos Wars aims to
create a completely original and fun open source RTS game.

In a real time strategy game (RTS) the player must combat his enemies
while running his war economy. Everything runs in so-called real-time.
The player actions apply immediately (which is in opposition to
turn-based games where the player always has to wait for his turn). The
main difficulties in RTS games are to balance the effort put into
building the economy, and building an army to defend and attack the
enemies.

Bos Wars has a dynamic rate based economy. Energy is produced by power
plants, and magma gets pumped from hot spots. Buildings and mobile units
are also built at a continuous rate. Control of larger parts of the map
creates the potential to increase your economy throughput. Holding key
points like roads and passages allow for different strategies.

The newest version of Bos Wars can always be found at the
[boswars.org](https://www.boswars.org) website.


## Copyright and Licence

This work, Bos Wars, is copyrighted in 2004-2023 by
Tina Petersen Jensen, Francois Beerten et al.

Read the file COPYRIGHT.txt for the detailed copyright notice.


## Run Bos Wars

Builds of Bos Wars can be obtained in different ways. The Download section
on the [Bos Wars website](https://www.boswars.org) has packages for amd64/x86_64
Linux systems. You will need to install a minimal set of dependencies.
For example on Debian:

    sudo apt install libsdl2-2.0 libz libgl1-mesa

And you can start the game:

    cd boswars-DATE
    ./boswars

The program has different command line options, which can be listed with:

    ./boswars --help

There is also the option to build from source code as explained in
[INSTALL.md](INSTALL.md). That page also contains important tips for
packaging Bos Wars for distribution.


## Multiplayer Games

Although Bos Wars supports multiplayer games over a LAN or the Internet, you
should only run network games on networks with exclusively fully trusted
parties. There are absolutely no security guarantees. Run at your own risk.

To play multiplayer, find the hostname or the IP address of someone you
want to play with and connect to him. There's currently no game server,
meta server or lobby yet. It's on the long term roadmap. You can join
our Matrix lobby room to find other players much more easily, and also
the [Matrix Matchmaking
room](https://matrix.to/#/#libregaming-matchmaking:tchncs.de) (after
joining the Matchmaking room, enter `.add boswars` and you'll be
notified if someone enters `.match boswars`).

Bos Wars uses UDP with port 6660 by default, you may start boswars
with '-Pxxxx' flag to use port xxxx instead.


## Project

For any more documentation, questions, constructive feedback,
contributions or thanks, see the [Bos Wars
website](https://www.boswars.org/). The website explains how to talk
with team (like the [Matrix
rooms](https://matrix.to/#/#boswars:matrix.org)) and how to help on the
project with new maps, music, translations, patches, feedback

[Known bugs](https://codeberg.org/boswars/boswars/issues) are listed on
the trackers of the [Bos Wars](https://www.boswars.org) project pages.
The bug tracker is for clear, precise issues with running or building
the game. For all other issues, it's best to first talk with the team.


Have fun,

The Bos Wars team.
