//
// Created by Kyle Sargeant on 3/9/22.
//

#ifndef NETWORKSLAB_FRAME_H
#define NETWORKSLAB_FRAME_H
class Frame{
public:
    long FrameID;
    bool DropBool;
    bool AckBool;
    bool ChecksumBool;
    struct Packet{
        float SeqNum;//AKA PacketID
        vector<char> data;
    };

    //getters
    long getFrameID(){
        return FrameID;
    }
    bool getDropBool(){
        return DropBool;
    }
    bool getAckBool(){
        return AckBool;
    }
    bool getChecksumBool(){
        return ChecksumBool;
    }
    float getSeqNum(){
        return Packet.SeqNum;
    }
    vector<char> getData(){
        return Packet.data;
    }
}


#endif //NETWORKSLAB_FRAME_H
