/*
Gwenview: an image viewer
Copyright 2021 Arjen Hiemstra <ahiemstra@heimr.nl>

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

#include "rasterimageitem.h"

#include <cmath>

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDebug>

#include "rasterimageview.h"
#include "gvdebug.h"
#include "lib/cms/cmsprofile.h"

using namespace Gwenview;

// Convenience constants for one third and one sixth.
static const qreal Third = 1.0 / 3.0;
static const qreal Sixth = 1.0 / 6.0;

RasterImageItem::RasterImageItem(Gwenview::RasterImageView* parent)
    : QGraphicsItem(parent)
    , mParentView(parent)
{
}

RasterImageItem::~RasterImageItem()
{
    if (mDisplayTransform) {
        cmsDeleteTransform(mDisplayTransform);
    }
}

void RasterImageItem::setRenderingIntent(RenderingIntent::Enum intent)
{
    mRenderingIntent = intent;
    update();
}

void Gwenview::RasterImageItem::updateCache()
{
    // Cache two scaled down versions of the image, one at a third of the size
    // and one at a sixth. These are used instead of the document image at small
    // zoom levels, to avoid having to copy around the entire image which can be
    // very slow for large images.
    auto document = mParentView->document();
    mThirdScaledImage = document->image().scaled(document->size() * Third, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    mSixthScaledImage = document->image().scaled(document->size() * Sixth, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

void RasterImageItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
    auto document = mParentView->document();

    if (document->image().isNull() || mThirdScaledImage.isNull() || mSixthScaledImage.isNull()) {
        return;
    }

    const auto dpr = mParentView->devicePixelRatio();
    const auto zoom = mParentView->zoom() / dpr;

    // This assumes we always have at least a single view of the graphics scene,
    // which should be true when painting a graphics item.
    const auto viewportRect = mParentView->scene()->views().first()->rect();

    // Map the viewport to the image so we get the area of the image that is
    // visible.
    auto imageRect = mParentView->mapToImage(viewportRect);

    // Grow the resulting rect by an arbitrary but small amount to avoid pixel
    // alignment issues. This results in the image being drawn slightly larger
    // than the viewport.
    imageRect = imageRect.marginsAdded(QMargins(5 * dpr, 5 * dpr, 5 * dpr, 5 * dpr));

    // Constrain the visible area rect by the image's rect so we don't try to
    // copy pixels that are outside the image.
    imageRect = imageRect.intersected(document->image().rect());

    QImage image;
    qreal targetZoom = zoom;

    // Copy the visible area from the document's image into a new image. This
    // allows us to modify the resulting image without affecting the original
    // image data. If we are zoomed out far enough, we instead use one of the
    // cached scaled copies to avoid having to copy a lot of data.
    if (zoom > Third) {
        image = document->image().copy(imageRect);
    } else if (zoom > Sixth) {
        auto sourceRect = QRect{imageRect.topLeft() * Third, imageRect.size() * Third};
        targetZoom = zoom / Third;
        image = mThirdScaledImage.copy(sourceRect);
    } else {
        auto sourceRect = QRect{imageRect.topLeft() * Sixth, imageRect.size() * Sixth};
        targetZoom = zoom / Sixth;
        image = mSixthScaledImage.copy(sourceRect);
    }

    // We want nearest neighbour when zooming in since that provides the most
    // accurate representation of pixels, but when zooming out it will actually
    // not look very nice, so use smoothing when zooming out.
    const auto transformationMode = zoom < 1.0 ? Qt::SmoothTransformation : Qt::FastTransformation;

    // Scale the visible image to the requested zoom.
    image = image.scaled(image.size() * targetZoom, Qt::IgnoreAspectRatio, transformationMode);

    // Perform color correction on the visible image.
    applyDisplayTransform(image);

    const auto destinationRect = QRect{
        // Ceil the top left corner to avoid pixel alignment issues on higher DPI
        QPoint{int(std::ceil(imageRect.left() * zoom)), int(std::ceil(imageRect.top() * zoom))},
        image.size()
    };

    painter->drawImage(destinationRect, image);
}

QRectF RasterImageItem::boundingRect() const
{
    return QRectF{QPointF{0, 0}, mParentView->documentSize() * mParentView->zoom()};
}

void RasterImageItem::applyDisplayTransform(QImage& image)
{
    if (mApplyDisplayTransform) {
        updateDisplayTransform(image.format());
        if (mDisplayTransform) {
            quint8 *bytes = image.bits();
            cmsDoTransform(mDisplayTransform, bytes, bytes, image.width() * image.height());
        }
    }
}

void RasterImageItem::updateDisplayTransform(QImage::Format format)
{
    if (format == QImage::Format_Invalid) {
        return;
    }

    mApplyDisplayTransform = false;
    if (mDisplayTransform) {
        cmsDeleteTransform(mDisplayTransform);
    }
    mDisplayTransform = nullptr;

    Cms::Profile::Ptr profile = mParentView->document()->cmsProfile();
    if (!profile) {
        // The assumption that something unmarked is *probably* sRGB is better than failing to apply any transform when one
        // has a wide-gamut screen.
        profile = Cms::Profile::getSRgbProfile();
    }
    Cms::Profile::Ptr monitorProfile = Cms::Profile::getMonitorProfile();
    if (!monitorProfile) {
        qCWarning(GWENVIEW_LIB_LOG) << "Could not get monitor color profile";
        return;
    }

    cmsUInt32Number cmsFormat = 0;
    switch (format) {
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
        cmsFormat = TYPE_BGRA_8;
        break;
    case QImage::Format_Grayscale8:
        cmsFormat = TYPE_GRAY_8;
        break;
    default:
        qCWarning(GWENVIEW_LIB_LOG) << "Gwenview can only apply color profile on RGB32 or ARGB32 images";
        return;
    }

    mDisplayTransform = cmsCreateTransform(profile->handle(), cmsFormat,
                                           monitorProfile->handle(), cmsFormat,
                                           mRenderingIntent, cmsFLAGS_BLACKPOINTCOMPENSATION);
    mApplyDisplayTransform = true;
}
