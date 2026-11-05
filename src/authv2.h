#ifndef WEIBOMM_AUTHV2_H
#define WEIBOMM_AUTHV2_H
#include <QVariantMap>
class QDeclarativeEngine;
QVariantMap loginV2(QDeclarativeEngine *, const QVariantMap &, int *);
QVariantMap checkLoginSession(QDeclarativeEngine *);
int probeLoginV2();
#endif
