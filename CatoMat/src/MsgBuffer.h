#ifndef MSGBUFFER_H
#define MSGBUFFER_H

class MsgBuffer
{
public:
    static const unsigned int Q_SIZE = 10;
    static const unsigned int MSG_SIZE = 6;

    String ID;

    MsgBuffer()
    {
        msgCount = 0;
    }

    bool Put(const byte msg[])
    {
       //LOG(ID + " MsgBuffer::Put()");
        if(msgCount == Q_SIZE)
        {
            LOG(ID + " MsgBuffer::Put(): Buffer full!");
            lock = false;
            return false;
        }

        //debugBuffOut(msg);

        if(lock)
            return false;
        lock = true;

        memcpy(queue[msgCount], msg, MSG_SIZE);
        msgCount++;

        LOG(ID + " MsgBuffer::Put(): Msg added. Total Msgs=" + msgCount);

        lock = false;
        return true;
    }

    bool Pop(byte msg[])
    {
        if(msgCount == 0)
        {
            //LOG(ID + " MsgBuffer::Pop(): No messages");
            return false;
        }


        if(lock)
            return false;
        lock = true;
        //LOG(ID + " MsgBuffer::Pop()");

        memcpy(msg, queue[0], MSG_SIZE);
        //debugBuffOut(msg);
        memcpy(queue[0], queue[1], MSG_SIZE * (msgCount-1));
        msgCount--;

        LOG(ID + " MsgBuffer::Pop(): Msg poped. Total Msgs=" + msgCount);

        lock = false;
        return true;
    }

private:
    byte queue[Q_SIZE][MSG_SIZE];
    unsigned int msgCount;
    volatile bool lock;

    static void debugBuffOut(byte buff[MsgBuffer::MSG_SIZE])
  	{
  		char b[14];
  		sprintf(b, "%2X%2X%2X:%2X%2X%2X", buff[0], buff[1], buff[2], buff[3], buff[4], buff[5]);
  		Serial.println(b);
  	}
};

#endif
