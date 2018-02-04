
### Euler Server Change Log.

This file's intent is to record releases and changes 
to the code. 

## [1.0.0] - 2018-02-03

## Additions

	- First version of this HTTP Server.
	- Is single-threaded.
	- Uses asynchronous I/0 (i.e, it uses epoll to handle clients).
	- Named Euler. Hey, the guy was cool.

## Changes

	- Added a Compilation section to README.md.

## [1.0.0] - 2018-02-03


## Additions

	- Added `script2.sh` to `test` directory (see `script2.sh` and `script1.sh` for more info).
	- Added `build_directory.sh` to `src` directory.
	- Added `clean` rule to `Makefile`.

## Changes

	- Renamed `testr.c` to `server.c`.
	- Renamed `testf` rule to `server` in `Makefile`.

## Fixed

	- Fixed grammar in `script1.sh`. 
	- Fixed grammar in `debug.h`.
