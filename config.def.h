/* See LICENSE file for copyright and license details. */
/* Suppoer for multimedia keys */
#include<X11/XF86keysym.h>
/* appearance */
static unsigned int borderpx  = 5;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0; /* 1 means swallow floating windows by default */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static const unsigned int gappih    = 6;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 6;       /* vert inner gap between windows */
static const unsigned int gappoh    = 5;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 5;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static const char *fonts[]          = { "monospace:size=14" };
static const char dmenufont[]       = "monospace:size=14";
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     		instance  		title           tags mask  iscentered	isfloating  isterminal  noswallow  monitor */
	{ "Gimp",    		NULL,     		NULL,           0,         1,		1,          0,           0,        -1 },
	{ "firefox", 		"Navigator",    	NULL,           1 << 8,    0,		0,          0,          -1,        -1 },
	{ "firefox", 		"Firefox",     		NULL,           0,	   1,		1,          0,          -1,        -1 },
	{ "st-256color",	NULL,     		NULL,           0,         0,		0,          1,           0,        -1 },
	{ "Alacritty",		NULL,     		NULL,           0,         0,		0,          1,           0,        -1 },
	{ "Org.gnome.Nautilus",	"org.gnome.Nautilus",   NULL,           0,         1,		1,          0,           0,        -1 },
	{ "File-roller",	"file-roller",   	NULL,           0,         1,		1,          0,           0,        -1 },
	{ NULL,      		NULL,     		"Event Tester", 0,         0,		0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int decorhints  = 1;    /* 1 means respect decoration hints */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, /* View specific Tag */ \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, /* Toggle Tag view */ \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, /* Move focus to Tag */ \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} }, /* Toggle tag on focus */

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }
/* Terminal definition */
#define TERM "kitty"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { TERM, NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "kitty", "-t", scratchpadname, "-d", "120","34", NULL };
static const char *tuifilemancmd[] = { TERM, "-e", "bash","-c","$HOME/bin/readWal; ranger","NULL"};
static const char *guifilemancmd[] = { "pcmanfm",NULL};
static const char *browsercmd[] = { "firefox", NULL  };
static const char *emailclient[] = { "thunderbird", NULL  };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          	INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	FLOAT,   &mfact },
};

static Key keys[] = {
	/* modifier                     key        function        argument */

	/*{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },			 Quit DWM  */
	{ MODKEY|ShiftMask,             XK_q,      spawn,  SHCMD("$HOME/.dwm/exit.conf") },/* Quit DWM  */
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} }, 			/* Restart DWM */
	{ MODKEY,			XK_w,	   spawn,	   {.v = browsercmd } },	/* Launch Browser (Firefox) */
	{ MODKEY,			XK_e,	   spawn,	   {.v = emailclient} },	/* Launch email client (Thunderbird)*/
	/* r */
	{ MODKEY,			XK_t,      setlayout,      {.v = &layouts[0]} },	/* Tile - Layout */
	{ MODKEY|ShiftMask,		XK_t,      setlayout,      {.v = &layouts[4]} },	/* Deck - Layout */
	{ MODKEY,			XK_y,      setlayout,      {.v = &layouts[2]} },	/* Spiral - Layout */
	{ MODKEY|ShiftMask,		XK_y,      setlayout,      {.v = &layouts[3]} },	/* Dwindle - Layout */
	{ MODKEY,			XK_u,      setlayout,      {.v = &layouts[5]} },	/* Backstack - Layout */
	{ MODKEY|ShiftMask,		XK_u,      setlayout,      {.v = &layouts[6]} },	/* Backstackhoriz - Layout */
	{ MODKEY,			XK_u,      setlayout,      {.v = &layouts[5]} },	/* Backstack - Layout */
	{ MODKEY|ShiftMask,		XK_u,      setlayout,      {.v = &layouts[6]} },	/* Backstackhoriz -  Layout */
	{ MODKEY,			XK_i,      setlayout,      {.v = &layouts[7]} },	/* Grid - Layout */
	{ MODKEY|ShiftMask,		XK_i,      setlayout,      {.v = &layouts[8]} },	/* nrowgrid - Layout */
	{ MODKEY,			XK_o,      setlayout,      {.v = &layouts[9]} },	/* HorizGrid - Layout */
	{ MODKEY|ShiftMask,		XK_o,      setlayout,      {.v = &layouts[10]} },	/* gaplessgrid - Layout */
	{ MODKEY,			XK_p,      setlayout,      {.v = &layouts[11]} },	/* Center master - Layout */
	{ MODKEY|ShiftMask,		XK_p,      setlayout,      {.v = &layouts[12]} },	/* Floating Center Master -  Layout */
	/* y u  i p*/
	/* a s */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },		/* Launch dmenu */
	{ MODKEY|ShiftMask,             XK_d,      spawn,   SHCMD("$HOME/.dwm/scripts/todo") },	/* Launch ToDo */
	{ MODKEY,			XK_f,	   spawn,	   {.v = tuifilemancmd } },	/* Launch Text File Manager (Ranger) */
	{ MODKEY|ShiftMask,		XK_f,	   spawn,	   {.v = guifilemancmd } },	/* Launch Text File Manager (Ranger) */
	/* g */
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },		/* Widen to Left - Focus */
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },		/* Move Forward Stack - Focus */
	{ MODKEY|ShiftMask,		XK_j,	   focusmaster,    {0} },			/* Move focus to Master - Focus */
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },		/* Move Back Stack - Focus */
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },		/* Widen to Right - Focus */
	{ MODKEY|ShiftMask,             XK_l,      spawn,       SHCMD( "slock" )},		/* Lock Screen with Slock*/
	/* z x c v */
	{ MODKEY,                       XK_b,      togglebar,      {0}  },			/* Toggle Status Bar  */
	{ MODKEY|ShiftMask,             XK_b,      spawn,    SHCMD("$HOME/.dwm/scripts/theme/dwmThemeMenu.sh")},/* Theme Menu  */
	{ MODKEY,			XK_n,      setlayout,      {.v = &layouts[13]}    },	/* Float - Layout */
	{ MODKEY|ShiftMask,             XK_n,  	   togglefloating, {0} }, 			/* Toggle Floating - Layout*/
	{ MODKEY,			XK_m,      setlayout,      {.v = &layouts[1]}    },	/* Monocle - Layout*/
	{ MODKEY|ShiftMask,		XK_m,      spawn,      	   SHCMD("$HOME/.dwm/scripts/monitor/dmenu_monitor.sh")}, /* Monitor Settings Menu - Monitor*/
	/*Left symbols*/
	{ MODKEY,                       XK_Tab,    view,           {0} }, 			/*Switch to Last Tag - Tag   */
	{ MODKEY|ShiftMask,             XK_Tab,    livereloadxrdb, {0} }, 			/*Switch to Last Tag - Tag   */
	/*Space*/
	{ MODKEY,                       XK_space,    spawn,         SHCMD("kill -35 $(pidof dwmblocks)") }, /*Switch Keyboard Language*/
	/*Right symbols*/
	{ MODKEY|ShiftMask,	     XK_BackSpace, killclient,     {0} },			/* Kill Focus Focus  */
	{ MODKEY,                       XK_Return, zoom,           {0} },			/* Switch to Focus  */
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },		/* Spawn Terminal */

	{ MODKEY,                       XK_bracketleft,       incnmaster,     {.i = -1 } },  	/* Increase Master Stack - Layout  */
	{ MODKEY,                       XK_bracketright,      incnmaster,     {.i = +1 } },	/* Decrease Master Stack - Layout  */
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },		/* Select Next Monitor - Monitor  */
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },		/* Select Previous Monitor - Monitor  */
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },		/* Move focus to Next Monitor - Monitor */
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },		/* Move focus to Previous Monitor - Monitor */
	{ MODKEY|ShiftMask,             XK_slash,  spawn,         SHCMD("$HOME/.dwm/scripts/dwmKeyMenu.sh $HOME/Misc/dwm/config.def.h") }, /* ? dwmKeyMenu - Info */


	/* Top Number Line*/
	{ MODKEY,                       XK_grave,  togglescratch,  {.v = scratchpadcmd } },	/* Toggle scratchpad Terminal */
	{ MODKEY|ShiftMask,             XK_grave,  spawn,          SHCMD("pgrep -x ckb-next &&  ckb-next -c || ckb-next -b ")}, /* Toggle ckb-next - Keyboard  */

	TAGKEYS(                        XK_1,                      0) /* General tag keys*/
	TAGKEYS(                        XK_2,                      1) /* General tag keys*/
	TAGKEYS(                        XK_3,                      2) /* General tag keys*/
	TAGKEYS(                        XK_4,                      3) /* General tag keys*/
	TAGKEYS(                        XK_5,                      4) /* General tag keys*/
	TAGKEYS(                        XK_6,                      5) /* General tag keys*/
	TAGKEYS(                        XK_7,                      6) /* General tag keys*/
	TAGKEYS(                        XK_8,                      7) /* General tag keys*/
	TAGKEYS(                        XK_9,                      8) /* General tag keys*/

	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },		/* Select All Tags */
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },               /* Tag focus to All Tags */
	{ MODKEY|ShiftMask|ControlMask, XK_0,      togglegaps,     {0} },                       /* Toggle Gaps  */
	/* XF keys multimedia etc */

	{ 0,              XF86XK_AudioMute,             spawn,     SHCMD("~/.dwm/scripts/sound/volumeHandler 'TOGGLE' && kill -36 $(pidof dwmblocks)") },  /* Toggle Mute - Volume*/
	{ 0,              XF86XK_AudioPlay,             spawn,     SHCMD("playerctl play-pause") },  /* Toggle Play -Pause */
	{ 0,              XF86XK_AudioRaiseVolume,      spawn,     SHCMD("~/.dwm/scripts/sound/volumeHandler 'UP' && kill -36 $(pidof dwmblocks)") },     /* Increase Volume +5% - Volume*/
	{ 0,              XF86XK_AudioLowerVolume,      spawn,     SHCMD("~/.dwm/scripts/sound/volumeHandler 'DOWN' && kill -36 $(pidof dwmblocks)") },     /* Decrease Volume -5% - Volume*/
	/*vanity gaps bindings	*/

	/*{ MODKEY|Mod4Mask,              XK_u,      incrgaps,       {.i = +1 } },*/
	/*{ MODKEY|Mod4Mask|ShiftMask,    XK_u,      incrgaps,       {.i = -1 } },*/
	/*{ MODKEY|Mod4Mask,              XK_i,      incrigaps,      {.i = +1 } },*/
	/*{ MODKEY|Mod4Mask|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } },*/
	/*{ MODKEY|Mod4Mask,              XK_o,      incrogaps,      {.i = +1 } },*/
	/*{ MODKEY|Mod4Mask|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } },*/
	/*{ MODKEY|Mod4Mask,              XK_6,      incrihgaps,     {.i = +1 } },*/
	/*{ MODKEY|Mod4Mask|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } },*/
	/*{ MODKEY|Mod4Mask,              XK_7,      incrivgaps,     {.i = +1 } },*/
	/*{ MODKEY|Mod4Mask|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } },*/
	/*{ MODKEY|Mod4Mask,              XK_8,      incrohgaps,     {.i = +1 } },*/
	/*{ MODKEY|Mod4Mask|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } },*/
	/*{ MODKEY|Mod4Mask,              XK_9,      incrovgaps,     {.i = +1 } },*/
	/*{ MODKEY|Mod4Mask|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } },*/
	/*{ MODKEY|Mod4Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },*/

	/*Must check those*/

	/*{ MODKEY,                       XK_space,  setlayout,      {0} },*/
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

void
setlayoutex(const Arg *arg)
{
	setlayout(&((Arg) { .v = &layouts[arg->i] }));
}

void
viewex(const Arg *arg)
{
	view(&((Arg) { .ui = 1 << arg->ui }));
}

void
viewall(const Arg *arg)
{
	view(&((Arg){.ui = ~0}));
}

void
toggleviewex(const Arg *arg)
{
	toggleview(&((Arg) { .ui = 1 << arg->ui }));
}

void
tagex(const Arg *arg)
{
	tag(&((Arg) { .ui = 1 << arg->ui }));
}

void
toggletagex(const Arg *arg)
{
	toggletag(&((Arg) { .ui = 1 << arg->ui }));
}

void
tagall(const Arg *arg)
{
	tag(&((Arg){.ui = ~0}));
}
void
reloadxrdb(const Arg *arg)
{
	livereloadxrdb(NULL);
}
/* signal definitions */
/* signum must be greater than 0 */
/* trigger signals using `xsetroot -name "fsignal:<signame> [<type> <value>]"` */
static Signal signals[] = {
	/* signum           function */
	{ "focusstack",     focusstack },
	{ "setmfact",       setmfact },
	{ "togglebar",      togglebar },
	{ "incnmaster",     incnmaster },
	{ "togglefloating", togglefloating },
	{ "focusmon",       focusmon },
	{ "tagmon",         tagmon },
	{ "zoom",           zoom },
	{ "view",           view },
	{ "viewall",        viewall },
	{ "viewex",         viewex },
	{ "toggleview",     view },
	{ "toggleviewex",   toggleviewex },
	{ "tag",            tag },
	{ "tagall",         tagall },
	{ "tagex",          tagex },
	{ "toggletag",      tag },
	{ "toggletagex",    toggletagex },
	{ "killclient",     killclient },
	{ "quit",           quit },
	{ "setlayout",      setlayout },
	{ "setlayoutex",    setlayoutex },
	{ "reloadxrdb",     reloadxrdb },
};
