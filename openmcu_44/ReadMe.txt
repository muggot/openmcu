Notes for OpenMCU
-----------------

Roger Hardiman, 20 June 2001
Updated by Craig Southeren, 6 March 2006


1. Introduction
---------------

OpenMCU is a simple Mutli Conference Unit using the H.323 protocol. It requires
the OpenH323 library - see http://www.openh323.org for more information.
It is known to run on Linux and FreeBSD and should run on any platform supported
by OpenH323.
If you run the program sucessfully on another platform them please
email to openh323@openh323.org


2. Features
-----------

The code contains the following features:

- configured by a web interface on port 1420 (use http://host:1420 access)

- requires no codec hardware to operate

- supports all plugin audio codecs supported by OpenH323

- supports H.261 and H.263 video

- can accept multiple connections simultaneously

- several different conferences can be talking place at the same time
  using the 'rooms' feature

- display statistics on calls in progress

- initiate calls from the MCU to remote endpoints

- audio loopback mode echoes back your audio in a specific room. Ideal for
  setup of audio hardware and testing network performance.


3. Operation
------------

OpenMCU works by setting up a H.323 listener process, and then waiting for
incoming connections.
Whenever an incoming connection is established, it determines which conference
is required via the 'rooms' feature and adds the call to that conference.
You call the MCU using the format "room_name@server_name"
eg using ohphone I type
  ohphone meeting1@mcu.myservers.com

New rooms are created automatically and there is a default room for
people who do not specify a room or cannot specify a room (eg NetMeeting).
The default room is called room101.

You hear the audio from the other users, but only see the video from the
four users activly talking.

4. Command line options
-----------------------

See the man page for command line options

                              ------------------------
