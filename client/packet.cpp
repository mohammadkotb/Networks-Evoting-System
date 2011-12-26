#include "packet.h"
#include "stdlib.h"
#include "string.h"

#define KEY 0xAA

Packet::Packet(bool ack,bool sync,bool disconnect, char * data,int length){
    this->ack = ack;
    this->sync = sync;
    this->disconnect = disconnect;
    this->data = data;
    this->dataLength = length;
    this->createdFromRawData = false;

    //calculations
    this->rawData = (char *)malloc(sizeof(char) * (length + 1));
    this->rawDataLength = length+1;

    //adjust headers
    int header = 0;
    if (ack) header |= 100;
    if (sync) header |= 10;
    if (disconnect) header |= 1;
    this->rawData[0] = (char)header;

    //encrypt the data (simple xor encryption)
    this->rawData[0] = (char)(this->rawData[0] ^ (char)KEY);
    for(int i=0;i<length;++i)
        this->rawData[i+1] = (char)(data[i] ^ (char)KEY);
}

Packet::Packet(char * raw_data,int length){
    this->createdFromRawData = true;
    this->rawData = raw_data;
    this->rawDataLength = length;

    //create data buffer
    this->data = (char *)malloc(sizeof(char) * (length-1));
    this->dataLength = length-1;

    //decrypt the data
    int header = (char)(rawData[0] ^ (char)KEY);
    for(int i=0;i<length-1;++i)
        this->data[i] = (char)(raw_data[i+1] ^ (char)KEY);

    //extract headers
    ack = header & 100;
    sync = header & 10;
    disconnect = header & 1;
}

bool Packet::isAck(){
    return ack;
}

bool Packet::getSyncBit(){
    return sync;
}

bool Packet::isDisconnect(){
    return disconnect;
}

int Packet::getRawDataLength(){
    return rawDataLength;
}

int Packet::getDataLength(){
    return dataLength;
}

char * Packet::getData(){
    return data;
}

char * Packet::getRawData(){
    return rawData;
}

Packet::~Packet(){
    if (!createdFromRawData && rawData)
        delete(rawData);
    else
        if (data)
            delete(data);
}

