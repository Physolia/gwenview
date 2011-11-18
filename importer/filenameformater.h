// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
Gwenview: an image viewer
Copyright 2009 Aurélien Gâteau <agateau@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.

*/
#ifndef FILENAMEFORMATER_H
#define FILENAMEFORMATER_H

// Qt
#include <QMap>

// KDE

// Local

class QString;

class KDateTime;
class KUrl;

namespace Gwenview
{

class FileNameFormaterPrivate;
class FileNameFormater
{
public:
    typedef QMap<QString, QString> HelpMap;

    FileNameFormater(const QString& format);
    ~FileNameFormater();

    /**
     * Given an url and its dateTime, returns a filename according to the
     * format passed to the constructor
     */
    QString format(const KUrl& url, const KDateTime& dateTime);

    /**
     * Returns a map whose keys are the available keywords and values are the
     * keyword help
     */
    static HelpMap helpMap();

private:
    FileNameFormaterPrivate* const d;
};

} // namespace

#endif /* FILENAMEFORMATER_H */
