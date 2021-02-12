#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QStandardItemModel>
#include <QThread>

#include <eagle/Core.h>
#define INQT
#include <eagle/ShpsReader.h>
#include <eagle/ShpsConverter.h>
#undef INQT

#include "ui_MainWindow.h"
#include "ShapeWidget.h"

namespace eagle {
	namespace ui {

		using namespace eagle::core;

		class MainWindow : public QMainWindow {
			Q_OBJECT

		   public:
			MainWindow(QWidget* parent = nullptr);

			~MainWindow();

		   private slots:
			/**
			 * todo
			 */
			void OnOpenSSH();

		   //public slots:
			void OnSSHItemSelection(const QItemSelection& /*newSelection*/, const QItemSelection& /*oldSelection*/);

		   private:
			Ui::MainWindow ui;
			QStandardItemModel* item_model;

			// TODO: Refactor!!!!
			std::vector<shps::Image> images;
			core::ShpsConverter writer;

			ShapeWidget* shapeWidget;
		};

	} // namespace ui
} // namespace eagle