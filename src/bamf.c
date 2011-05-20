/*
 * Copyright 2010 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option) any later
 * version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <libbamf/bamf-matcher.h>
#include <string.h>

char *getName(char *deskfile)
{
    char *temp;
    temp = strdup(strrchr(deskfile, '/'));
    return strndup(temp + 1, strlen(temp) - 9);
}

char *getCurrentApp()
{
    g_type_init();
    char *deskfile, *appName;
    char *temp;

    BamfApplication *app =
        bamf_matcher_get_active_application(bamf_matcher_get_default());
    if (app) {
        appName = (char *) bamf_view_get_name(BAMF_VIEW(app));
        temp = bamf_application_get_desktop_file(app);

        if (strchr(appName, ' ') && temp && strlen(temp) > 1)
            return getName((char *) temp);
        else
            return appName;
    } else
        return "";

}
