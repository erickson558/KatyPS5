#include "mainDialog.h"

#include <QApplication>
#include <QArgument>
#include <QLocale>
#include <QObject>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>

class QStyle;

namespace {

// Kept deliberately simple: one optional translation (Spanish) embedded as a Qt resource by
// qt_add_translations() in CMakeLists.txt, selected via the "Launcher language" combo in the
// global settings dialog (ConfigurationEditDialog). Unset/"English" or an unrecognized value
// both mean "use the source strings as-is" - no translator needs to be installed for that case.
QString SelectedLanguage() {
	QSettings settings(QStringLiteral("Katy.ini"), QSettings::IniFormat);
	settings.beginGroup(QStringLiteral("Launcher"));
	return settings.value(QStringLiteral("language"), QStringLiteral("auto")).toString();
}

// Installing the translator has to happen before any widget is constructed: tr() lookups
// (including the ones uic generates for .ui <string> properties) resolve at the point the
// string is first requested, which for most of this app's dialogs is inside their constructor.
void InstallTranslator(QApplication& app) {
	static QTranslator translator;

	auto language = SelectedLanguage();
	if (language == QStringLiteral("auto")) {
		language = QLocale::system().language() == QLocale::Spanish ? QStringLiteral("es")
		                                                             : QStringLiteral("en");
	}
	if (language != QStringLiteral("es")) {
		return; // English is the source language - nothing to load.
	}
	if (translator.load(QStringLiteral(":/translations/katyps5_es.qm"))) {
		app.installTranslator(&translator);
	}
}

} // namespace

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	InstallTranslator(a);
	MainDialog w;

	QStyle* s = QStyleFactory::create("fusion");
	QApplication::setStyle(s);

	QObject::connect(&a, &QApplication::aboutToQuit, &w, &MainDialog::Quit);

	w.emit Start();

	w.show();

	return QApplication::exec();
}
