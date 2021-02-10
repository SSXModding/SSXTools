#include "MainWindow.h"

#include <QToolTip>
#include <QScrollBar>
#include <QTextStream>
#include <QFileDialog>


#include <QItemSelectionModel>

namespace eagle {
	namespace ui {

		MainWindow::MainWindow(QWidget* parent)
			: QMainWindow(parent),
			  item_model(new QStandardItemModel(this)) {
			ui.setupUi(this);
			setFixedSize(width(), height());

			// connect all of the UI events to functions in here
#if 0
			connect(ui.inputBrowse, SIGNAL(clicked()), this, SLOT(InputBrowseButtonClicked()));
			connect(ui.outputBrowse, SIGNAL(clicked()), this, SLOT(OutputBrowseButtonClicked()));
			connect(ui.extractButton, SIGNAL(clicked()), this, SLOT(ExtractButtonClicked()));
			connect(ui.saveLog, SIGNAL(clicked()), this, SLOT(SaveLogButtonClicked()));
			connect(ui.clearLog, SIGNAL(clicked()), this, SLOT(ClearLogButtonClicked()));

			connect(ui.aboutQtButton, SIGNAL(clicked()), this, SLOT(AboutButtonClicked()));

			// connect textchanged events to our handler for both of them
			connect(ui.inputFiles, SIGNAL(textChanged(const QString&)), this, SLOT(TextChanged()));
			connect(ui.outputDir, SIGNAL(textChanged(const QString&)), this, SLOT(TextChanged()));
#endif
			//connect(ui.);
			connect(ui.openSSH, SIGNAL(triggered(bool)), this, SLOT(OnOpenSSH()));

			ui.treeView->setModel(item_model);

			QItemSelectionModel* selectionModel = ui.treeView->selectionModel();
			connect(selectionModel, &QItemSelectionModel::selectionChanged,
					this, &MainWindow::OnSSHItemSelection);
		}

		MainWindow::~MainWindow() {

			if(shapeWidget) {
				delete shapeWidget;
			}
		}

		void MainWindow::OnOpenSSH() {
			QFileDialog fileSelector(this, "Select input filename", "", "Playstation2 Shape Files (*.SSH)");
			fileSelector.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
			fileSelector.setFileMode(QFileDialog::FileMode::ExistingFile);

			if(fileSelector.exec()) {
				if(fileSelector.selectedFiles().isEmpty())
					return;

				// Disable the open-file nag
				ui.openfilenag->setEnabled(false);

				std::string file = fileSelector.selectedFiles()[0].toStdString();

				// TODO: this should run in another thread..

				std::ifstream stream(file, std::ifstream::in | std::ifstream::binary);
				core::ShpsReader reader(stream, file);

				if(!reader.ReadHeader()) {
					// TODO: display feedback to the user....
					return;
				}

				QStandardItem* root = item_model->invisibleRootItem();
				root->clearData();

				QStandardItem* rootshape = new QStandardItem(fileSelector.selectedFiles()[0]);

				root->appendRow(rootshape);

				auto header = reader.GetHeader();
				reader.ReadTOC();

				for(uint32 i = 0; i < header.FileTextureCount; ++i) {
					auto image = reader.ReadImage(i);

					if(!image.data.empty()) {
						images.push_back(image);
						rootshape->appendRow(new QStandardItem(tr("Shape %1: %2").arg(QString::number(image.index), QString::fromLocal8Bit(image.toc_entry.Name, 4))));
					}
				}

				// Create the shape widget if it has not been created before.
				if(!shapeWidget) {
					shapeWidget = new ShapeWidget();
					shapeWidget->setParent(ui.centralwidget);
				}
			}
		}

		void MainWindow::OnSSHItemSelection(const QItemSelection& newSelection, const QItemSelection& oldSelection) {
			const QModelIndex index = ui.treeView->selectionModel()->currentIndex();
			//setWindowTitle(tr("FUCK %1 ").arg(QString::number(index.row())));

			// tell it to paint the shape we selected
			shapeWidget->PaintShape(images[index.row() % images.size()]);
		}

	} // namespace ui
} // namespace eagle