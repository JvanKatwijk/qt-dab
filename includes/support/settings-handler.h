
#pragma once

#include	<QString>
#include	<QSettings>


void	store (QSettings *s, QString paragraph, QString key, QString &v);
void	store (QSettings *s, QString paragraph, QString key, int value);
int	value_i (QSettings *s, QString paragraph, QString key, int def);
float	value_f (QSettings *s, QString paragraph, QString key, float def);
QString	value_s (QSettings *s, QString paragraph, QString key, QString def);
void	remove	(QSettings *s, QString paragraoh, QString key);

