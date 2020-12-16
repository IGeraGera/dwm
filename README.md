# dwm - dynamic window manager

dwm is an extremely fast, small, and dynamic window manager for X.

This is (my work in progress) build for dwm taken under the MIT License from [suckless website](https://dwm.suckless.org/).

## Patches

+ autostart
+ decorhints
+ dwmc
+ focusmaster
+ restartsig
+ scratchpad
+ swallow
+ vanitygaps
+ xresources


## Requirements

In order to build dwm you need the Xlib header files.  
For swallow :  
`xcb libxcb-xkb-dev x11-xkb-utils libx11-xcb-dev libxkbcommon-x11-dev libxcb-res0 libxcb-res0-dev` (Ubuntu 20.04)


## Installation

Edit config.mk to match your local setup (dwm is installed into the /usr/local namespace by default).

Afterwards enter the following command to build and install dwm (if necessary as root):

`make clean install`

Makefile is edited to delete `config.h`

## Special Additions

This build can change the color live by using the Xresources patch and an Image Color extractor like pywal.  
A new function that reads live the X server resource database has been added. It was taken from the xrdb patch.  

```C
void
livereloadxrdb(const Arg *arg)
{
        load_xresources();
        int i;
        for (i = 0; i < LENGTH(colors); i++)
                scheme[i] = drw_scm_create(drw, colors[i], 3);
        focus(NULL);
        arrange(NULL);
}
```

