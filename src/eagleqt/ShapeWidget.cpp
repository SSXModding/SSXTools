#include "ShapeWidget.h"
#include <QPainter>

namespace eagle {
	namespace ui {

		void ShapeWidget::PaintShape(core::shps::Image& shape) {
			core::ShpsWriter writer;
			std::vector<mco::byte> rawBuffer;

			if(!writer.BuildImageBuffer(rawBuffer, shape))
				return;

			core::shps::Bgra8888* ptr = (core::shps::Bgra8888*)rawBuffer.data();
			qtImage = QImage(shape.width, shape.height, QImage::Format_RGBA8888);

			for(int x = 0; x < shape.width; ++x) {
				for(int y = 0; y < shape.height; ++y) {
					auto shapecolor = ptr[y * shape.width + x];
					qtImage.setPixel(x, y, qRgba(shapecolor.r, shapecolor.g, shapecolor.g, shapecolor.a));
				}
			}
		}

		void ShapeWidget::paintEvent(QPaintEvent* ev) {
			QWidget::paintEvent(ev);

			QPainter p(this);
			p.drawPixmap(0, 0, QPixmap::fromImage(qtImage));
		}

	} // namespace ui
} // namespace eagle