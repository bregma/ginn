/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Mohamed-Ikbel Boulabiar <boulabiar@gmail.com>
 */

#include <X11/X.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>

void
injKey(KeySym ks, char * modifiers[])
{
	int i;
        Display* disp = XOpenDisplay(NULL);
	for (i=0 ; i<4 && 0!=strcmp(modifiers[i],"") ; i++)
		XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, XStringToKeysym(modifiers[i])),  True, CurrentTime);
        XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, ks),  True, CurrentTime);
        XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, ks), False, CurrentTime);
	for (i=0 ; i<4 && 0!=strcmp(modifiers[i],"") ; i++)
		XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, XStringToKeysym(modifiers[i])),  False, CurrentTime);

        XCloseDisplay(disp);
}

void
injButton(int btn, KeySym modifier)
{
        Display* disp = XOpenDisplay(NULL);
        XTestFakeButtonEvent(disp, btn, True, CurrentTime);
        XTestFakeButtonEvent(disp, btn, False, CurrentTime);
        XCloseDisplay(disp);
}

static void
pressKey(KeySym ks, Display *disp)
{
  XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, ks), True, CurrentTime);
}

static void
releasekey(KeySym ks, Display *disp)
{
  XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, ks), False, CurrentTime);
}

static void
injectSymbols()
{
 Display* disp = XOpenDisplay(NULL);
 XCloseDisplay(disp);
}

