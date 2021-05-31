#include "landisgyre230.h"

#include <QtCore>
#include <QTime>

///[!] type-converter
#include "src/base/convertatype.h"

///[!] meter-plugin-shared
#include "shared/meterpluginhelper.h"

#include "moji_defy.h"
#include "definedpollcodes.h"
#include "matildalimits.h"

#include "myucdevicetypes.h"
#include "definedpollcodes.h"
#include "ucmetereventcodes.h"


#define PLG_4_PC         1 //Parametrization



//-----------------------------------------------------------------------------------------

QString LandisGyrE230::getMeterType()
{
    return QString("LandisGyrIEC");
}

//-----------------------------------------------------------------------------------------

quint16 LandisGyrE230::getPluginVersion()
{
    return PLG_VER_RELEASE;
}

//-----------------------------------------------------------------------------------------

QString LandisGyrE230::getMeterAddrsAndPsswdRegExp()
{
    return QString("%1%2").arg("^([0-9]{8})$").arg("^([0-9]{12})$");
}

//-----------------------------------------------------------------------------------------

quint8 LandisGyrE230::getPasswdType()
{
    return UCM_PSWRD_TEXT;
}

//-----------------------------------------------------------------------------------------

QString LandisGyrE230::getVersion()
{
    return QString("LandisGyrIEC v0.0.1 %1").arg(QString(BUILDDATE));
}

//-----------------------------------------------------------------------------------------

QByteArray LandisGyrE230::getDefPasswd()
{
    return  LandisGyrE230EncoderDecoder::getDefPasswd();
}

//-----------------------------------------------------------------------------------------

QString LandisGyrE230::getSupportedMeterList()
{
    return QString("E230");
}

//-----------------------------------------------------------------------------------------

quint8 LandisGyrE230::getMaxTariffNumber(const QString &version)
{
    Q_UNUSED(version);  return (quint8)4 ;
}

//-----------------------------------------------------------------------------------------

QStringList LandisGyrE230::getEnrgList4thisMeter(const quint8 &pollCode, const QString &version)
{
    return gamadecoder.getSupportedEnrg(pollCode, version);

}

//-----------------------------------------------------------------------------------------

quint8 LandisGyrE230::getMeterTypeTag()
{
    return  UC_METER_ELECTRICITY;
}

//-----------------------------------------------------------------------------------------

Mess2meterRezult LandisGyrE230::mess2meter(const Mess2meterArgs &pairAboutMeter)
{
    const QVariantHash hashConstData = pairAboutMeter.hashConstData;
    QVariantHash hashTmpData = pairAboutMeter.hashTmpData;

    gamadecoder.verbouseMode = true;// hashConstData.value("verboseMode").toBool();


    if(!isPollCodeSupported(hashConstData.value("pollCode").toUInt())){
        hashTmpData.insert("notsup", true);
        hashTmpData.insert("notsupasdone", true);
        hashTmpData.insert("step", 0xFFFF);
        return Mess2meterRezult(QVariantHash(),hashTmpData);
    }


    return message2Meter(pairAboutMeter, hashTmpData.value("vrsn").toString());
}

//-----------------------------------------------------------------------------------------

QVariantHash LandisGyrE230::decodeMeterData(const DecodeMeterMess &threeHash)
{
    const QVariantHash hashTmpData = threeHash.hashTmpData;

    if(gamadecoder.verbouseMode){
        const QVariantHash hashRead = threeHash.hashRead;
        foreach (QString key, hashRead.keys()) {
            qDebug() << "CE303 read "  << key << hashRead.value(key).toByteArray().toHex().toUpper() << hashRead.value(key).toByteArray().simplified().trimmed();
        }
    }
    return decodeMeterData(threeHash, hashTmpData.value("vrsn").toString());
}

//-----------------------------------------------------------------------------------------

QVariantHash LandisGyrE230::helloMeter(const QVariantHash &hashMeterConstData)
{
    return gamadecoder.getHelloMeter(hashMeterConstData);
}

//-----------------------------------------------------------------------------------------

QString LandisGyrE230::meterTypeFromMessage(const QByteArray &readArr)
{
    QString retVal("");
    if(gamadecoder.isLogin2supportedMeter(readArr)){// readArr.mid(5,4) == "CE30" || readArr.mid(5,4) == "CE20"){

        const QByteArray version = readArr.simplified().trimmed().mid(5);

        //         if(ok && version > 6 && version < 88){
        retVal = getMeterType();
        if(gamadecoder.verbouseMode) qDebug() << retVal << "version" << version;
        //         }
    }
    return retVal;
}

//-----------------------------------------------------------------------------------------

QVariantHash LandisGyrE230::isItYour(const QByteArray &arr)
{
    return gamadecoder.isItYourExt(arr, lastDN, timeFromLastAuth);

}

//-----------------------------------------------------------------------------------------

QVariantHash LandisGyrE230::isItYourRead(const QByteArray &arr)
{
    if(gamadecoder.verbouseMode) qDebug() << 2414 << "CE303::isItYourRead";
    QVariantHash hash;
    hash.insert("7DtEpt",true);
    if(gamadecoder.isItYourReadMessage(arr)){
        hash.insert("Tak", true);
    }
    return hash;
}

//-----------------------------------------------------------------------------------------

QByteArray LandisGyrE230::niChanged(const QByteArray &arr)
{
    Q_UNUSED(arr);

    return "";
}

//-----------------------------------------------------------------------------------------

QVariantHash LandisGyrE230::meterSn2NI(const QString &meterSn)
{
    QVariantHash h;
    h.insert("hard", meterSn);
    return h;
}

//-----------------------------------------------------------------------------------------

Mess2meterRezult LandisGyrE230::messParamPamPam(const Mess2meterArgs &pairAboutMeter)
{
    QVariantHash hashTmpData = pairAboutMeter.hashTmpData;

    hashTmpData.insert("notsup", true);
    hashTmpData.insert("notsupasdone", true);
    hashTmpData.insert("step", 0xFFFF);
    return Mess2meterRezult(QVariantHash(),hashTmpData);
}

//-----------------------------------------------------------------------------------------

QVariantHash LandisGyrE230::decodeParamPamPam(const DecodeMeterMess &threeHash)
{
    return threeHash.hashTmpData;

}

//-----------------------------------------------------------------------------------------

QVariantHash LandisGyrE230::how2logout(const QVariantHash &hashConstData, const QVariantHash &hashTmpData)
{
    Q_UNUSED(hashConstData);
    Q_UNUSED(hashTmpData);
    return gamadecoder.getGoodByeMessageSmpl();
}

//-----------------------------------------------------------------------------------------

QVariantHash LandisGyrE230::getDoNotSleep(const quint8 &minutes)
{

    Q_UNUSED(minutes);
    return QVariantHash();
}

//-----------------------------------------------------------------------------------------

QVariantHash LandisGyrE230::getGoToSleep(const quint8 &seconds)
{
    Q_UNUSED(seconds);
    return QVariantHash();
}

//-----------------------------------------------------------------------------------------

QVariantList LandisGyrE230::getDefaultVirtualMetersSchema()
{
    return QVariantList();

}

//-----------------------------------------------------------------------------------------

bool LandisGyrE230::isPollCodeSupported(const quint16 &pollCode)
{
    return (pollCode == POLL_CODE_READ_VOLTAGE || pollCode == POLL_CODE_READ_TOTAL);

}

//-----------------------------------------------------------------------------------------

Mess2meterRezult LandisGyrE230::message2Meter(const Mess2meterArgs &pairAboutMeter, const QString &defVersion)
{
    const QVariantHash hashConstData = pairAboutMeter.hashConstData;
    QVariantHash hashTmpData = pairAboutMeter.hashTmpData;
    QVariantHash hashMessage;

    const QByteArray arrNI = gamadecoder.message2meterChecks(hashTmpData, lastAutoDetectNi, hashConstData, defVersion, lastAutoDetectDt);

    quint8 pollCode = hashConstData.value("pollCode").toUInt();

    quint16 step = hashTmpData.value("step", 0).toUInt();


    if(step == 11){
        hashMessage = gamadecoder.getGoodByeMessage(hashTmpData);
        return Mess2meterRezult(hashMessage, hashTmpData);
    }


    if(pollCode >= POLL_CODE_READ_METER_LOGBOOK && pollCode <= POLL_CODE_READ_END_MONTH ){

        hashMessage = gamadecoder.getServiceMessages(step, hashTmpData, arrNI);


        if(gamadecoder.verbouseMode) qDebug() << "CE303 authorize module" << step << pollCode;

        if(!hashMessage.isEmpty()){
            if(!hashMessage.contains("endSymb"))
                hashMessage.insert("endSymb", "");
            hashTmpData.insert("step", step);
            hashMessage.insert("data7EPt", true);

            return Mess2meterRezult(hashMessage, hashTmpData);
        }

    }

//    switch(pollCode){
//    case POLL_CODE_READ_VOLTAGE : hashMessage = gamadecoder.getVoltageMessages(hashTmpData, hashConstData)          ; break;
//    case POLL_CODE_READ_TOTAL   : hashMessage = gamadecoder.getTotalEnergyMessages(step, hashTmpData, hashConstData); break;
//    }


    if(hashTmpData.value("step", 0).toInt() == 0xFFFF){
        hashMessage.clear();
    }else{
        hashMessage.insert("data7EPt", true);

        if(gamadecoder.verbouseMode)
            qDebug() << "LandisGyrE230 write " << hashMessage.value("message_0").toByteArray().simplified().trimmed();
    }

    return Mess2meterRezult(hashMessage, hashTmpData);
}

//-----------------------------------------------------------------------------------------

QVariantHash LandisGyrE230::decodeMeterData(const DecodeMeterMess &threeHash, const QString &defVersion)
{
    const QVariantHash hashConstData = threeHash.hashConstData;
    const QVariantHash hashRead = threeHash.hashRead;
    QVariantHash hashTmpData = threeHash.hashTmpData;


    int error_counter = qMax(0, hashTmpData.value("error_counter", 0).toInt());
    int warning_counter = qMax(0, hashTmpData.value("warning_counter", 0).toInt());


    hashTmpData.insert("messFail", true);
    quint8 pollCode = hashConstData.value("pollCode").toUInt();
    bool skipMeterSN = !hashTmpData.value("SN").toString().isEmpty();
    quint16 step = hashTmpData.value("step", (quint16)0).toUInt();

    if(gamadecoder.verbouseMode)
        qDebug() << "LandisGyrE230 read " << hashRead.value("readArr_0").toByteArray().simplified().trimmed();


    if(step == 11){ //ignore the answer
         hashTmpData.insert("messFail", false);
         hashTmpData.insert("step", 0xFFFF);
         return hashTmpData;
    }


    //    const QByteArray arrNI = GamaG1MG3MEncoderDecoder::message2meterChecks(hashTmpData, lastAutoDetectNi, hashConstData, defVersion, lastAutoDetectDt);

    if(hashRead.value("readArr_0").toByteArray() == QByteArray::fromHex("15")){
        if(gamadecoder.verbouseMode) qDebug() << "authorize error 15, try again" ;
        hashTmpData.insert(MeterPluginHelper::errWarnKey(error_counter, true), MeterPluginHelper::prettyMess(tr("Authorize error 15"), "", true, lastErr));
        hashTmpData.insert("error_counter", error_counter);
        hashTmpData.insert("warning_counter", warning_counter);

        hashTmpData.insert("logined", false);
        hashTmpData.insert("step", (quint16)0);
        hashTmpData.insert("CE303_BuvEkt", true);//спочатку закрити попередній сеанс
        return hashTmpData;
    }


    if(pollCode >= POLL_CODE_READ_METER_LOGBOOK && pollCode <= POLL_CODE_READ_END_MONTH && step < 2){

        gamadecoder.decodeServiceMessages(step, hashTmpData, hashRead, lastErr, error_counter);

        if(step < 2)
            pollCode = (quint8)0;
        if(gamadecoder.verbouseMode) qDebug() << "CE303 authorize module" << step;
    }

    switch(pollCode){
    case POLL_CODE_READ_TOTAL : gamadecoder.decodeTotalEnergyMessages(step, hashTmpData, hashConstData, hashRead, lastErr, error_counter, warning_counter)          ; break;
    case POLL_CODE_READ_VOLTAGE   : gamadecoder.decodeVoltageMessages(step, hashTmpData, hashConstData, hashRead, lastErr, error_counter, warning_counter); break;
    }


    if(step < 0xFFFF){
        if(hashTmpData.value("messFail").toBool()){
            hashTmpData.insert("logined", false);
            hashTmpData.insert("step", (quint16)0);
        }
    }
    hashTmpData.insert("error_counter", error_counter);
    hashTmpData.insert("warning_counter", warning_counter);

    if(gamadecoder.verbouseMode) qDebug() << "\r\nCE303::decodeGroupPollData "  << pollCode << skipMeterSN << hashTmpData.value("step").toUInt() << hashTmpData.value("messFail").toBool();
    return hashTmpData;
}

//-----------------------------------------------------------------------------------------
