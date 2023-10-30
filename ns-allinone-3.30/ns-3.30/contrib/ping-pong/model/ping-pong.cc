#include "ns3/log.h"
#include "ping-pong.h"
namespace ns3{
NS_LOG_COMPONENT_DEFINE("ns3-ping-pong");
namespace{
const int kPacketSize = 1000;
}
enum Role{
    CLIENT,
    SERVER,
};
class UselessHeader:public Header{
public:
    UselessHeader(){}
    UselessHeader(uint32_t seq,uint32_t ts);
    static TypeId GetTypeId (void);
    TypeId GetInstanceTypeId (void) const override;
    void Print(std::ostream &os) const override;
    uint32_t GetSerializedSize (void) const override;
    void Serialize (Buffer::Iterator start) const override;
    uint32_t Deserialize (Buffer::Iterator start) override;
    uint32_t get_seq() const {return seq_;}
    uint32_t get_ts() const{return send_ts_;}
private:
    uint32_t seq_=0;
    uint32_t send_ts_=0;
};
UselessHeader::UselessHeader(uint32_t seq,uint32_t ts):seq_(seq),send_ts_(ts){}
TypeId UselessHeader::GetTypeId ()
{
    static TypeId tid = TypeId ("UselessHeader")
                        .SetParent<Header> ()
                        .AddConstructor<UselessHeader>();
    return tid;
}
TypeId UselessHeader::GetInstanceTypeId (void) const{
    return GetTypeId();
}
void UselessHeader::Print (std::ostream &os) const{
    os<<"UselessHeader";
}
uint32_t UselessHeader::GetSerializedSize () const{
    return sizeof(seq_)+sizeof(send_ts_);
}
void UselessHeader::Serialize (Buffer::Iterator i) const{
    i.WriteU32(seq_);
    i.WriteU32(send_ts_);
}
uint32_t UselessHeader::Deserialize (Buffer::Iterator i){
    seq_=i.ReadU32();
    send_ts_=i.ReadU32();
    return GetSerializedSize();
}
class PingPongApp:public Application{
public:
    PingPongApp(Role role,uint32_t uuid,uint32_t epoch,uint32_t first_seq,uint32_t total);
    void Bind(uint16_t port);
    InetSocketAddress GetLocalAddress();
    void ConfigurePeer(Ipv4Address peer_ip,uint16_t port);
    virtual void StartApplication() override;
    virtual void StopApplication() override;
private:
    void SendPing(Ipv4Address &ip,uint16_t port);
    void FeedPong(uint32_t seq,Ipv4Address &ip,uint16_t port);
    void RecvPacket(Ptr<Socket> socket);
    void SendToNetwork(Ptr<Packet> p,Ipv4Address &ip,uint16_t port);
    Role role_;
    uint32_t uuid_;
    uint32_t epoch_;
    uint32_t seq_;
    uint32_t start_seq_;
    uint32_t stop_seq_;
    Ptr<Socket> socket_;
    uint16_t bind_port_=0;
    Ipv4Address peer_ip_;
    uint16_t peer_port_;
};
PingPongApp::PingPongApp(Role role,uint32_t uuid,uint32_t epoch,uint32_t first_seq,uint32_t total)
:role_(role),uuid_(uuid),epoch_(epoch),seq_(first_seq),start_seq_(first_seq),stop_seq_(first_seq+total){}
void PingPongApp::Bind(uint16_t port){
    if (socket_== NULL) {
        socket_ = Socket::CreateSocket (GetNode (),UdpSocketFactory::GetTypeId ());
        auto local = InetSocketAddress{Ipv4Address::GetAny (), port};
        auto res = socket_->Bind (local);
        NS_ASSERT (res == 0);
    }
    bind_port_=port;
    socket_->SetRecvCallback (MakeCallback(&PingPongApp::RecvPacket,this));
}
InetSocketAddress PingPongApp::GetLocalAddress(){
    Ptr<Node> node=GetNode();
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
    Ipv4Address local_ip = ipv4->GetAddress (1, 0).GetLocal ();
    return InetSocketAddress{local_ip,bind_port_};
}
void PingPongApp::ConfigurePeer(Ipv4Address peer_ip,uint16_t port){
    peer_ip_=peer_ip;
    peer_port_=port;
}
void PingPongApp::StartApplication(){
    if(role_==Role::CLIENT&&peer_port_>0){
        SendPing(peer_ip_,peer_port_);
    }
}
void PingPongApp::StopApplication(){}
void PingPongApp::SendPing(Ipv4Address &ip,uint16_t port){
    if(role_==Role::CLIENT&&seq_<=stop_seq_){
        Ptr<Packet> p=Create<Packet>(kPacketSize);
        uint32_t now=Simulator::Now().GetMilliSeconds();
        UselessHeader header(seq_,now);
        p->AddHeader(header);
        SendToNetwork(p,ip,port);
        seq_++;
    }
}
void PingPongApp::FeedPong(uint32_t seq,Ipv4Address &ip,uint16_t port){
    if(role_==Role::SERVER){
        Ptr<Packet> p=Create<Packet>(kPacketSize);
        uint32_t now=Simulator::Now().GetMilliSeconds();
        UselessHeader header(seq,now);
        p->AddHeader(header);
        SendToNetwork(p,ip,port);
    }
}
void PingPongApp::SendToNetwork(Ptr<Packet> p,Ipv4Address &ip,uint16_t port){
    socket_->SendTo(p,0,InetSocketAddress{ip,port});
}
void PingPongApp::RecvPacket(Ptr<Socket> socket){
    Address remoteAddr;
    auto packet = socket->RecvFrom (remoteAddr);
    UselessHeader header;
    packet->RemoveHeader(header);
    uint32_t seq=header.get_seq();
    uint32_t send_ts=header.get_ts();
    if(role_==Role::CLIENT){
        SendPing(peer_ip_,peer_port_);
        uint32_t now=Simulator::Now().GetMilliSeconds();
        uint32_t delta=now-send_ts;
        if(start_seq_==seq||stop_seq_==seq){
            NS_LOG_INFO(uuid_<<" "<<epoch_<<" "<<seq<<" "<<delta);
        }
    }else{
        peer_ip_= InetSocketAddress::ConvertFrom (remoteAddr).GetIpv4 ();
        peer_port_= InetSocketAddress::ConvertFrom (remoteAddr).GetPort ();
        FeedPong(seq,peer_ip_,peer_port_);
    }
}
NodeContainer BuildExampleTopo (uint64_t bps,
                                       uint32_t msDelay,
                                       uint32_t msQdelay)
{
    NodeContainer nodes;
    nodes.Create (2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", DataRateValue  (DataRate (bps)));
    pointToPoint.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (msDelay)));
    auto bufSize = std::max<int> (kPacketSize, bps * msQdelay / 8000);
    int packets=bufSize/kPacketSize;
    pointToPoint.SetQueue ("ns3::DropTailQueue",
                           "MaxSize", StringValue (std::to_string(1)+"p"));
    NetDeviceContainer devices = pointToPoint.Install (nodes);

    InternetStackHelper stack;
    stack.Install (nodes);

    TrafficControlHelper pfifoHelper;
    uint16_t handle = pfifoHelper.SetRootQueueDisc ("ns3::FifoQueueDisc", "MaxSize", StringValue (std::to_string(packets)+"p"));
    pfifoHelper.AddInternalQueues (handle, 1, "ns3::DropTailQueue", "MaxSize",StringValue (std::to_string(packets)+"p"));
    pfifoHelper.Install(devices);
    
    Ipv4AddressHelper address;
    std::string nodeip="10.1.1.0";
    address.SetBase (nodeip.c_str(), "255.255.255.0");
    address.Assign (devices);
    return nodes;
}
void InstallPingApplication(
                        Ptr<Node> sender,
                        Ptr<Node> receiver,
                        uint16_t send_port,
                        uint16_t recv_port,
                        Time startTime,
                        abc_user_t *priv){
    Ptr<PingPongApp> sendApp = 
            CreateObject<PingPongApp>(Role::CLIENT,priv->uuid,priv->epoch,priv->start,priv->count);
    Ptr<PingPongApp> recvApp = CreateObject<PingPongApp>(Role::SERVER,priv->uuid,priv->epoch,0,0);
    sender->AddApplication (sendApp);
    receiver->AddApplication (recvApp);
    Ptr<Ipv4> ipv4 = receiver->GetObject<Ipv4> ();
    Ipv4Address receiverIp = ipv4->GetAddress (1, 0).GetLocal();
    recvApp->Bind(recv_port);
    sendApp->Bind(send_port);
    sendApp->ConfigurePeer(receiverIp,recv_port);
    sendApp->SetStartTime (startTime);
    recvApp->SetStartTime (startTime);
}
void ns3_main(void *priv){
    LogComponentEnable("ns3-ping-pong",LOG_LEVEL_INFO);
    abc_user_t *user=(abc_user_t*)priv;
    uint64_t linkBw   = 3000000;
    uint32_t msDelay  = 100;
    uint32_t msQDelay = 20;
    uint32_t send_port=1234;
    uint32_t recv_port=1234;
    NodeContainer nodes = BuildExampleTopo (linkBw, msDelay, msQDelay);
    InstallPingApplication(nodes.Get(0), nodes.Get(1),
                             send_port,recv_port,
                             Seconds(1),user);
    Simulator::Run ();
    Simulator::Destroy();
}
}
