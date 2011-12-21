#ifndef PACKET_H_
#define PACKET_H_

/*
 * HEADER FORMAT
 * - - - - - A S D
 * 0 0 0 0 0 0 0 0
 * ---------------
 * A = ACK
 * S = SYNC
 * D = Disconnect
 */
class Packet{
    public:
        //creates a packet given the ack and sync bits and
        //the application layer data and length
        Packet(bool ack,bool sync,bool disconnect, char * data,int length);

        //creates a packet from the raw data received from
        //the network layer (ie the ack and sync bit will be
        //extracted from the raw_data)
        Packet(char * raw_data,int length);

        //return true if this packet is an ACK packet
        bool isAck();

        //returns true if this bit has a sequence of 1
        //and false if this bit has a sequence of 0
        bool getSyncBit();

        //returns true if this is a disconnect packet
        bool isDisconnect();

        //returns the length of the data encapsulated inside packet
        int getDataLength();

        //returns the length of the raw data
        int getRawDataLength();

        //returns data encapsulated inside packet
        char * getData();

        //returns packet raw data
        char * getRawData();

        ~Packet();
    private:
        bool sync;
        bool ack;
        bool disconnect;

        char * data;
        char * rawData;
        int dataLength;
        int rawDataLength;

        bool createdFromRawData;
};
#endif
