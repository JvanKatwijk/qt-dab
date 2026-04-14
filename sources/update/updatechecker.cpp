#
/*
 * This file is part of the AbracaDABra project
 *
 * MIT License
 *
 * Copyright (c) 2019-2025 Petr Kopecký <xkejpi (at) gmail (dot) com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

//
//	This file is part of Qt-DAB and an adapted version of
//	the original file

#include "updatechecker.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>

		UpdateChecker::UpdateChecker (const QString &theUrl) {
	this	-> theUrl	= theUrl;
	theAccessManager	= nullptr;
	isPreRelease		= false;
}
	
		UpdateChecker::~UpdateChecker () {
	if (theAccessManager != nullptr) 
	   theAccessManager -> deleteLater ();
}

void	UpdateChecker::check () {
	if (theAccessManager == nullptr) {
	   theAccessManager = new QNetworkAccessManager ();
	   connect (theAccessManager, &QNetworkAccessManager::finished,
	            this, &UpdateChecker::dataLoaded);
	   connect (theAccessManager, &QNetworkAccessManager::destroyed,
	            this, [this] () { theAccessManager = nullptr; });
	}

	QNetworkRequest request;
	request. setUrl (QUrl(theUrl));

	QSslConfiguration sslConfiguration =
	                 QSslConfiguration::defaultConfiguration ();
	sslConfiguration.
	       setCaCertificates (QSslConfiguration::systemCaCertificates ());
	sslConfiguration. setProtocol (QSsl::AnyProtocol);
	request. setSslConfiguration (sslConfiguration);
	request. setTransferTimeout (10 * 1000);
	theAccessManager -> get (request);
}

void	UpdateChecker::dataLoaded (QNetworkReply *reply) {
bool result = false;

	if (reply -> error () == QNetworkReply::NoError) {
	   QByteArray data = reply -> readAll ();
	   if (!data.isEmpty()) {
	      result = extractData (data);
	   }
	}

	reply -> deleteLater ();
	theAccessManager -> deleteLater ();
	emit done (result);
}

bool	UpdateChecker::extractData (const QByteArray &data) {
	QJsonDocument jsonDoc = QJsonDocument::fromJson (data);
	if (jsonDoc. isNull ()) {
	   return false;
	}

	if (jsonDoc. isObject ()) {
	   auto value = jsonDoc.object (). value ("tag_name");
	   if (!value. isUndefined ()) 
	      theVersion = value. toString ("");

	   value = jsonDoc. object (). value ("prerelease");
	   if (!value. isUndefined ())
	      isPreRelease = value. toBool (false);
//
//	for future modifications
	   value = jsonDoc. object (). value ("body");
	   if (!value. isUndefined ())
	      bodyText	= value. toString ("");
	   
	   return true;
	}

	return false;
}

QString UpdateChecker::version () const {
	return theVersion;
}

bool	UpdateChecker::checkPreRelease () const {
	return isPreRelease;
}

