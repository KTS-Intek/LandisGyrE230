#include "landisgyre230encoderdecoder.h"


#include <QtCore>

///[!] meter-plugin-shared
#include "shared/meterpluginhelper.h"

///[!] type-converter
#include "src/base/prettyvalues.h"
#include "src/base/convertatype.h"



#include "ucmetereventcodes.h"
#include "moji_defy.h"
#include "matildalimits.h"

#include "definedpollcodes.h"


//----------------------------------------------------------------------

LandisGyrE230EncoderDecoder::LandisGyrE230EncoderDecoder(QObject *parent) : IEC62056_21_Helper(parent)
{
    readNumber = 2;//R2
}

//----------------------------------------------------------------------

QString LandisGyrE230EncoderDecoder::getSupportedMetersPrefixes()
{
    return QString("/LGZ5ZMR110");// /LGZ5ZMR110C

}

//----------------------------------------------------------------------

QByteArray LandisGyrE230EncoderDecoder::getDefPasswd()
{
    return QByteArray();

}

//----------------------------------------------------------------------

bool LandisGyrE230EncoderDecoder::isLogin2supportedMeter(const QVariantHash &hashRead)
{
    return isLogin2supportedMeterExt(hashRead, getSupportedMetersPrefixes());

}

//----------------------------------------------------------------------

bool LandisGyrE230EncoderDecoder::isLogin2supportedMeter(const QByteArray &readArr)
{
    return isLogin2supportedMeterExt(readArr, getSupportedMetersPrefixes());

}

//----------------------------------------------------------------------

QString LandisGyrE230EncoderDecoder::versionFromMessage(const QVariantHash &hashRead)
{
    QString vrsn = versionFromMessageExt(hashRead, "/LGZ5ZMR110", -1).simplified().trimmed();// /LGZ5 ZMR110CR.K53
    return vrsn;

}

//----------------------------------------------------------------------



bool LandisGyrE230EncoderDecoder::decodeMeterSN(const QVariantHash &hashRead, const quint16 &goodStep, QVariantHash &hashTmpData, ErrsStrct &errWarn, int &error_counter)
{
    //02 44 30 30 30 28 33 30 39 34 30 39 33 36 29 03 .D000(30940936)
    //74                                              t

    bool isSnBroken;
    const bool r = decodeMeterSNBase(hashRead, "02 44 30 30 30 28", goodStep, hashTmpData, isSnBroken);
    if(!r && error_counter >= 0){
        hashTmpData.insert(MeterPluginHelper::errWarnKey(error_counter, true),
                       MeterPluginHelper::prettyMess(QString("!MeterSN.isValid: %1").arg(hashRead.value("readArr_0").toString().simplified().trimmed()),
                                                     PrettyValues::prettyHexDump(hashRead.value("readArr_0").toByteArray()), true, errWarn));
    }
    return r;
}

//QString LandisGyrE230EncoderDecoder::meterProgVersion(const QString &readArr)
//{
//use versionFromMessage
//}

//----------------------------------------------------------------------

bool LandisGyrE230EncoderDecoder::isAPolyPhaseMeter(const QVariantHash &hashTmpData)
{
    return  isAPolyPhaseMeter(hashTmpData.value("vrsn").toString());

}

//----------------------------------------------------------------------

bool LandisGyrE230EncoderDecoder::isAPolyPhaseMeter(const QString &vrsn)
{
    Q_UNUSED(vrsn);
    return true;//sure it is only polyPhase  !vrsn.startsWith("G1M"); //G3M or G1M

}

//----------------------------------------------------------------------

bool LandisGyrE230EncoderDecoder::isItYourReadMessage(const QByteArray &arr)
{
    return isItYourReadMessageExt(arr, getSupportedMetersPrefixes());

}

//----------------------------------------------------------------------

QStringList LandisGyrE230EncoderDecoder::getSupportedEnrg(const quint8 &code, const QString &vrsn)
{
    Q_UNUSED(vrsn);
     if(code == POLL_CODE_READ_VOLTAGE)
         return isAPolyPhaseMeter(vrsn) ?
                     QString("UA,UB,UC,IA,IB,IC,cos_fA,cos_fB,cos_fC,F").split(',') :
                     QString("UA,IA,cos_fA,PA,F").split(',');

     return QString("A+,A-,R+,R-").split(",");
}
//----------------------------------------------------------------------

int LandisGyrE230EncoderDecoder::calculateEnrgIndx(qint16 currEnrg, const quint8 &pollCode, const QStringList &listEnrg, const QString &version)
{
    //currEnrg індекс в списку опитування
    const QStringList spprtdListEnrg = getSupportedEnrg(pollCode, version);
    const QStringList listPlgEnrg = getSupportedEnrg(pollCode, "");//енергія підтримувана плагіном

    return calculateEnrgIndxExt(currEnrg, listEnrg, spprtdListEnrg, listPlgEnrg);

}


//----------------------------------------------------------------------

QVariantHash LandisGyrE230EncoderDecoder::getServiceMessages(quint16 &step, QVariantHash &hashTmpData, const QByteArray &arrNI)
{


    QVariantHash hashMessage;




    if(step < 1 && hashTmpData.value("logined", false).toBool()){
        step = 1;
    }else{
        if(hashTmpData.value("E230_logOutAfter").toBool()){
            step = 0;
        }
    }


    if(verbouseMode) qDebug() << "CE303 step " << step << hashTmpData.value("vrsn").toString();

    switch(step){
    case 0: {//log in start0
        hashTmpData.insert("E230_logOutAfter", false);

        hashMessage = getStep0HashMesssage(hashTmpData, arrNI);
        break;}

    case 1: {//log in start1

        //it is for data reading, for dt writing use another way
        hashMessage = getStep1HashMesssageLowLevel();
        hashMessage.insert("heavy_answer_0", true); //it is about 4K
        hashMessage.insert("endSymb2", QByteArray::fromHex("0D 0A 03"));//


        hashTmpData.insert("E230_logOutAfter", true);

        break;}




    }


    return hashMessage;


}

//----------------------------------------------------------------------

void LandisGyrE230EncoderDecoder::decodeServiceMessages(quint16 &step, QVariantHash &hashTmpData, const QVariantHash &hashRead, ErrsStrct &errWarn, int &error_counter)
{
    if(verbouseMode) qDebug() << "CE303 step " << step << hashTmpData.value("vrsn").toString();



    switch (step) {
    case 0: {//log in start0

        if(isLogin2supportedMeter(hashRead)){

            hashTmpData.insert("vrsn", versionFromMessage(hashRead));
            if(verbouseMode) qDebug() << "versionFromMess=" << hashTmpData.value("CE303_vrsnPart").toString();

            markStep0Done(hashTmpData);

        }
        break;}

    case 1: {//log in start1
        //for data reading , do not do anything
        step = 2;

//        isLoginGoodSmart(hashRead,  2, hashTmpData);
        break;}


//    case 2: { decodeMeterSN(hashRead, 7, hashTmpData, errWarn, error_counter); break; }//read serial number

//    case 5:{ decodeMeterTime(hashRead, 5, hashTmpData, lastErr, error_counter); break;} // read time

//    case 6:{ //read date
//        if(!CE303helper::decodeMeterDate(hashRead, hashConstData, 7, hashTmpData, lastErr, error_counter, warning_counter, step) && step == 0xFFFF)
//            return hashTmpData;
//        break;}
    }
}

//----------------------------------------------------------------------

bool LandisGyrE230EncoderDecoder::canProcessDtSNObisCodes(const QHash<QString, IEC62056_21_Helper::OneIECParsedAnswer> &hAnswers, const QVariantHash &hashConstData, QVariantHash &hashTmpData, ErrsStrct &errWarn, int &error_counter, int &warning_counter)
{

    //C.90.1(0000000030940936)
    bool ok;
    const quint64 metersn = hAnswers.value("C.90.1").value.toULongLong(&ok);

    if(ok && metersn > 0)
        hashTmpData.insert("SN", QString::number(metersn));

    //0.9.2(21-05-25)<CR><LF>0.9.1(14:40:41)
    const QDate meterdate = QDate::fromString(QString("20%1").arg(hAnswers.value("0.9.2").value), "yyyy-MM-dd");
    const QTime metertime = QTime::fromString(hAnswers.value("0.9.1").value, "hh:mm:ss");

    const QDateTime meterdt = QDateTime(meterdate, metertime);


    bool isMeterDtOk = meterdt.isValid();

    if(isMeterDtOk){//const bool asLocalTime = true, const QDateTime dtpwrCorr25hour = QDateTime(), const bool ignoredst = false

        hashTmpData.insert("lastMeterDateTime", meterdt);

        const QString dtlines = hAnswers.value("0.9.2").value + " " + hAnswers.value("0.9.1").value;


        const bool lockWriteDt = (hashConstData.value("memo").toString().startsWith("DNWT ") || hashConstData.value("dta", false).toBool());
        const QDateTime currDt = QDateTime::currentDateTime();
        if(lockWriteDt){
            //lock write
            hashTmpData.insert("IEC_timeDiffSec", meterdt.secsTo(currDt));
            hashTmpData.insert("IEC_DateTime", meterdt);


            hashTmpData.insert("IEC_currentDateTime", currDt);

            hashTmpData.insert(MeterPluginHelper::errWarnKey(error_counter, true),
                        MeterPluginHelper::prettyMess(tr("Time correction: disabled, diff.: %1 [s], ignore")
                                                      .arg( meterdt.secsTo(currDt) ),
                                                      PrettyValues::prettyHexDump(dtlines.toLocal8Bit(), QByteArray(), 0),
                                                      false, errWarn));



        }



        if(!lockWriteDt && qAbs(meterdt.secsTo(currDt)) > hashConstData.value("corDTintrvl", SETT_DT_DIFF_MAX_SECS).toInt()){ //need to correct dateTime  90 sec;
//                        hash.insert("corrDateTime", true);ignore date time
            hashTmpData.insert(MeterPluginHelper::nextMatildaEvntName(hashTmpData), MeterPluginHelper::addEvnt2hash(ZBR_EVENT_DATETIME_NEED2CORR, QDateTime::currentDateTimeUtc(),
                                                                              tr("Meter date %1 UTC%2%3")
                                                                              .arg(meterdt.toString("yyyy-MM-dd hh:mm:ss"))
                                                                              .arg( (meterdt.offsetFromUtc() >= 0) ? "+" : "" )
                                                                              .arg(QString::number(meterdt.offsetFromUtc()))) );


            hashTmpData.insert(MeterPluginHelper::errWarnKey(warning_counter, false),
                               MeterPluginHelper::prettyMess(tr("Need to correct time, diff.: %1 [s]").arg( meterdt.secsTo(currDt) ),
                                                             PrettyValues::prettyHexDump(dtlines.toLocal8Bit(), QByteArray(), 0),
                                                             false, errWarn));

            if(!hashConstData.value("corDTallow", false).toBool()){
                hashTmpData.insert(MeterPluginHelper::errWarnKey(error_counter, true),
                                   MeterPluginHelper::prettyMess(tr("Time correction: disabled, diff.: %1 [s], exiting")
                                                                               .arg( meterdt.secsTo(currDt) ),
                                                                 PrettyValues::prettyHexDump(dtlines.toLocal8Bit(), QByteArray(), 0),
                                                                 true,  errWarn));

//                            step = 0xFFFF; ignore date time

            }
            if(verbouseMode) qDebug() << "IEC need to correct dateTime" << meterdt << currDt;
//            isMeterDtOk = false;
//                        break; ignore date time
        }
        hashTmpData.insert("IEC_currentDateTime", currDt);


        hashTmpData.insert("IEC_DateTime", meterdt);
        hashTmpData.insert("lastMeterDateTime", meterdt);
    }





    return (ok && isMeterDtOk);

}


//----------------------------------------------------------------------


QStringList LandisGyrE230EncoderDecoder::getTotalEnergyObisCodes(const QVariantHash &hashTmpData, const QVariantHash &hashConstData, QStringList &enrgTariffL)
{
    QStringList obisl;

    const quint8 pollCode = POLL_CODE_READ_TOTAL;
//    qint16 currEnrg = 0;// hashTmpData.value("currEnrg", (qint16)(0)).toInt();

    const QString vrsn = hashTmpData.value("vrsn").toString();
    const QStringList listEnrg = hashConstData.value("listEnrg").toStringList();

    const int maxTariff = hashConstData.value("trff", 3).toInt();


    for(quint16 currEnrg = 0, imax = listEnrg.size(); currEnrg < imax; currEnrg++){
        //current A+ = ET0PE() ,  A- = ET0PI() , R+ = ET0QE() , R- = ET0QI()



        const int energyIndex = calculateEnrgIndx(currEnrg, pollCode, listEnrg, vrsn);//відповідник для індеку currEnrg в списку енергій плагіну
//        const QStringList listPlgEnrg = getSupportedEnrg(pollCode, "");

        if(energyIndex > 3 || energyIndex < 0)
            continue;
//            energyIndex = 0;

        //        currEnrg = listEnrg.indexOf( listPlgEnrg.at(energyIndex) );
        //        hashTmpData.insert("currEnrg", currEnrg);

        const QString enrgName = listEnrg.at(currEnrg);

        if(verbouseMode) qDebug() << currEnrg  << energyIndex << enrgName;


        for(int tariff = 0; tariff <= maxTariff; tariff++){

            obisl.append(QString("%1.8.%2").arg((energyIndex + 1)).arg(tariff));

            enrgTariffL.append(QString("T%1_%2").arg(tariff).arg(enrgName));


        }
    }

    return obisl;
}

//----------------------------------------------------------------------

void LandisGyrE230EncoderDecoder::decodeTotalEnergyMessages(quint16 &step, QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QVariantHash &hashRead, ErrsStrct &errWarn, int &error_counter, int &warning_counter)
{

    const QHash<QString, OneIECParsedAnswer> hAnswers = getAnswersExt(hashRead);

    if(hAnswers.isEmpty())
        return;//someting bad happened

    if(!canProcessDtSNObisCodes(hAnswers, hashConstData, hashTmpData, errWarn, error_counter , warning_counter))
        return ;

    QStringList enrgKeyL;
    const QStringList obisl = getTotalEnergyObisCodes(hashTmpData, hashConstData, enrgKeyL);


    for(int i = 0, imax = obisl.size(); i < imax; i++){
        const QString inStr = hAnswers.value(obisl.at(i)).value;
        if(inStr.isEmpty()){
            if(verbouseMode)
                qDebug() << "decodeTotalEnergy ignore " << obisl.at(i) << enrgKeyL.at(i) ;
            continue;
        }

        bool ok;
        const qreal val = inStr.toDouble(&ok);
        const QString valStr = PrettyValues::prettyNumber(val, 3);
        if(!ok){
            if(verbouseMode)
                qDebug() << "decodeTotalEnergy bad value " << obisl.at(i) << enrgKeyL.at(i) << inStr << val << ok;

            hashTmpData.insert(enrgKeyL.at(i), "-");
            continue;
        }


        hashTmpData.insert(enrgKeyL.at(i), valStr);// QString("T%1_%2").arg( QString::number(tariff)).arg(energyKey), valStr.isEmpty() ? "-" : valStr);

    }

    step = 11;
    hashTmpData.insert("step", step);
    hashTmpData.insert("messFail", false);



}

//----------------------------------------------------------------------


QStringList LandisGyrE230EncoderDecoder::getVoltageObisCodes( QStringList &enrgKeyL)
{
    QStringList obisl;


    for(int i = 0; i < 16; i++){
        QString obiscode;
        QString valueUnits;


        switch(i){
        case 0: obiscode = "14.7.0"; valueUnits = "F"; break; //.R5.14.7.0().e
        case 1: obiscode = "36.7.0" ; valueUnits = "PA"; break;   // IA          .R5.31.7.0().b
        case 2: obiscode = "151.7.0"; valueUnits = "QA"; break;   // IA          .R5.31.7.0().b

        case 3: obiscode = "31.7.0" ; valueUnits = "IA"; break;   // IA          .R5.31.7.0().b
        case 4: obiscode = "32.7.0" ; valueUnits = "UA"; break;//UA          .R5.32.7.0().a
        case 5: obiscode = "33.7.0" ; valueUnits = "cos_fA"; break; //  cos_fA      .R5.33.7.0().`



        case 6: obiscode = "56.7.0" ; valueUnits = "PB"; break;   // IA          .R5.31.7.0().b
        case 7: obiscode = "171.7.0"; valueUnits = "QB"; break;   // IA          .R5.31.7.0().b

        case 8: obiscode = "51.7.0" ; valueUnits = "IB"; break; //  IB      .R5.51.7.0().d
        case 9: obiscode = "52.7.0" ; valueUnits = "UB"; break; //   UB     .R5.52.7.0().g
        case 10: obiscode = "53.7.0"; valueUnits = "cos_fB"; break; //  cos_fB     .R5.53.7.0().f



        case 11: obiscode = "76.7.0"; valueUnits = "PC"; break;   // IA          .R5.31.7.0().b
        case 12: obiscode = "191.7.0"; valueUnits = "QC"; break;   // IA          .R5.31.7.0().b

        case 13: obiscode = "71.7.0"; valueUnits = "IC"; break; //   IC    .R5.71.7.0().f
        case 14: obiscode = "72.7.0"; valueUnits = "UC"; break;  //     UC   .R5.72.7.0().e
        case 15: obiscode = "73.7.0"; valueUnits = "cos_fC"; break;  //     cos_fC   .R5.73.7.0().d



        }

        if(obiscode.isEmpty())
            break;

        obisl.append(obiscode);
        enrgKeyL.append(valueUnits);

    }

    return obisl;
}

//----------------------------------------------------------------------

void LandisGyrE230EncoderDecoder::decodeVoltageMessages(quint16 &step, QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QVariantHash &hashRead, ErrsStrct &errWarn, int &error_counter, int &warning_counter)
{


    const QHash<QString, OneIECParsedAnswer> hAnswers = getAnswersExt(hashRead);

    if(hAnswers.isEmpty())
        return;//someting bad happened

    if(!canProcessDtSNObisCodes(hAnswers, hashConstData, hashTmpData, errWarn, error_counter, warning_counter ))
        return ;

    QStringList enrgKeyL;
    const QStringList obisl = getVoltageObisCodes(enrgKeyL);


    for(int i = 0, imax = obisl.size(); i < imax; i++){
        const QString inStr = hAnswers.value(obisl.at(i)).value;
        if(inStr.isEmpty()){
            if(verbouseMode)
                qDebug() << "decodeVoltageMessages ignore " << obisl.at(i) << enrgKeyL.at(i) ;
            continue;
        }

        bool ok;
        const qreal val = inStr.toDouble(&ok);
        const QString valStr = PrettyValues::prettyNumber(val, 3);
        if(!ok){
            if(verbouseMode)
                qDebug() << "decodeVoltageMessages bad value " << obisl.at(i) << enrgKeyL.at(i) << inStr << val << ok;

            hashTmpData.insert(enrgKeyL.at(i), "!");
            continue;
        }


        hashTmpData.insert(enrgKeyL.at(i), valStr);// QString("T%1_%2").arg( QString::number(tariff)).arg(energyKey), valStr.isEmpty() ? "-" : valStr);

    }

   step = 11;
    hashTmpData.insert("step", step);
    hashTmpData.insert("messFail", false);


}

QVariantHash LandisGyrE230EncoderDecoder::isItYourExt(const QByteArray &arr, QByteArray &lastDN, QTime &timeFromLastAuth)
{
    QByteArray mArr = ConvertAtype::convertData7ToData8(arr);
    QVariantHash hash;
    hash.insert("endSymb2", QByteArray::fromHex("03"));
    hash.insert("7DtEpt",true);

    //old addres
    if(mArr == "/?!\r\n"){
        lastDN = "\r\n";
        hash.remove("endSymb2");
         hash.insert("endSymb", "\r\n");
        hash.insert("nodeID", lastDN);
        timeFromLastAuth.restart();
        return hash;
    }

    if(mArr.left(2) == "/?" && mArr.right(3) == "!\r\n"){
        mArr.chop(3);
        lastDN = mArr.mid(2) + "\r\n";
        hash.insert("nodeID", lastDN);
        hash.remove("endSymb2");
         hash.insert("endSymb", "\r\n");
         timeFromLastAuth.restart();
        return hash;
    }

    //IEC Data unit
    if(mArr == QByteArray::fromHex("01 42 30 03 71")){//.B0.u
        //lastDN.clear();
        hash.insert("nodeID", lastDN);// "\r\n");
        lastDN.clear();
        return hash;
    }
    if(lastDN.isEmpty())
        return hash;

   QByteArray leftArr = QByteArray::fromHex("01");
   QByteArray rightArr = QByteArray::fromHex("03");
   int mArrLen = mArr.length();

    if(mArr.right(1) != QByteArray::fromHex("03"))
        rightArr.append(byteBCC(mArr.left(mArrLen - 1)));

    if((mArr.left(4) == QByteArray::fromHex("01573102") || mArr.left(4) == QByteArray::fromHex("01503102")  )&& mArr.right(rightArr.length()) == rightArr){
        hash.insert("readLen", 1);
        hash.remove("endSymb2");
        hash.insert("endSymb", QByteArray::fromHex("06"));
        hash.insert("nodeID", lastDN);
        return hash;
    }

    if(mArr.left(leftArr.length()) == leftArr && mArr.right(rightArr.length()) == rightArr){

        hash.insert("nodeID", lastDN);
        return hash;
    }


    leftArr = QByteArray::fromHex("06");
    rightArr = "\r\n";
    if(mArr.left(leftArr.length()) == leftArr && mArr.right(rightArr.length()) == rightArr){
        hash.insert("nodeID", lastDN);
//        hash.remove("endSymb2");
//         hash.insert("endSymb", QByteArray::fromHex("06"));
        return hash;
    }



//    if(timeFromLastAuth.elapsed() < 5000 ){
//        if(mArr.length() == 5 && mArr.right(2) == "\r\n"){
//            hash.insert("nodeID", lastDN);
//            return hash;
//        }
//    }
    return QVariantHash();
}

//----------------------------------------------------------------------


