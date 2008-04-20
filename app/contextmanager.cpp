/*
Gwenview: an image viewer
Copyright 2007 Aurélien Gâteau <aurelien.gateau@free.fr>

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
#include "contextmanager.moc"

// KDE
#include <kfileitem.h>

// Local
#include "sidebar.h"
#include "abstractcontextmanageritem.h"

namespace Gwenview {

ContextManager::ContextManager(QObject* parent)
: QObject(parent)
, mDirModel(0) {}


ContextManager::~ContextManager() {
	Q_FOREACH(AbstractContextManagerItem* item, mList) {
		delete item;
	}
}


void ContextManager::setSideBar(SideBar* sideBar) {
	mSideBar = sideBar;
}


void ContextManager::addItem(AbstractContextManagerItem* item) {
	Q_ASSERT(mSideBar);
	mList << item;
	item->setSideBar(mSideBar);
}


void ContextManager::setContext(const KUrl& currentUrl, const KFileItemList& selection) {
	mCurrentUrl = currentUrl;
	mSelection = selection;
	selectionChanged();
}


KFileItemList ContextManager::selection() const {
	return mSelection;
}


void ContextManager::setCurrentDirUrl(const KUrl& url) {
	if (url.equals(mCurrentDirUrl, KUrl::CompareWithoutTrailingSlash)) {
		return;
	}
	mCurrentDirUrl = url;
	currentDirUrlChanged();
}


KUrl ContextManager::currentDirUrl() const {
	return mCurrentDirUrl;
}


KUrl ContextManager::currentUrl() const {
	return mCurrentUrl;
}


QString ContextManager::currentUrlMimeType() const {
	/*
	if (mDocumentView->isVisible() && !mDocumentView->isEmpty()) {
		return MimeTypeUtils::urlMimeType(mDocumentView->url());
	} else {
		QModelIndex index = mThumbnailView->currentIndex();
		if (!index.isValid()) {
			return QString();
		}
		KFileItem item = mDirModel->itemForIndex(index);
		Q_ASSERT(!item.isNull());
		return item.mimetype();
	}
	*/
	// FIXME
	Q_FOREACH(const KFileItem& item, mSelection) {
		if (item.url() == mCurrentUrl) {
			return item.mimetype();
		}
	}
	return QString();
}


SortedDirModel* ContextManager::dirModel() const {
	return mDirModel;
}


void ContextManager::setDirModel(SortedDirModel* dirModel) {
	mDirModel = dirModel;
}
} // namespace
