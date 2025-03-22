// main.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QSplitter>
#include <QFileInfo>
#include <QPixmap>
#include <QBuffer>
#include <QScrollArea>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QDir>
#include <QStandardPaths>

class HTMLToPNGConverter : public QMainWindow {
    Q_OBJECT

public:
    HTMLToPNGConverter(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("HTML/CSS to PNG Converter");
        resize(1200, 800);
        setupUi();
    }

private slots:
    void openFile() {
        QString filePath = QFileDialog::getOpenFileName(this, 
            "Open HTML File", 
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
            "HTML Files (*.html *.htm)");
            
        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                htmlEditor->setPlainText(file.readAll());
                file.close();
                
                // Try to load associated CSS if exists
                QFileInfo htmlFileInfo(filePath);
                QString possibleCssPath = htmlFileInfo.absolutePath() + "/" + 
                                          htmlFileInfo.baseName() + ".css";
                QFile cssFile(possibleCssPath);
                if (cssFile.exists() && cssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    cssEditor->setPlainText(cssFile.readAll());
                    cssFile.close();
                }
                
                renderPreview();
            }
        }
    }
    
    void renderPreview() {
        QString html = htmlEditor->toPlainText();
        QString css = cssEditor->toPlainText();
        
        // Combine HTML and CSS
        if (!css.isEmpty()) {
            html.replace("</head>", "<style>" + css + "</style></head>");
            if (!html.contains("<head>")) {
                html = "<head><style>" + css + "</style></head>" + html;
            }
        }
        
        // Set content
        webView->setHtml(html);
        statusBar()->showMessage("Preview updated");
    }
    
    void saveToPNG() {
        if (htmlEditor->toPlainText().isEmpty()) {
            QMessageBox::warning(this, "Warning", "No HTML content to convert!");
            return;
        }
        
        QString filePath = QFileDialog::getSaveFileName(this, 
            "Save as PNG",
            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/converted.png",
            "PNG Files (*.png)");
            
        if (!filePath.isEmpty()) {
            QWebEnginePage* page = webView->page();
            
            // Get dimensions from settings
            int width = widthSpinBox->value();
            int height = heightSpinBox->value();
            
            page->setViewportSize(QSize(width, height));
            
            // Use a timer to wait for rendering to complete
            QTimer::singleShot(1000, this, [this, page, filePath, width, height]() {
                page->grab().then([filePath, width, height](const QPixmap& pixmap) {
                    QPixmap scaledPixmap = pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    scaledPixmap.save(filePath, "PNG");
                    QMessageBox::information(nullptr, "Success", "HTML content has been saved as PNG!");
                });
            });
        }
    }
    
    void saveHTML() {
        QString filePath = QFileDialog::getSaveFileName(this, 
            "Save HTML File",
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/index.html",
            "HTML Files (*.html)");
            
        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                stream << htmlEditor->toPlainText();
                file.close();
                
                // Save CSS if it's not empty
                QString cssContent = cssEditor->toPlainText();
                if (!cssContent.isEmpty()) {
                    QFileInfo htmlFileInfo(filePath);
                    QString cssPath = htmlFileInfo.absolutePath() + "/" + 
                                      htmlFileInfo.baseName() + ".css";
                    QFile cssFile(cssPath);
                    if (cssFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream cssStream(&cssFile);
                        cssStream << cssContent;
                        cssFile.close();
                        QMessageBox::information(this, "Success", "HTML and CSS files have been saved!");
                    }
                } else {
                    QMessageBox::information(this, "Success", "HTML file has been saved!");
                }
            }
        }
    }
    
    void setTheme(int index) {
        QString styleSheet;
        switch (index) {
            case 0: // Light
                styleSheet = "QMainWindow, QWidget { background-color: #f0f0f0; color: #202020; }"
                             "QTextEdit, QWebEngineView { background-color: #ffffff; border: 1px solid #c0c0c0; }"
                             "QPushButton { background-color: #e0e0e0; border: 1px solid #b0b0b0; padding: 5px 10px; }"
                             "QPushButton:hover { background-color: #d0d0d0; }";
                break;
            case 1: // Dark
                styleSheet = "QMainWindow, QWidget { background-color: #2d2d2d; color: #e0e0e0; }"
                             "QTextEdit { background-color: #3d3d3d; color: #e0e0e0; border: 1px solid #555555; }"
                             "QWebEngineView { background-color: #ffffff; border: 1px solid #555555; }"
                             "QPushButton { background-color: #555555; color: #e0e0e0; border: 1px solid #777777; padding: 5px 10px; }"
                             "QPushButton:hover { background-color: #666666; }"
                             "QComboBox, QSpinBox { background-color: #3d3d3d; color: #e0e0e0; border: 1px solid #555555; }"
                             "QLabel { color: #e0e0e0; }";
                break;
            case 2: // Blue
                styleSheet = "QMainWindow, QWidget { background-color: #1e3a5f; color: #ffffff; }"
                             "QTextEdit { background-color: #2c4c7c; color: #ffffff; border: 1px solid #3a5e8c; }"
                             "QWebEngineView { background-color: #ffffff; border: 1px solid #3a5e8c; }"
                             "QPushButton { background-color: #3a5e8c; color: #ffffff; border: 1px solid #4a7eb0; padding: 5px 10px; }"
                             "QPushButton:hover { background-color: #4a7eb0; }"
                             "QComboBox, QSpinBox { background-color: #2c4c7c; color: #ffffff; border: 1px solid #3a5e8c; }"
                             "QLabel { color: #ffffff; }";
                break;
        }
        qApp->setStyleSheet(styleSheet);
    }

private:
    void setupUi() {
        // Main widget
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        // Main layout
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        
        // Create splitter for resizable areas
        QSplitter *splitter = new QSplitter(Qt::Horizontal);
        
        // Left panel - Code editors
        QWidget *editorsWidget = new QWidget();
        QVBoxLayout *editorsLayout = new QVBoxLayout(editorsWidget);
        
        // HTML Editor
        QLabel *htmlLabel = new QLabel("HTML");
        htmlEditor = new QTextEdit();
        htmlEditor->setFont(QFont("Courier New", 10));
        htmlEditor->setLineWrapMode(QTextEdit::NoWrap);
        
        // CSS Editor
        QLabel *cssLabel = new QLabel("CSS");
        cssEditor = new QTextEdit();
        cssEditor->setFont(QFont("Courier New", 10));
        cssEditor->setLineWrapMode(QTextEdit::NoWrap);
        
        // Add editors to layout
        editorsLayout->addWidget(htmlLabel);
        editorsLayout->addWidget(htmlEditor);
        editorsLayout->addWidget(cssLabel);
        editorsLayout->addWidget(cssEditor);
        
        // Right panel - Preview
        QWidget *previewWidget = new QWidget();
        QVBoxLayout *previewLayout = new QVBoxLayout(previewWidget);
        
        QLabel *previewLabel = new QLabel("Preview");
        webView = new QWebEngineView();
        
        // Scrollable preview
        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setWidget(webView);
        scrollArea->setWidgetResizable(true);
        
        previewLayout->addWidget(previewLabel);
        previewLayout->addWidget(scrollArea);
        
        // Add the panels to the splitter
        splitter->addWidget(editorsWidget);
        splitter->addWidget(previewWidget);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 1);
        
        // Controls area
        QWidget *controlsWidget = new QWidget();
        QHBoxLayout *controlsLayout = new QHBoxLayout(controlsWidget);
        
        // Button group
        QWidget *buttonWidget = new QWidget();
        QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
        
        QPushButton *openButton = new QPushButton("Open HTML");
        QPushButton *renderButton = new QPushButton("Render Preview");
        QPushButton *saveHtmlButton = new QPushButton("Save HTML/CSS");
        QPushButton *saveButton = new QPushButton("Export to PNG"); 
        
        buttonLayout->addWidget(openButton);
        buttonLayout->addWidget(renderButton);
        buttonLayout->addWidget(saveHtmlButton);
        buttonLayout->addWidget(saveButton);
        
        // Settings group
        QGroupBox *settingsGroup = new QGroupBox("Settings");
        QFormLayout *settingsLayout = new QFormLayout(settingsGroup);
        
        QLabel *themeLabel = new QLabel("Theme:");
        QComboBox *themeCombo = new QComboBox();
        themeCombo->addItems({"Light", "Dark", "Blue"});
        
        QLabel *widthLabel = new QLabel("PNG Width:");
        widthSpinBox = new QSpinBox();
        widthSpinBox->setRange(100, 3840);
        widthSpinBox->setValue(1024);
        
        QLabel *heightLabel = new QLabel("PNG Height:");
        heightSpinBox = new QSpinBox();
        heightSpinBox->setRange(100, 2160);
        heightSpinBox->setValue(768);
        
        settingsLayout->addRow(themeLabel, themeCombo);
        settingsLayout->addRow(widthLabel, widthSpinBox);
        settingsLayout->addRow(heightLabel, heightSpinBox);
        
        controlsLayout->addWidget(buttonWidget);
        controlsLayout->addWidget(settingsGroup);
        
        // Add all components to main layout
        mainLayout->addWidget(splitter);
        mainLayout->addWidget(controlsWidget);
        
        // Status bar
        statusBar()->showMessage("Ready");
        
        // Connect signals and slots
        connect(openButton, &QPushButton::clicked, this, &HTMLToPNGConverter::openFile);
        connect(renderButton, &QPushButton::clicked, this, &HTMLToPNGConverter::renderPreview);
        connect(saveButton, &QPushButton::clicked, this, &HTMLToPNGConverter::saveToPNG);
        connect(saveHtmlButton, &QPushButton::clicked, this, &HTMLToPNGConverter::saveHTML);
        connect(themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HTMLToPNGConverter::setTheme);
        
        // Initial theme
        setTheme(0);
    }
    
    QTextEdit *htmlEditor;
    QTextEdit *cssEditor;
    QWebEngineView *webView;
    QSpinBox *widthSpinBox;
    QSpinBox *heightSpinBox;
};

// main.cpp continued
#include <main.moc>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Check if Qt WebEngine is available
    #ifndef QT_WEBENGINE_LIB
    QMessageBox::critical(nullptr, "Error", "This application requires Qt WebEngine!");
    return 1;
    #endif
    
    HTMLToPNGConverter converter;
    converter.show();
    
    return app.exec();
}
