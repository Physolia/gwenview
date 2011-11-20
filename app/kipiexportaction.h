// vim: set tabstop=4 shiftwidth=4 expandtab:
/*
Gwenview: an image viewer
Copyright 2010 Aurélien Gâteau <agateau@kde.org>

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
#ifndef KIPIEXPORTACTION_H
#define KIPIEXPORTACTION_H

// Qt

// KDE
#include <ktoolbarpopupaction.h>

// Local

namespace Gwenview
{

class KIPIInterface;

class KIPIExportActionPrivate;
class KIPIExportAction : public KToolBarPopupAction
{
    Q_OBJECT
public:
    KIPIExportAction(QObject* parent);
    ~KIPIExportAction();

    void setKIPIInterface(KIPIInterface*);

private Q_SLOTS:
    void init();
    void setDefaultAction(QAction*);

private:
    KIPIExportActionPrivate* const d;
};

} // namespace

#endif /* KIPIEXPORTACTION_H */
