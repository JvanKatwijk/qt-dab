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
//	This file is art of Qt-DAB and an adapted version of the
//	original
#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class UpdateChecker : public QObject {
Q_OBJECT

public:
			UpdateChecker	(const QString &);
			~UpdateChecker	();
	void		check		();
	QString 	version		() const;
	bool		checkPreRelease	() const;

signals:
	void 		done		(bool);

private:
	QString		theUrl;
	QNetworkAccessManager *theAccessManager;
	QString		theVersion;
	bool		isPreRelease;
	QString		bodyText;
	void		dataLoaded	(QNetworkReply *);
	bool		extractData	(const QByteArray &);
};

