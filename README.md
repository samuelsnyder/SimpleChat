# SimpleChat

Simple command line chat server and client

# Compilation 

`g++ chatserve.c -o chatserve`

`javac ChatClient.java `

# Execution

## Host

`./chatserve #PORT`

(#HOST can be "localhost"):

## Client 

`java ChatClient #HOST #PORT`

# Control

Enter a handle in ChatClient as promted.
When programs have connected, they may trade messages in turn, beginning with ChatClient.
"/quit" is the exit command for both programs.
