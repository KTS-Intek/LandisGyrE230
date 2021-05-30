#ifndef LANDISGYRE230_H
#define LANDISGYRE230_H

#include <QObject>
#include <QtPlugin>


#include "landisgyre230encoderdecoder.h"


class LandisGyrE230 : public QObject, MeterPlugin
{
    Q_OBJECT
public:
    Q_PLUGIN_METADATA(IID "ua.zbyralko.hello_zb.MeterPlugin" FILE "zbyralko.json")
    Q_INTERFACES(MeterPlugin)

public:
    QString getMeterType() ;

    quint16 getPluginVersion() ;

    QString getMeterAddrsAndPsswdRegExp();

    quint8 getPasswdType() ;

    QString getVersion() ;

    QByteArray getDefPasswd() ;

    QString getSupportedMeterList() ;

    quint8 getMaxTariffNumber(const QString &version) ;

    QStringList getEnrgList4thisMeter(const quint8 &pollCode, const QString &version) ;

    quint8 getMeterTypeTag();

    Mess2meterRezult mess2meter(const Mess2meterArgs &pairAboutMeter);

    QVariantHash decodeMeterData(const DecodeMeterMess &threeHash);

    QVariantHash helloMeter(const QVariantHash &hashMeterConstData);

    QString meterTypeFromMessage(const QByteArray &readArr);

    QVariantHash isItYour(const QByteArray &arr);

    QVariantHash isItYourRead(const QByteArray &arr);

    QByteArray niChanged(const QByteArray &arr);

    QVariantHash meterSn2NI(const QString &meterSn);



    //parametryzatsiya
    Mess2meterRezult messParamPamPam(const Mess2meterArgs &pairAboutMeter);
    QVariantHash decodeParamPamPam(const DecodeMeterMess &threeHash) ;

    QVariantHash how2logout(const QVariantHash &hashConstData, const QVariantHash &hashTmpData) ;

    QVariantHash getDoNotSleep(const quint8 &minutes);

    QVariantHash getGoToSleep(const quint8 &seconds) ;

    QVariantList getDefaultVirtualMetersSchema()  ;//{ return QVariantList(); }// hashTmpData.insert("virtualmetersonly", true);


    bool isPollCodeSupported(const quint16 &pollCode);

private:
    Mess2meterRezult message2Meter(const Mess2meterArgs &pairAboutMeter, const QString &defVersion) ;
    QVariantHash decodeMeterData(const DecodeMeterMess &threeHash, const QString &defVersion);


    LandisGyrE230EncoderDecoder gamadecoder;


    QByteArray lastDN;
    QByteArray lastAddr;
    QTime timeFromLastAuth;

    ErrsStrct lastErr;

    QByteArray lastAutoDetectNi;
    QDateTime lastAutoDetectDt;

};

#endif // LANDISGYRE230_H
