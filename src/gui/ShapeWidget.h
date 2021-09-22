#pragma once
#include <ssxtools/Core.h>

#include <ssxtools/shps/ShpsStructs.h>

#include <ssxtools/shps/ShpsConverter.h>

#include <QWidget>
#include <QImage>
#include <QLayout>

namespace ssxtools::ui {

	/**
	 * Widget for displaying a shape's image onto some preview area.
	 */
	struct ShapeWidget : public QWidget {
		Q_OBJECT
	   public:

		ShapeWidget(QWidget* parent, QLayout* layout)
		 : QWidget(parent) {
			this->layout = layout;
			this->layout->addWidget(this);
		}

		/**
		 * Display a shape image.
		 *
		 * \param[in] shape The shape image to load and display.
		 */
		void PaintShape(shps::Image& shape, bool ssxHack);

		//inline QSize sizeHint() const override {
		//	std::printf("fuck you: %d x %d\n", qtImage.width(), qtImage.height());
		//	return QSize{qtImage.width() + 16, qtImage.height() + 16};
		//}

		//inline QSize minimumSizeHint() const override {
		//	return QSize{qtImage.width() , qtImage.height()};
		//}

	   protected:
		void paintEvent(QPaintEvent*) override;

	   private:
		QImage qtImage;
		QLayout* layout;
		/**
		 * The backing raw buffer for the qt image.
		 */
		std::vector<std::uint8_t> rawBuffer;
		//shps::shps::Image* painting_shape = nullptr;
	};

} // namespace ssxtools::ui