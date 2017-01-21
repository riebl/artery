# Vanetza socket demo

*socktap* runs Vanetza on top of Linux raw packet sockets and demonstrates the basic API usage.
This enables tests on commodity hardware, i.e. no special V2X or Car2X hardware is required.
However, raw packet sockets cannot replace such dedicated hardware entirly.
Consider *socktap* as an experimental application showcasing some of Vanetza's features.

## Permissions

Since *socktap* builds upon raw packet sockets you need to run it with special privileges.
Either run *socktap* as root user or set the CAP\_NET\_RAW capabilities on the executable.
You can do this via `sudo setcap cap_net_raw+ep bin/socktap`.
When CAP\_NET\_RAW is attached to the *socktap* binary you can run it as ordinary user.

## Running

You can locate *socktap* in your build directory at **bin/socktap**.
It requires the network device name as startup argument on which *socktap* should send and receive packets.
Usually, such devices are named *eth0* or *wlan0*.
You can look up the available devices on your machine with the `ip link` command.

# Acknowledgement

This demo application has been initially developed as part of a student's project at Hochschule Darmstadt in summer term 2016.
Participating students were in alphabetical order: Sachin Kashyap Bukkambudhi Satyanarayana, Alvita Marina Menezes, Mrunmayi Parchure, Subashini Rajan and Deeksha Venkadari Yogendra.
