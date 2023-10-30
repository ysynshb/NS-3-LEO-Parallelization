#pragma once
#include <chrono>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"
#include "ns3/applications-module.h"
typedef struct{
    uint32_t uuid;
    uint32_t epoch;
    uint32_t start;
    uint32_t count;
}abc_user_t;
namespace ns3{
inline int64_t WallTimeNowInUsec(){
    std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();    
    std::chrono::microseconds mic = std::chrono::duration_cast<std::chrono::microseconds>(d);
    return mic.count(); 
}
inline int64_t TimeMillis(){
    return WallTimeNowInUsec()/1000000;
}
void ns3_main(void *priv);
}
