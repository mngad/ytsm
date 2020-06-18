# YTSM

Ncurses viewer and watcher (through mpv) for youtube subscriptions.

## Use

Show key bindings `?`

Download the OPML file by clicking Export subscriptions at the bottom of your [Manage Subscriptions](https://www.youtube.com/subscription_manager).

or

Put a list of channel urls in a channelUrlList.conf file. Both should be kept in the same folder as the executable.

Compiles in Gentoo, ncurses is finickity, probably remove -ltinfo from the LIBS line of the makefile for non-gentoo systems where the two libraries aren't split.s

## Requirements

- `wget` `mpv`

# TODO
- [x] implement a new video icon
- [x] implement a watched icon
- [ ] catagories
- [x] oplm file import?
- [x] key binding help guide/ better  keybndings
- [ ] fix locations (move to .config)
- [ ] Makefile (chmod +x the shell scripts)
- [ ] Makefile create cache folder
- [x] Alphabetise the sub list
- [x] Pageup and down + scroll
- [ ] Add search
- [ ] ~~Add video length~~ Not possible from RSS
