#ifndef LANDISGYRE230ENCODERDECODER_H
#define LANDISGYRE230ENCODERDECODER_H

#include "iec62056_21_helper.h"

#include "shared/meterplugin.h"
#include "shared/meterpluginhelper.h"

class LandisGyrE230EncoderDecoder : public IEC62056_21_Helper
{
    Q_OBJECT
public:
    explicit LandisGyrE230EncoderDecoder(QObject *parent = nullptr);


    static QString getSupportedMetersPrefixes();

    static QByteArray getDefPasswd() ;

    static bool isLogin2supportedMeter(const QVariantHash &hashRead);

    static bool isLogin2supportedMeter(const QByteArray &readArr);

    static QString versionFromMessage(const QVariantHash &hashRead);



    static bool decodeMeterSN(const QVariantHash &hashRead, const quint16 &goodStep, QVariantHash &hashTmpData, ErrsStrct &errWarn, int &error_counter);
//    static QString meterProgVersion(const QString &readArr);





    bool isAPolyPhaseMeter(const QVariantHash &hashTmpData);

    bool isAPolyPhaseMeter(const QString &vrsn);


    bool isItYourReadMessage(const QByteArray &arr);


    QStringList getSupportedEnrg(const quint8 &code, const QString &vrsn);


    int calculateEnrgIndx(qint16 currEnrg, const quint8 &pollCode, const QStringList &listEnrg, const QString &version);


    QVariantHash getServiceMessages(quint16 &step, QVariantHash &hashTmpData, const QByteArray &arrNI);

    QVariantHash getServiceMessagesExt(quint16 &step, QVariantHash &hashTmpData, const QByteArray &arrNI, const bool &dataReadingMode, const QVariantHash &hashConstData);


    void decodeServiceMessages(quint16 &step, QVariantHash &hashTmpData, const QVariantHash &hashRead, ErrsStrct &errWarn, int &error_counter);

    void decodeServiceMessagesExt(quint16 &step, QVariantHash &hashTmpData, const QVariantHash &hashRead, const bool &dataReadingMode, ErrsStrct &errWarn, int &error_counter);

    bool canProcessDtSNObisCodes(const QHash<QString, OneIECParsedAnswer> &hAnswers, const QVariantHash &hashConstData, QVariantHash &hashTmpData, ErrsStrct &errWarn, int &error_counter, int &warning_counter);

    QStringList getTotalEnergyObisCodes(const QVariantHash &hashTmpData, const QVariantHash &hashConstData, QStringList &enrgTariffL);


    void decodeTotalEnergyMessages(quint16 &step, QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QVariantHash &hashRead, ErrsStrct &errWarn, int &error_counter, int &warning_counter);


    QStringList getVoltageObisCodes(QStringList &enrgKeyL);


    void decodeVoltageMessages(quint16 &step, QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QVariantHash &hashRead, ErrsStrct &errWarn, int &error_counter, int &warning_counter);


    QVariantHash isItYourExt(const QByteArray &arr, QByteArray &lastDN, QTime &timeFromLastAuth);


    QVariantHash getReadDtMessage(QVariantHash &hashTmpData);


    bool decodeDtMessage(const QVariantHash &hashRead, QVariantHash &hashTmpData);


    void fixEnrgTariffs(QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QString &enrg, const bool &useRealValueOnly);

};

#endif // LANDISGYRE230ENCODERDECODER_H
