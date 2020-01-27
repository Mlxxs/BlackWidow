/*
 * @Author:幽弥狂
 * @E-mail:1768478912@qq.com
 * @Phone:13812991101
 * @Date:2019-11-15
 * @License:LGPL3
 * @Function:计算文本相似度
 * @Description:筛选文本中最长的5个句子，计算其MD5值，存储至数据库中，
 *              添加新MD5值时先检测是否存在相同的MD5值
 */

#ifndef SIMILARITY_HPP
#define SIMILARITY_HPP

#include "base.hpp"
#include "dbprocessor.hpp"
#include "md5.hpp"

class Similarity{
private:
    DBProcessor *dbp;
    QString sql;
public:
    Similarity(){
        dbp = new DBProcessor("similarity","");
    }

    ~Similarity(){}

    //从文本中提取最长的一句话
    QString filterMaxStr(QString ctx){
        if(ctx.isEmpty()) return nullptr;
        QString result;

        QStringList symList;
        symList<<"。"<<"？"<<"！"<<"，"<<"."<<"?"<<"!"<<",";
        foreach(QString sym,symList){
            ctx.replace(sym,"<b>");
        }
        QStringList scentenceList = ctx.split("<b>");

        int size = 0;
        foreach(QString scentence,scentenceList){
            int temp = scentence.count();
            if(temp>size){
                result=scentence;
            }
        }
        return result;
    }

    QString getMD5(QString maxStr){
        if(maxStr.isEmpty()) return nullptr;
        QString result;
        string temp = maxStr.toStdString();
        result = QString::fromStdString(MD5(temp).toStr());

        return result;
    }

    bool save2db(QString md5){
        sql.clear();
        int maxID = dbp->getMaxId("md5");
        sql = QString("insert into md5 values(:id,:value)");
        dbp->query->prepare(sql);
        dbp->query->bindValue(":id",maxID);
        dbp->query->bindValue(":value",md5);
        if(dbp->query->exec()){
            qDebug()<<"Save value: "<<md5<<" into db";
        }
        else{
            qDebug()<<"Cannot insert data into db: "<<dbp->query->lastError();
            return false;
        }

        return true;
    }

    bool checkout(QString ctx){
        //获取最长子串
        QString maxstr = filterMaxStr(ctx);
        QString md = getMD5(maxstr);
        return save2db(md);
    }
};

#endif // SIMILARITY_HPP
