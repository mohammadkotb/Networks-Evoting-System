#include "client_socket.h" // Header file for server_socket
#include "packet.h"
#include "bernoulli_trial.h"
#include <sys/time.h>
#include <cerrno>

#define PACKET_LOSS_PROBABILITY 0
#define ACK_LOSS_PROBABILITY 0

#define PACKET_TIMEOUT 3000
#define TIMEOUT_COUNT 8

using std::cout;
using std::cerr;
using std::endl;

const int defaultPortNumber = 80;
char defaultServerName[32] = "localhost";

bool ClientSocket::init(char connection_type, int server_port_number, char *server_name){

    this->server_port_num = server_port_number;

    if(connection_type == 'T'){
        this->connection_type = SOCK_STREAM;
    } else if(connection_type == 'U'){
        this->connection_type = SOCK_DGRAM;
    } else{
        return false;
    }

    if(server_name){
        strcpy(this->server_name, server_name);
        if(!(server = gethostbyname(server_name))){
            cerr << "Error! Couldn't find server" << endl;
            return false;
        }
    } else{
        cerr << "Error! Null Argument Exception!" << endl;
        return false;
    }

    socket_file_descriptor = socket(AF_INET, this->connection_type, 0);
    if(socket_file_descriptor < 0){
        cerr << "Error Opening Socket" << endl;
        return false;
    }

    bzero((char*) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char*) server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(server_port_number);

    if(this->connection_type == SOCK_STREAM){
        if(connect(socket_file_descriptor, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
            cerr << "Error connecting to socket!" << endl;
            return false;
        }
    }

    //sendLastSyncBit initialized to true so on the first use it will be inverted to false
    sendLastSyncBit = true;
    //receiveLastSyncBit initialized to true so on the first use it will be inverted to false
    receiveLastSyncBit = true;

    return true;
}

ClientSocket::ClientSocket(char connection_type, int server_port_number){
    if(!init(connection_type, server_port_number, defaultServerName)){
        cerr << "Error Constructing ClientSocket Object" << endl;
        throw -1;
    }
}

ClientSocket::ClientSocket(char connection_type, int server_port_number, char *server_name){
    if(!init(connection_type, server_port_number, server_name)){
        cerr << "Error Constructing ClientScoket Object" << endl;
        throw -1;
    }
}

ClientSocket::ClientSocket(char connection_type){
    if(!init(connection_type, defaultPortNumber, defaultServerName)){
        cerr << "Error Constructing ClientSocket Object" << endl;
        throw -1;
    }
}

ClientSocket::~ClientSocket(){
    if(socket_file_descriptor >= 0)
        close(socket_file_descriptor);
}

int ClientSocket::readFromSocket(char buf[], int buffer_size){
    int ret=-1;
    memset(buf, 0, buffer_size);
    if(connection_type == SOCK_STREAM){
        ret = read(socket_file_descriptor, buf, buffer_size);
    } else if(connection_type == SOCK_DGRAM){
        //unreliable recvfrom
        //unsigned int sz = sizeof(sender_address);
        //ret = recvfrom(socket_file_descriptor, buf, buffer_size, 0, (struct sockaddr *)&sender_address, &sz);

        //reliable receive
        //wait till get the Packet with the correct Sync bit
        bool done = false;
        bool expectedSyncBit = !receiveLastSyncBit;
        BernoulliTrial bernoulli(ACK_LOSS_PROBABILITY);
        while (!done){
            unsigned int sz = sizeof(sender_address);
            ret = recvfrom(socket_file_descriptor, buf, buffer_size, 0, (struct sockaddr *)&sender_address, &sz);
            if (ret < 0) continue;

            Packet packet(buf,ret);
            if (packet.isDisconnect() || packet.isAck()){
                //corrupt packet since receiver is waiting for data packets
                //TODO: send ack packet with syncbit = lastSyncBit
                cout << "CORRUPT" << endl;
                continue;
            }

            cout << "<---- PACKET : " << packet.getSyncBit() << " received" << endl;

            //send ack packet
            Packet ackPacket(true,packet.getSyncBit(),false,(char*)"",0);
            if (!bernoulli.shouldDoIt()){
                sendto(socket_file_descriptor, ackPacket.getRawData(), ackPacket.getRawDataLength(),
                        0, (struct sockaddr *)&server_address, sizeof(server_address));
                cout << "----> PACKET : Ack "<< packet.getSyncBit() << " message Sent" << endl;
            }else{
                cout << "-||-> PACKET : Ack "<< packet.getSyncBit() << " message Simulated Loss" << endl;
            }

            if (expectedSyncBit != packet.getSyncBit()){
                //unsynchronized packet .. discard it
                cout << "----- PACKET : " << packet.getSyncBit() << " unsynced , discarded" << endl;
                continue;
            }
            receiveLastSyncBit = expectedSyncBit;
            done = true;
            memcpy(buf,packet.getData(),packet.getDataLength());
            ret = packet.getDataLength();
        }
    }

    return ret;
}

int ClientSocket::writeToSocket(char *msg){
    return writeToSocket(msg,strlen(msg)+1);
}

int ClientSocket::writeToSocket(char *msg,int n){
    int ret=-1;
    if(connection_type == SOCK_STREAM){
        ret = write(socket_file_descriptor, msg, n);
    } else if(connection_type == SOCK_DGRAM){
        //unreliable
        //ret = sendto(socket_file_descriptor, msg, n, 0, (const struct sockaddr *)&server_address,sizeof(server_address));
        //reliable connection over udp
        ret = reliableUdpSend(msg,n);
    }
    return ret;
}

int ClientSocket::reliableUdpSend(char* buffer,int length){
    //0 - create a new packet with syncbit = invertion the last one used
    //1 - send the packet (if bernoulli says ok :) )
    //2 - while not timeout (wait for the correct ACK packet)
    //3 - if timeout go to step 1 else, we are done
    //--------
    //0
    bool sync = !sendLastSyncBit;
    bool done = false;
    Packet packet(false,sync,false,buffer,length);
    BernoulliTrial bt(PACKET_LOSS_PROBABILITY);
    int count = 0;
    while (!done && count < TIMEOUT_COUNT){
        //1
        bool packet_lost = bt.shouldDoIt();
        if (!packet_lost){
            cout << "----> PACKET : "<< sync << " sent" << endl;
            int ret = sendto(socket_file_descriptor, packet.getRawData(), packet.getRawDataLength(), 0,
                    (const struct sockaddr *)&server_address,sizeof(server_address));
            //an error occured couldn't send the packet
            if (ret < 0)
                return ret;
        }else
            cout << "-||-> PACKET : "<< sync << " Simulated Loss" << endl;
            
        //2
        bool correctACK = false;
        int timeout = PACKET_TIMEOUT;
        struct timeval start,end;
        while (!correctACK){
            //wait for ACK (ack size is very small (2bytes))
            char data[10];
            unsigned int sz = sizeof(sender_address);
            gettimeofday(&start,NULL);
            int ackret = recvfromTimeout(socket_file_descriptor,(char*)data,10,
                    (struct sockaddr*)&sender_address,&sz,timeout);
            gettimeofday(&end,NULL);

            if (ackret == -1){
                //3- timeout , resend packet
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                else
                    return ackret;  //other error
            }
            //check packet
            Packet ackPacket(data,ackret);
            if (ackPacket.isAck() && ackPacket.getSyncBit() == sync){
                cout << "<---- ACK " << ackPacket.getSyncBit() << " packet received" << endl;
                //we are done
                correctACK = true;
                done = true;
            }else{
                int delta = (end.tv_sec*1000 + end.tv_usec/1000) -
                    (start.tv_sec*1000 + start.tv_usec/1000);
                timeout -= delta;
                //3- timeout , resend packet
                if (timeout <= 0) break;
            }
        }
        if (!correctACK)
            cout << "----- PACKET : "<< sync << " time out #" << count << endl;
        count++;
    }
    sendLastSyncBit = sync;
}

int ClientSocket::recvfromTimeout(int socket_fd,char * buff,int bufflen,struct sockaddr * client,socklen_t* client_len,int msec){
    struct timeval t;
    t.tv_sec = msec/1000;
    t.tv_usec = msec%1000;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(t)))
        return -1;
    int recv_count = recvfrom(socket_fd,buff,bufflen,0,client,client_len);
    return recv_count;
}

//------------------------------------------------------------------------
// client main
/*
int main(){
        int a, b;
        ClientSocket clientSocket('U', 6060);

        a = rand()%1000;
        b = rand()%1000;

        char buf[256];
        sprintf(buf, "%d %d", a, b);
        char str[256];
        sprintf(str, "sending message: %s", buf);
        cerr << str << endl;
        clientSocket.writeToSocket(buf);
        clientSocket.readFromSocket(buf, 256);

        cerr << "Returned Message = " << buf << endl;


        return 0;
}
//*/
/*
int main(){
	int a, b;
	ClientSocket clientSocket('T', 6060);

	char buf[256] = "send file";
	char str[256];
	sprintf(str, "sending message: %s", buf);
	cerr << str << endl;
	clientSocket.writeToSocket(buf);


	FILE *fout = fopen("img_copy.jpg", "w");

	char packet[256];
	memset(packet,0,256);
	int cnt=0;
	while(clientSocket.readFromSocket(packet, 256)){
		cerr << ++cnt << endl;
		fwrite(packet, 1, ret, fout);
		memset(packet,0,256);
	}

	fclose(fout);

	cerr << "Done, thank God :)" << endl;


	return 0;
} //*/
