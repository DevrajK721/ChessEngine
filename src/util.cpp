#include "util.hpp"
#include <string>

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64_encode(const std::string &data) {
    std::string out;
    int val=0, valb=-6;
    for(unsigned char c : data){
        val = (val<<8) + c;
        valb += 8;
        while(valb >= 0){
            out.push_back(b64_table[(val>>valb)&0x3F]);
            valb -= 6;
        }
    }
    if(valb>-6) out.push_back(b64_table[((val<<8)>>(valb+8))&0x3F]);
    while(out.size()%4) out.push_back('=');
    return out;
}