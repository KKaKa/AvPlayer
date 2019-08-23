//
// Created by Administrator on 2019/8/21 0021.
//

#ifndef AVPALYER_BASECHANNEL_H
#define AVPALYER_BASECHANNEL_H

/**
 * Video Audio 的父类
 */
class BaseChannel {
public:
    BaseChannel(int id) : id(id){

    }

    virtual ~BaseChannel() {

    }

private:
    int id;
};


#endif //AVPALYER_BASECHANNEL_H
