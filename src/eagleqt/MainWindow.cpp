#include "MainWindow.h"

#include <QToolTip>
#include <QScrollBar>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>

// TODO: Switch to widget based model
// (so we can have icons & stuff. A lot better as well)
#include <QItemSelectionModel>

namespace eagle::ui {

	MainWindow::MainWindow(QWidget* parent)
		: QMainWindow(parent),
		  item_model(new QStandardItemModel(this)) {
		ui.setupUi(this);

		// connect all of the UI events to functions in here
		connect(ui.openSSH, &QAction::triggered, this, &MainWindow::OnOpenSSH);

		connect(ui.aboutEagle, &QAction::triggered, [&]() {
			// TODO: Needs to be stateful in MainWindow.
			QMessageBox::information(this, "About EAGLe", "EAGLE Qt 5 Frontend. TODO: Stateful window widget.");
		});

		connect(ui.aboutQt, &QAction::triggered, []() {
			// aboutQt needs to be provided in this case
			QApplication::aboutQt();
		});

		ui.treeView->setModel(item_model);

		QItemSelectionModel* selectionModel = ui.treeView->selectionModel();
		connect(selectionModel, &QItemSelectionModel::selectionChanged,
				this, &MainWindow::OnSSHItemSelection);
	}

	MainWindow::~MainWindow() = default;

	void MainWindow::EnableNag() {
		if(!nag_enabled) {
			nag_enabled = true;
			if(ui.sshLayout->rowCount() == 1)
				ui.sshLayout->removeRow(0);
			ui.sshLayout->addWidget(ui.openfilenag);
		}
	}

	void MainWindow::DisableNag() {
		if(nag_enabled) {
			nag_enabled = false;
			ui.sshLayout->removeRow(0);
		}
	}

	void MainWindow::OnOpenSSH() {
		QFileDialog fileSelector(this, "Select input filename", "", "Playstation2 Shape Files (*.SSH)");
		fileSelector.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
		fileSelector.setFileMode(QFileDialog::FileMode::ExistingFile);

		if(fileSelector.exec()) {
			if(fileSelector.selectedFiles().isEmpty())
				return;

			std::string file = fileSelector.selectedFiles()[0].toStdString();

			// Disable the nag text since the file selector is running.
			DisableNag();

			std::ifstream stream(file, std::ifstream::in | std::ifstream::binary);
			core::ShpsReader reader(stream, file);

			if(!stream) {
				QMessageBox::critical(this, "EAGLe Error", "There was an error opening the given .SSH file for reading.");
				return;
			}

			if(!reader.ReadHeader()) {
				QMessageBox::critical(this, "EAGLe Error", "There was an error reading the given .SSH file.");
				return;
			}

			QStandardItem* root = item_model->invisibleRootItem();

			// Clear any existing data from the item model.
			if(item_model->hasChildren()) {
				item_model->removeRows(0, item_model->rowCount());
				images.clear();
			}

			auto* rootshape = new QStandardItem(fileSelector.selectedFiles()[0]);
			root->appendRow(rootshape);

			auto& header = reader.GetHeader();
			reader.ReadTOC();

			for(int i = 0; i < header.FileTextureCount; ++i) {
				auto image = reader.ReadImage(i);

				if(!image.data.empty()) {
					images.push_back(image);
					rootshape->appendRow(new QStandardItem(tr("Shape %1: %2").arg(QString::number(image.index), QString::fromLocal8Bit(image.toc_entry.Name, 4))));
				}
			}

			// Create the shape widget if it has not been created before.
			if(!shape_widget_)
				shape_widget_ = new ShapeWidget(ui.centralwidget, ui.sshLayout);
		}
	}

	void MainWindow::OnSSHItemSelection(const QItemSelection& newSelection, const QItemSelection& oldSelection) {
		const QModelIndex index = ui.treeView->selectionModel()->currentIndex();
		auto& shape = images[index.row() % images.size()];

		// Show a message on the status bar of the image type and WxH
		// TODO: This is Rationale for using widget based list view...
		QString message;
		QTextStream(&message) << eagle::core::EnumToString<eagle::core::shps::ShapeImageType>(shape.format) << " " << shape.width << 'x' << shape.height;
		statusBar()->showMessage(message);

		if(shape_widget_)
			shape_widget_->PaintShape(shape);
	}

} // namespace eagle::ui
