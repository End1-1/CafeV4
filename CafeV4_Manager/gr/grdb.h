#ifndef GRDB_H
#define GRDB_H

#include <QString>
#include <QLocale>
#include <QRegExp>

#define double_str(value, f) QLocale().toString(value, 'f', f).remove(QRegExp("(?!\\d[\\.\\,][1-9]+)0+$")).remove(QRegExp("[\\.\\,]$"))
#define FORMAT_DATETIME_TO_STR "dd/MM/yyyy HH:mm:ss"
#define FORMAT_DATETIME_TO_STR2 "dd/MM/yyyy HH:mm"
#define FORMAT_DATE_TO_STR "dd/MM/yyyy"
#define FORMAT_TIME_TO_STR "HH:mm:ss"
#define FORMAT_DATE_TO_STR_MYSQL "yyyy-MM-dd"

extern QString _gr_host_;
extern QString _gr_path_;
extern QString _gr_user_;
extern QString _gr_pass_;

#endif // GRDB_H
