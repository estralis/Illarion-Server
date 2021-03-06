
  Illarion

Objectives

   Illarion is the online multiplayer roleplaying game that is developed and
   maintained by the Illarion e.V. This repository contains the server 
   application.

Details

   The application in this repository is the Illarion Server. The official
   repository is https://github.com/Illarion-eV/Illarion-Server. The lead
   developer's repository is https://github.com/vilarion/Illarion-Server. 
   It works together with the Illarion Client found at
   https://github.com/Illarion-eV/Illarion-Java.

Requirements

   GCC 4.9.2 OR clang 3.5
   GNU Make 4.0
   Automake 1.14.1
   Boost 1.55.0
   PostgreSQL 9.4.3
   libpqxx 4.0.1
   Lua 5.2.3
   Luabind 0.9.1 with some bugfixes from
        http://illarion.org/repos/apt/debian (binary)
     or https://github.com/vilarion/luabind/tree/debian (source)

Build

   Run inside the source directory:
   ./bootstrap
   ./configure OR ./configure CXX=clang
   make

   for non-standard locations of luabind (i.e. in /usr/local/lib and not in /usr/lib)
   use LDFLAGS=-L/usr/local CPPFLAGS=-I/usr/local/include ./configure

   if you want to use multiple boost versions in parallel you should use the
   flag --layout=versioned to b2 on installing this version to make sure we link against
   the correct version
     ______________________________________________________________________

   Last modified: Apr 30, 2013
