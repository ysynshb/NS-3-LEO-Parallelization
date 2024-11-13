

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mpi-interface.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/olsrs-routing-protocol.h"
#include "ns3/olsrs-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/csma-module.h"
#include "ns3/aodvs-module.h"
#include "ns3/aodv-module.h"
#include "ns3/ping-pong-module.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/leo-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SimpleDistributed");
uint32_t pktrecv = 0;
uint32_t pktsend = 0;
void ReceivePacket (Ptr<Socket> socket)
{ //recvPkt++;
  pktrecv++;
  //NS_LOG_UNCOND ("Received packet "<<pktrecv<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!");  
  //std::cout<<pktcount<<std::endl;
  //NS_LOG_UNCOND(pktcount);
  
}

static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,
                             uint32_t pktCount, Time pktInterval )
{ //uint32_t pktAll=pktCount;
  if (pktCount > 0)
    { pktsend++;
      //NS_LOG_UNCOND ("sending packet "<<pktsend);
      socket->Send (Create<Packet> (pktSize));
      Simulator::Schedule (pktInterval, &GenerateTraffic,
                           socket, pktSize,pktCount - 1, pktInterval);
    }
  else
    {
      socket->Close ();
    }
}
/*
void OlsrAndAodvStack(Ptr<Node> node)
{      Ptr<Ipv4> stack0 = node->GetObject<Ipv4> ();
      Ptr<Ipv4RoutingProtocol> rp_Gw = (stack0->GetRoutingProtocol ());
      Ptr<Ipv4ListRouting> lrp_Gw = DynamicCast<Ipv4ListRouting> (rp_Gw);

      Ptr<aodvs::RoutingProtocol> aodvspro;
      Ptr<olsrs::RoutingProtocol> olsrspro;
      for (uint32_t i = 0; i < lrp_Gw->GetNRoutingProtocols ();  i++)
      {
         int16_t priority;
         Ptr<Ipv4RoutingProtocol> temp = lrp_Gw->GetRoutingProtocol (i, priority);
         if (DynamicCast<aodvs::RoutingProtocol> (temp))
        {
          aodvspro = DynamicCast<aodvs::RoutingProtocol> (temp);
        }
         if (DynamicCast<olsrs::RoutingProtocol> (temp))
        {
          olsrspro = DynamicCast<olsrs::RoutingProtocol> (temp);
        }
      }
      //Ptr<NetDevice> dev = node->GetDevice(0);
      //aodvspro->AddOlsrRoute (dev,Ipv4Address("10.3.1.1"),Ipv4Address("10.3.1.1"),Ipv4InterfaceAddress(Ipv4Address("10.3.1.2"),Ipv4Mask("255.255.255.0")),1);
      olsrspro->AddAodvProtocol(aodvspro);
}
*/

void CourseChange (std::string context, Ptr<const MobilityModel> position)
{
  Vector pos = position->GetPosition ();
  Ptr<const Node> node = position->GetObject<Node> ();
  //std::cout << Simulator::Now () << "，" << "节点"<<node->GetId () << ":  " << pos.x << " | " << pos.y << " | " << pos.z << " | "<< std::endl;
  //std::cout << "1" <<std::endl;
  //if(node->GetId ()==0){NS_LOG_UNCOND ("节点 "<<node->GetId ()<<" "<<pos.z);}
  if(pos.z>7114500 || pos.z<-7114500)
  {
    NS_LOG_UNCOND (Simulator::Now ()<<", 节点 "<<node->GetId ()<<" 位置大于70度");
  }else
    {
      NS_LOG_UNCOND (Simulator::Now ()<<", 节点 "<<node->GetId ()<<" 位置小于70度");
    }
  for(uint32_t i = 0 ;i<node->GetNDevices();i++)
    {Ptr<NetDevice> dev =node->GetDevice(i);
     Ptr<Channel> channel=dev->GetChannel();
     if(DynamicCast<PointToPointChannel> (channel))
       {//std::cout<<"存在p2pchannel";
        //std::cout<<i<<std::endl;
        Ptr<PointToPointNetDevice> p2pdev = DynamicCast<PointToPointNetDevice> (dev);
        Ptr<PointToPointChannel> p2pchannel = DynamicCast<PointToPointChannel> (channel);
         Ptr<PointToPointNetDevice> p2pdevice1 = p2pchannel->GetPointToPointDevice(0);
        Ptr<PointToPointNetDevice> p2pdevice2 = p2pchannel->GetPointToPointDevice(1);
        if(p2pdev==p2pdevice1){//std::cout<<"等于1"<<std::endl;
				};
	if(p2pdev==p2pdevice2){//std::cout<<"等于2"<<std::endl;
				}
	//p2pchannel->shuchu();
        uint16_t lastp2pdevstate = p2pdev->getdevicestate();
	if(pos.z>7114500 || pos.z<-7114500)
          {     
                if(lastp2pdevstate ==1 || lastp2pdevstate ==3)
                  {p2pdev->setdevicestate(true);
   		p2pchannel->setchannelstate(false);
                std::cout<< "节点"<<node->GetId ()<<"的"<<i<<"端口因高纬度关闭，所在信道关闭"<<std::endl;}
                
		//std::cout<< "节点"<<node->GetId ()<<"位置大于70度:"<< pos.z<<std::endl;
	  } else 
	    {   
                if((p2pdevice1->getdevicestate()==3) || (p2pdevice2->getdevicestate()==3))
                {
                  NS_LOG_UNCOND ("节点 "<<node->GetId ()<<"的"<<i<<"端口所在的信道关闭");
                  p2pchannel->setchannelstate(false);
                }
                if(lastp2pdevstate ==2 || lastp2pdevstate ==4)
                 {p2pdev->setdevicestate(true);}
                if((p2pdevice1->getdevicestate()==1) && (p2pdevice2->getdevicestate()==1))
                  {  p2pchannel->setchannelstate(true);
                     NS_LOG_UNCOND ("节点 "<<node->GetId ()<<"的"<<i<<" 端口恢复正常@@@@@@@@@@@");
		    }
                //NS_LOG_UNCOND ("节点 "<<node->GetId ()<<" 位置小于70度=========");
                //p2pchannel->setchannelstate(true);
                
	     }
	}
    }
  
}

int
main (int argc, char *argv[])
{

  uint32_t numorbits = 6;
  uint32_t numsatellites = 8; 
  uint32_t duration =150;
  double interval = 0.01; // seconds
  uint32_t sendnode = 2;
  uint32_t last=TimeMillis();
  uint32_t packetSize = 1000; // bytes
  uint32_t numPackets = 100000000;
  std::string traceFile = "pcap/leocompare.log";
  Time interPacketInterval = Seconds (interval);
  // Parse command line
  CommandLine cmd;
  cmd.Parse (argc, argv);
   //LogComponentEnable("AodvsRoutingProtocol",LOG_LEVEL_LOGIC);
 

  //LogComponentEnable ("AodvsRoutingProtocol", LOG_LEVEL_INFO);
  //LogComponentEnable ("OlsrsRoutingProtocol", LOG_LEVEL_LOGIC);
  //LogComponentEnable ("OlsrsHeader", LOG_LEVEL_INFO);
  //LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);

 

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::LeoCircularOrbitPostionAllocator",
                                 "NumOrbits", IntegerValue (numorbits),
                                 "NumSatellites", IntegerValue (numsatellites));
  mobility.SetMobilityModel ("ns3::LeoCircularOrbitMobilityModel",
  			     "Altitude", DoubleValue (1200),
  			     "Inclination", DoubleValue (90));
  NodeContainer Nodes[numorbits];
  for(uint32_t i = 0;i<numorbits;i++)
    {Nodes[i].Create(numsatellites);}
  for(uint32_t i = 0;i<numorbits;i++)
    {mobility.Install (Nodes[i]);}
  //安装进程间信道
  PointToPointHelper P2PLink[numorbits-1];
  for(uint32_t i = 0;i<numorbits-1;i++)
  {P2PLink[i].SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   P2PLink[i].SetChannelAttribute ("Delay", StringValue ("5ms"));
  }
  
  //安装进程内信道
  //CsmaHelper CsmaLink[numorbits];
  PointToPointHelper CsmaLink[numorbits];
  for(uint32_t i = 0;i<numorbits;i++)
  {//CsmaLink[i].SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
   //CsmaLink[i].SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
   CsmaLink[i].SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   CsmaLink[i].SetChannelAttribute ("Delay", StringValue ("5ms"));
  }
  

  NetDeviceContainer CsmaDevices[numorbits];
  NetDeviceContainer P2PDevices[2*(numorbits-1)];
   for(uint32_t u = 0; u < numorbits-1; ++u)
  {for(uint32_t i = 0; i < numsatellites; ++i)
    {NetDeviceContainer temp = P2PLink[u].Install (NodeContainer(Nodes[u].Get(i),Nodes[u+1].Get(i)));
    P2PDevices[2*u].Add (temp.Get (0));
     P2PDevices[2*u+1].Add (temp.Get (1));
     uint32_t v = (i!=numsatellites-1)?(i+1):0;
     NetDeviceContainer temp1 = P2PLink[u].Install (NodeContainer(Nodes[u].Get(i),Nodes[u+1].Get(v)));
    P2PDevices[2*u].Add (temp1.Get (0));
     P2PDevices[2*u+1].Add (temp1.Get (1));
    }
  }

  for(uint32_t u = 0; u < 2*(numorbits-1); ++u)
  {for(uint32_t i = 0; i < numsatellites; ++i)
    { 
     
        Ptr<PointToPointNetDevice> p2pdevclose = DynamicCast<PointToPointNetDevice> (P2PDevices[u].Get(2*i));
        p2pdevclose->inidevicestate(false);
        Ptr<PointToPointNetDevice> p2pdevopen = DynamicCast<PointToPointNetDevice> (P2PDevices[u].Get(2*i+1));
        p2pdevopen->inidevicestate(true);
    
    }
    /*for(uint32_t i = 0; i < numsatellites; ++i)
    { 
        Ptr<PointToPointNetDevice> p2pdevopen = DynamicCast<PointToPointNetDevice> (P2PDevices[2*u+1].Get(2*i));
        p2pdevopen->inidevicestate(true);
        Ptr<PointToPointNetDevice> p2pdevclose = DynamicCast<PointToPointNetDevice> (P2PDevices[2*u+1].Get(2*i+1));
        p2pdevclose->inidevicestate(false);
       
    }*/
  }
	
  //NetDeviceContainer rightRouterDevices;
  //NetDeviceContainer 1Devices;
  for(uint32_t u = 0; u < numorbits; ++u)
  {for (uint32_t i = 0; i < numsatellites; ++i)
    {
     if(i!=numsatellites-1)
        { NetDeviceContainer temp = CsmaLink[u].Install (NodeContainer (Nodes[u].Get (i), Nodes[u].Get (i+1)));
          CsmaDevices[u].Add (temp.Get (0));
          CsmaDevices[u].Add (temp.Get (1));
         }else
	    {  NetDeviceContainer temp = CsmaLink[u].Install (NodeContainer (Nodes[u].Get (i), Nodes[u].Get (0)));
               CsmaDevices[u].Add (temp.Get (0));
               CsmaDevices[u].Add (temp.Get (1));
             }
    }
  }



  //安装协议栈
  InternetStackHelper stack;
  AodvsHelper aodvs;
  aodvs.Set ("EnableHello", BooleanValue (false));
  aodvs.Set ("HelloInterval", TimeValue (Seconds (5)));

  //OlsrsHelper olsrs;
  /*for(uint32_t i = 0; i < 8; ++i)
    {olsrs.ExcludeInterface (Nodes0.Get (i), 1);
     olsrs.ExcludeInterface (Nodes1.Get (i), 1);
     olsrs.ExcludeInterface (Nodes2.Get (i), 1);
     olsrs.ExcludeInterface (Nodes3.Get (i), 1);
     olsrs.ExcludeInterface (Nodes4.Get (i), 1);
     olsrs.ExcludeInterface (Nodes5.Get (i), 1);
     olsrs.ExcludeInterface (Nodes0.Get (i), 2);
     olsrs.ExcludeInterface (Nodes1.Get (i), 2);
     olsrs.ExcludeInterface (Nodes2.Get (i), 2);
     olsrs.ExcludeInterface (Nodes3.Get (i), 2);
     olsrs.ExcludeInterface (Nodes4.Get (i), 2);
     olsrs.ExcludeInterface (Nodes5.Get (i), 2);
     olsrs.ExcludeInterface (Nodes0.Get (i), 3);
     olsrs.ExcludeInterface (Nodes1.Get (i), 3);
     olsrs.ExcludeInterface (Nodes2.Get (i), 3);
     olsrs.ExcludeInterface (Nodes3.Get (i), 3);
     olsrs.ExcludeInterface (Nodes4.Get (i), 3);
     olsrs.ExcludeInterface (Nodes5.Get (i), 3);
     olsrs.ExcludeInterface (Nodes1.Get (i), 4);
     olsrs.ExcludeInterface (Nodes2.Get (i), 4);
     olsrs.ExcludeInterface (Nodes3.Get (i), 4);
     olsrs.ExcludeInterface (Nodes4.Get (i), 4);
    }*/


  Ipv4StaticRoutingHelper staticRouting;
  Ipv4ListRoutingHelper list;
  //list.Add (staticRouting, 0);
  list.Add (aodvs, 20);
  //list.Add (olsrs, 10);
  
  stack.SetRoutingHelper (list);
  stack.InstallAll();



  Ipv4AddressHelper CsmaAddress;
  CsmaAddress.SetBase ("10.1.1.0", "255.255.255.0");
 
  Ipv4AddressHelper P2PAddress;
  P2PAddress.SetBase ("10.100.1.0", "255.255.255.0");
 


 
  // 配IP
  for(uint32_t u = 0; u < numorbits-1; ++u)
  {for (uint32_t i = 0; i < numsatellites; ++i)
    {
      NetDeviceContainer ndc;
      ndc.Add (P2PDevices[2*u].Get (i));
      ndc.Add (P2PDevices[2*u+1].Get (i));
       P2PAddress.Assign (ndc);
       P2PAddress.NewNetwork ();
    }
  
  }
  for(uint32_t u = 0; u < numorbits; ++u)
  {for (uint32_t i = 0; i < numsatellites; ++i)
    {
      NetDeviceContainer ndc;
      ndc.Add (CsmaDevices[u].Get (i*2));
      ndc.Add (CsmaDevices[u].Get (i*2+1));
      CsmaAddress.Assign (ndc);
      CsmaAddress.NewNetwork ();
    }
  }
 
/*
    if (systemId == 0)
      {
       //routerLink.EnablePcap("pcap/router-left-router", routerDevices, true);
       //leafLink.EnablePcap("pcap/leaf-left-leaf", leftLeafDevices, true);
       0Link.EnablePcap ("pcap/second", csmaDevices.Get (1), true);
       leftLeafLink.EnablePcapAll("pcap/left-nodes");
       }
      
    if (systemId == 1)
      {
       //routerLink.EnablePcap("pcap/router-right-router", routerDevices, true);
       //leafLink.EnablePcap("pcap/leaf-right-leaf", rightLeafDevices, true);
       routerLink.EnablePcapAll("pcap/right-router");
       rightLeafLink.EnablePcapAll("pcap/right-nodes");
       }
*/    
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
   /*for(int i=0;i<8;i++)
        {OlsrAndAodvStack(Nodes0.Get (i));
         OlsrAndAodvStack(Nodes1.Get (i));
         OlsrAndAodvStack(Nodes2.Get (i));
         OlsrAndAodvStack(Nodes3.Get (i));
         OlsrAndAodvStack(Nodes4.Get (i));
         OlsrAndAodvStack(Nodes5.Get (i));
        }
  */
  
  

      Ptr<Socket> recvSink0 = Socket::CreateSocket (Nodes[0].Get (0), tid);
      InetSocketAddress local0 = InetSocketAddress (Ipv4Address::GetAny (), 80);
      recvSink0->Bind (local0);
      recvSink0->SetRecvCallback (MakeCallback (&ReceivePacket));
      /*Ptr<Socket> recvSink1 = Socket::CreateSocket (Nodes[0].Get (1), tid);
      InetSocketAddress local1 = InetSocketAddress (Ipv4Address::GetAny (), 80);
      recvSink1->Bind (local1);
      recvSink1->SetRecvCallback (MakeCallback (&ReceivePacket));
      Ptr<Socket> recvSink2 = Socket::CreateSocket (Nodes[0].Get (2), tid);
      InetSocketAddress local2 = InetSocketAddress (Ipv4Address::GetAny (), 80);
      recvSink2->Bind (local2);
      recvSink2->SetRecvCallback (MakeCallback (&ReceivePacket));
      Ptr<Socket> recvSink3 = Socket::CreateSocket (Nodes[0].Get (3), tid);
      InetSocketAddress local3 = InetSocketAddress (Ipv4Address::GetAny (), 80);
      recvSink3->Bind (local3);
      recvSink3->SetRecvCallback (MakeCallback (&ReceivePacket));
      Ptr<Socket> recvSink4 = Socket::CreateSocket (Nodes[0].Get (4), tid);
      InetSocketAddress local4 = InetSocketAddress (Ipv4Address::GetAny (), 80);
      recvSink4->Bind (local4);
      recvSink4->SetRecvCallback (MakeCallback (&ReceivePacket));
      Ptr<Socket> recvSink5 = Socket::CreateSocket (Nodes[0].Get (5), tid);
      InetSocketAddress local5 = InetSocketAddress (Ipv4Address::GetAny (), 80);
      recvSink5->Bind (local5);
      recvSink5->SetRecvCallback (MakeCallback (&ReceivePacket));*/


      Ptr<OutputStreamWrapper> routingStream2 = Create<OutputStreamWrapper> ("pcap/compare-aodvs350.routes", std::ios::out);
      list.PrintRoutingTableAllAt (Seconds (350), routingStream2);
  
       
      Ptr<Socket> source0 = Socket::CreateSocket (Nodes[numorbits-1].Get (sendnode), tid);
      InetSocketAddress remote0 = InetSocketAddress (Ipv4Address ("10.1.1.1"), 80);
      source0->Connect (remote0);
      Simulator::ScheduleWithContext (source0->GetNode ()->GetId (),
                                  Seconds (10), &GenerateTraffic,
                                  source0, packetSize, numPackets, interPacketInterval);  
      /*Ptr<Socket> source1 = Socket::CreateSocket (Nodes[numorbits-1].Get (1), tid);
      InetSocketAddress remote1 = InetSocketAddress (Ipv4Address ("10.1.2.1"), 80);
      source1->Connect (remote1);
      Simulator::ScheduleWithContext (source1->GetNode ()->GetId (),
                                  Seconds (10), &GenerateTraffic,
                                  source1, packetSize, numPackets, interPacketInterval);  
      Ptr<Socket> source2 = Socket::CreateSocket (Nodes[numorbits-1].Get (2), tid);
      InetSocketAddress remote2 = InetSocketAddress (Ipv4Address ("10.1.3.1"), 80);
      source2->Connect (remote2);
      Simulator::ScheduleWithContext (source2->GetNode ()->GetId (),
                                  Seconds (10), &GenerateTraffic,
                                  source2, packetSize, numPackets, interPacketInterval); 
      Ptr<Socket> source3 = Socket::CreateSocket (Nodes[numorbits-1].Get (3), tid);
      InetSocketAddress remote3 = InetSocketAddress (Ipv4Address ("10.1.4.1"), 80);
      source3->Connect (remote3);
      Simulator::ScheduleWithContext (source3->GetNode ()->GetId (),
                                  Seconds (10), &GenerateTraffic,
                                  source3, packetSize, numPackets, interPacketInterval); 
      Ptr<Socket> source4 = Socket::CreateSocket (Nodes[numorbits-1].Get (4), tid);
      InetSocketAddress remote4 = InetSocketAddress (Ipv4Address ("10.1.5.1"), 80);
      source4->Connect (remote4);
      Simulator::ScheduleWithContext (source4->GetNode ()->GetId (),
                                  Seconds (10), &GenerateTraffic,
                                  source4, packetSize, numPackets, interPacketInterval); 
      Ptr<Socket> source5 = Socket::CreateSocket (Nodes[numorbits-1].Get (5), tid);
      InetSocketAddress remote5 = InetSocketAddress (Ipv4Address ("10.1.6.1"), 80);
      source5->Connect (remote5);
      Simulator::ScheduleWithContext (source5->GetNode ()->GetId (),
                                  Seconds (10), &GenerateTraffic,
                                  source5, packetSize, numPackets, interPacketInterval); */
  
      Ptr<OutputStreamWrapper> routingStream1 = Create<OutputStreamWrapper> ("pcap/compare-aodvs400.routes", std::ios::out);
      list.PrintRoutingTableAllAt (Seconds (400), routingStream1);
      Ptr<OutputStreamWrapper> routingStream3 = Create<OutputStreamWrapper> ("pcap/compare-0-aodvs22.routes", std::ios::out);
      list.PrintRoutingTableAllAt (Seconds (22), routingStream3);

//CsmaLink[0].EnablePcap("pcap/CsmaLink", CsmaDevices[0], true);
//P2PLink[0].EnablePcap ("pcap/P2PLink", P2PDevices[0], true);
  for(uint32_t u = 0; u < numorbits; ++u)   
    {  
      //CsmaLink[u].EnablePcapAll ("pcap/CsmaLink");
      CsmaLink[u].EnablePcap("pcap/CsmaLink", CsmaDevices[u], true);
    };
  for(uint32_t u = 0; u < numorbits-1; ++u)   
    {
      //P2PLink[u].EnablePcapAll ("pcap/P2PLink");
      P2PLink[u].EnablePcap("pcap/P2PLink", P2PDevices[2*u], true);
      P2PLink[u].EnablePcap("pcap/P2PLink", P2PDevices[2*u+1], true);
    } 
     
    
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",MakeCallback (&CourseChange)); 
   //std::streambuf *coutbuf = std::cout.rdbuf();
  // redirect cout if traceFile is specified
  //std::ofstream out;
  //out.open (traceFile);
  /*if (out.is_open ())
    {
      std::cout.rdbuf(out.rdbuf());
    }*/
  Simulator::Stop (Seconds (duration));
  Simulator::Run ();
  Simulator::Destroy ();
  //out.close ();
  //std::cout.rdbuf(coutbuf);
  uint32_t delta=TimeMillis()-last;
    std::cout<<"run_time:"<<delta<<std::endl;

}
