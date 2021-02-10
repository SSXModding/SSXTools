#pragma once
#include <eagle/Core.h>

#include <eagle/ShpsStructs.h>

#define INQT
#include <eagle/ShpsWriter.h>
#undef INQT

#include <QWidget>
#include <QImage>

namespace eagle {
	namespace ui {

		/**
		 * Widget for displaying shape images.
		 */
		struct ShapeWidget : public QWidget {
			Q_OBJECT
		   public:

			/**
			 * Display a shape image.
			 * 
			 * \param[in] shape The shape image to load and display.
			 */
			void PaintShape(core::shps::Image& shape);

		   protected:
			void paintEvent(QPaintEvent*);

		   private:
			QImage qtImage;
			//core::shps::Image* painting_shape = nullptr;
		};

	} // namespace ui

} // namespace eagle