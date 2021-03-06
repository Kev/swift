/*
 * Copyright (c) 2012-2014 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swift/QtUI/QtVCardWidget/QtVCardPhotoAndNameFields.h>

#include <Swift/QtUI/QtVCardWidget/ui_QtVCardPhotoAndNameFields.h>

#include <QMenu>

namespace Swift {

QtVCardPhotoAndNameFields::QtVCardPhotoAndNameFields(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::QtVCardPhotoAndNameFields) {
	ui->setupUi(this);
	ui->lineEditPREFIX->hide();
	ui->lineEditMIDDLE->hide();
	ui->lineEditSUFFIX->hide();
	ui->lineEditFN->hide();
	ui->lineEditNICKNAME->hide();
	ui->labelFULLNAME->hide();

#if QT_VERSION >= 0x040700
	ui->lineEditFN->setPlaceholderText(tr("Formatted Name"));
	ui->lineEditNICKNAME->setPlaceholderText(tr("Nickname"));
	ui->lineEditPREFIX->setPlaceholderText(tr("Prefix"));
	ui->lineEditGIVEN->setPlaceholderText(tr("Given Name"));
	ui->lineEditMIDDLE->setPlaceholderText(tr("Middle Name"));
	ui->lineEditFAMILY->setPlaceholderText(tr("Last Name"));
	ui->lineEditSUFFIX->setPlaceholderText(tr("Suffix"));
#endif

	setEditable(false);
}

QtVCardPhotoAndNameFields::~QtVCardPhotoAndNameFields() {
	delete ui;
}

bool QtVCardPhotoAndNameFields::isEditable() const {
	return editable;
}

void QtVCardPhotoAndNameFields::setEditable(bool editable) {
	this->editable = editable;

	ui->avatarWidget->setEditable(editable);
	ui->lineEditFN->setVisible(editable ? true : !ui->lineEditFN->text().isEmpty());
	ui->lineEditFN->setEditable(editable);
	ui->lineEditFN->setStyleSheet(editable ? "" : "QLineEdit {border: none; background-color: transparent;}");

	ui->lineEditNICKNAME->setVisible(editable ? true : !ui->lineEditNICKNAME->text().isEmpty());
	ui->lineEditNICKNAME->setEditable(editable);
	ui->lineEditNICKNAME->setStyleSheet(editable ? "" : "QLineEdit {border: none; background-color: transparent;}");

	// prefix given middle last suffix
	ui->lineEditPREFIX->setVisible(editable);
	ui->lineEditGIVEN->setVisible(editable);
	ui->lineEditMIDDLE->setVisible(editable);
	ui->lineEditFAMILY->setVisible(editable);
	ui->lineEditSUFFIX->setVisible(editable);
	ui->labelFULLNAME->setVisible(!editable);

	QStringList fullname;
	fullname << ui->lineEditPREFIX->text() << ui->lineEditGIVEN->text() << ui->lineEditMIDDLE->text();
	fullname << ui->lineEditFAMILY->text() << ui->lineEditSUFFIX->text();
	for (QStringList::iterator i = fullname.begin(); i != fullname.end(); i++) {
		*i = i->trimmed();
	}
	ui->labelFULLNAME->setText(fullname.join(" "));
}

void QtVCardPhotoAndNameFields::setAvatar(const ByteArray &data, const std::string &type) {
	ui->avatarWidget->setAvatar(data, type);
}

ByteArray QtVCardPhotoAndNameFields::getAvatarData() const {
	return ui->avatarWidget->getAvatarData();
}

std::string QtVCardPhotoAndNameFields::getAvatarType() const {
	return ui->avatarWidget->getAvatarType();
}

void QtVCardPhotoAndNameFields::setFormattedName(const QString& formattedName) {
	ui->lineEditFN->setText(formattedName);
}

QString QtVCardPhotoAndNameFields::getFormattedName() const {
	return ui->lineEditFN->text();
}

void QtVCardPhotoAndNameFields::setNickname(const QString& nickname) {
	ui->lineEditNICKNAME->setText(nickname);
}

QString QtVCardPhotoAndNameFields::getNickname() const {
	return ui->lineEditNICKNAME->text();
}

void QtVCardPhotoAndNameFields::setPrefix(const QString& prefix) {
	ui->lineEditPREFIX->setText(prefix);
}

QString QtVCardPhotoAndNameFields::getPrefix() const {
	return ui->lineEditPREFIX->text();
}

void QtVCardPhotoAndNameFields::setGivenName(const QString& givenName) {
	ui->lineEditGIVEN->setText(givenName);
}

QString QtVCardPhotoAndNameFields::getGivenName() const {
	return ui->lineEditGIVEN->text();
}

void QtVCardPhotoAndNameFields::setMiddleName(const QString& middleName) {
	ui->lineEditMIDDLE->setText(middleName);
}

QString QtVCardPhotoAndNameFields::getMiddleName() const {
	return ui->lineEditMIDDLE->text();
}

void QtVCardPhotoAndNameFields::setFamilyName(const QString& familyName) {
	ui->lineEditFAMILY->setText(familyName);
}

QString QtVCardPhotoAndNameFields::getFamilyName() const {
	return ui->lineEditFAMILY->text();
}

void QtVCardPhotoAndNameFields::setSuffix(const QString& suffix) {
	ui->lineEditSUFFIX->setText(suffix);
}

QString QtVCardPhotoAndNameFields::getSuffix() const {
	return ui->lineEditSUFFIX->text();
}

}
