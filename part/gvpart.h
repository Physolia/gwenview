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
#ifndef GVPART_H
#define GVPART_H

// Qt
#include <QPoint>

// Local
#include "../lib/imageviewpart.h"

class KAboutData;
class KAction;

class QLabel;
class QSlider;

namespace KParts {
class StatusBarExtension;
}

namespace Gwenview {

class ImageView;
class ScrollTool;

class GVPart : public ImageViewPart {
	Q_OBJECT
public:
	GVPart(QWidget* parentWidget, QObject* parent, const QStringList&);

	static KAboutData* createAboutData();

	virtual Document::Ptr document();

	virtual ImageView* imageView() const;

public Q_SLOTS:
	virtual void loadConfig();

protected:
	virtual bool openFile();
	virtual bool openUrl(const KUrl&);
	virtual bool eventFilter(QObject*, QEvent*);

private Q_SLOTS:
	void setZoomToFit(bool);
	void zoomActualSize();
	void zoomIn(const QPoint& center = QPoint(-1,-1));
	void zoomOut(const QPoint& center = QPoint(-1,-1));
	void setViewImageFromDocument();
	void updateCaption();
	void showContextMenu();
	void slotImageRectUpdated(const QRect&);
	void saveAs();
	void showJobError(KJob*);
	void initStatusBarExtension();
	void applyZoomSliderValue();
	void slotZoomChanged();

private:
	ImageView* mView;
	Document::Ptr mDocument;
	KAction* mZoomToFitAction;
	bool mGwenviewHost;
	ScrollTool* mScrollTool;
	QList<qreal> mZoomSnapValues;
	KParts::StatusBarExtension* mStatusBarExtension;

	QWidget* mStatusBarWidgetContainer;
	QLabel* mZoomLabel;
	QSlider* mZoomSlider;

	void disableZoomToFit();
	void addPartSpecificActions();
	void setZoom(qreal, const QPoint& center = QPoint(-1, -1));

	void updateZoomSnapValues();
	void createStatusBarWidget();
};

} // namespace


#endif /* GVPART_H */
