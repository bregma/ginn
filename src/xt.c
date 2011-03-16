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

static Display *disp = NULL;

void openDisplay()
{
    disp = XOpenDisplay(NULL);
}

void closeDisplay()
{
    XCloseDisplay(disp);
}

void movePointer(int x, int y)
{
    XTestFakeMotionEvent(disp, 0, x, y, CurrentTime);
}

void injKey(KeySym ks, char *modifiers[])
{
    int i;

    for (i = 0; i < 4 && 0 != strcmp(modifiers[i], ""); i++)
        XTestFakeKeyEvent(disp,
                          XKeysymToKeycode(disp,
                                           XStringToKeysym(modifiers
                                                           [i])), True,
                          CurrentTime);
    XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, ks), True, CurrentTime);
    XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, ks), False,
                      CurrentTime);
    for (i = 0; i < 4 && 0 != strcmp(modifiers[i], ""); i++)
        XTestFakeKeyEvent(disp,
                          XKeysymToKeycode(disp,
                                           XStringToKeysym(modifiers
                                                           [i])), False,
                          CurrentTime);

    XFlush(disp);
}

void injButton(int btn, char *modifiers[])
{
    int i;

    for (i = 0; i < 4 && 0 != strcmp(modifiers[i], ""); i++)
        XTestFakeKeyEvent(disp,
                          XKeysymToKeycode(disp,
                                           XStringToKeysym(modifiers
                                                           [i])), True,
                          CurrentTime);
    XTestFakeButtonEvent(disp, btn, True, CurrentTime);
    XTestFakeButtonEvent(disp, btn, False, CurrentTime);
    for (i = 0; i < 4 && 0 != strcmp(modifiers[i], ""); i++)
        XTestFakeKeyEvent(disp,
                          XKeysymToKeycode(disp,
                                           XStringToKeysym(modifiers
                                                           [i])), False,
                          CurrentTime);

    XFlush(disp);
}

void injMixBtnKey(KeySym ks, int btn, char *modifiers[])
{
    int i;

    XTestFakeButtonEvent(disp, btn, True, CurrentTime);
    for (i = 0; i < 4 && 0 != strcmp(modifiers[i], ""); i++)
        XTestFakeKeyEvent(disp,
                          XKeysymToKeycode(disp,
                                           XStringToKeysym(modifiers
                                                           [i])), True,
                          CurrentTime);
    XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, ks), True, CurrentTime);
    XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, ks), False,
                      CurrentTime);
    for (i = 0; i < 4 && 0 != strcmp(modifiers[i], ""); i++)
        XTestFakeKeyEvent(disp,
                          XKeysymToKeycode(disp,
                                           XStringToKeysym(modifiers
                                                           [i])), False,
                          CurrentTime);
    XTestFakeButtonEvent(disp, btn, False, CurrentTime);
    XFlush(disp);
}
