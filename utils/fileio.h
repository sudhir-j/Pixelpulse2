/// simple implementation of writing files for qml

#ifndef FILEIO_H
#define FILEIO_H

#include <QObject>
#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QTextStream>
#include<QDataStream>
#include <QFileDialog>

class FileIO : public QObject
{
    Q_OBJECT

public slots:
    /// Show a "Save As" dialog and return the chosen path (empty if
    /// cancelled). Used instead of QML QtQuick.Dialogs FileDialog, whose
    /// fallback implementation does not render on macOS.
    QUrl getSaveFileName(const QString& title, const QString& nameFilter) {
        QString path = QFileDialog::getSaveFileName(nullptr, title, QString(),
                                                    nameFilter, nullptr,
                                                    fileDialogOptions());
        if (path.isEmpty())
            return QUrl();
        return QUrl::fromLocalFile(path);
    }

    /// Show an "Open" dialog and return the chosen path (empty if cancelled).
    QUrl getOpenFileName(const QString& title, const QString& nameFilter) {
        QString path = QFileDialog::getOpenFileName(nullptr, title, QString(),
                                                    nameFilter, nullptr,
                                                    fileDialogOptions());
        if (path.isEmpty())
            return QUrl();
        return QUrl::fromLocalFile(path);
    }

	/// accept a file handle by URI and source datastring
    bool writeByURI(const QUrl& destination, const QString& data) {
        auto path = destination.toLocalFile();
        return writeByFilename(path, data);
    }
	/// accept a file handle by string and source datastring
    bool writeByFilename(const QString& source, const QString& data)
    {
        if (source.isEmpty())
            return false;
        QString s = source;
        QFile file(s);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        // end with a newline
        out << data << "\n";
        return true;
    }
    /// accept a file handle by URI and source datastring
    bool writeRawByURI(const QUrl& destination, const QByteArray& data) {
        auto path = destination.toLocalFile();
        return writeRawByFilename(path, data);
    }
    /// accept a file handle by string and source datastring
    bool writeRawByFilename(const QString& source, const QByteArray& data)
    {
        if (source.isEmpty())
            return false;
        QString s = source;
        QFile file(s);
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);
        out.writeRawData(data, data.length());
        return true;
    }

    QString readByURI(const QUrl& source) {
		auto path = source.toLocalFile();
		QFile file(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
		QTextStream in(&file);
        return in.readAll();
	}

public:
    FileIO() {}

private:
    /// Options for the QFileDialog helpers above. On macOS the native (cocoa)
    /// panel fails to display in this build and returns an empty selection
    /// immediately, so force Qt's own widget-based dialog there. Other
    /// platforms keep their working native dialog.
    static QFileDialog::Options fileDialogOptions() {
#ifdef Q_OS_MAC
        return QFileDialog::DontUseNativeDialog;
#else
        return QFileDialog::Options();
#endif
    }
};

#endif // FILEIO_H
