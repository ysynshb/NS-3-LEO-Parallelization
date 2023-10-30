

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mpi-interface.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/olsr-routing-protocol.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/csma-module.h"
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
{ 
  pktrecv++;
  
  
}

static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,
                             uint32_t pktCount, Time pktInterval )
{ 
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


void CourseChange (std::string context, Ptr<const MobilityModel> position)
{
  Vector pos = position->GetPosition ();
  Ptr<const Node> node = position->GetObject<Node> ();
  
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
       {
        Ptr<PointToPointNetDevice> p2pdev = DynamicCast<PointToPointNetDevice> (dev);
        Ptr<PointToPointChannel> p2pchannel = DynamicCast<PointToPointChannel> (channel);
         Ptr<PointToPointNetDevice> p2pdevice1 = p2pchannel->GetPointToPointDevice(0);
        Ptr<PointToPointNetDevice> p2pdevice2 = p2pchannel->GetPointToPointDevice(1);
      
        uint16_t lastp2pdevstate = p2pdev->getdevicestate();
	if(pos.z>7114500 || pos.z<-7114500)
          {     
                if(lastp2pdevstate ==1 || lastp2pdevstate ==3)
                  {p2pdev->setdevicestate(true);
   		p2pchannel->setchannelstate(false);
                std::cout<< "节点"<<node->GetId ()<<"的"<<i<<"端口因高纬度关闭，所在信道关闭"<<std::endl;}
                
		
	  } else 
	    {   
                if((p2pdevice1->getdevicestate()==3) || (p2pdevice2->getdevicestate()==3))
                {
                  //NS_LOG_UNCOND ("节点 "<<node->GetId ()<<"的"<<i<<"端口所在的信道关闭");
                  p2pchannel->setchannelstate(false);
                }
                if(lastp2pdevstate ==2 || lastp2pdevstate ==4)
                 {p2pdev->setdevicestate(true);}
                if((p2pdevice1->getdevicestate()==1) && (p2pdevice2->getdevicestate()==1))
                  {  p2pchannel->setchannelstate(true);
                     NS_LOG_UNCOND ("节点 "<<node->GetId ()<<"的"<<i<<" 端口恢复正常@@@@@@@@@@@");
		    }
               
                
	     }
	}
    }
  
}

int
main (int argc, char *argv[])
{

  uint32_t numorbits = 48;
  uint32_t numsatellites = 64; 
  uint32_t duration =500;
  //double interval = 0.01; // seconds
  //uint32_t sendnode = numsatellites/4;
  uint32_t last=TimeMillis();
  //uint32_t packetSize = 1000; // bytes
  //uint32_t numPackets = 100000000;
  //std::string traceFile = "pcap/leocompare.log";
  //Time interPacketInterval = Seconds (interval);
  // Parse command line
 
   CommandLine cmd;
  cmd.AddValue("numorbits", "numbers of orbits", numorbits);
  cmd.AddValue("numsatellites", "numbers of satellites in an orbit", numsatellites);
  cmd.AddValue("duration", "duration of simulation", duration);
  cmd.AddValue("interval", "interval of sending two packets", interval);
  cmd.AddValue("nummpi", "numbers of LP", nummpi);
  cmd.AddValue("traceFile", "output the wall clock time into file", traceFile);
  cmd.Parse (argc, argv);

 

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
  {
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
   
  }
	
 
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



  
  InternetStackHelper stack;
  AodvHelper aodv;
  aodv.Set ("EnableHello", BooleanValue (false));
  aodv.Set ("HelloInterval", TimeValue (Seconds (5)));



  Ipv4StaticRoutingHelper staticRouting;
  Ipv4ListRoutingHelper list;
  //list.Add (staticRouting, 0);
  list.Add (aodv, 20);
  //list.Add (olsrs, 10);
  
  stack.SetRoutingHelper (list);
  stack.InstallAll();



  Ipv4AddressHelper CsmaAddress;
  CsmaAddress.SetBase ("10.1.1.0", "255.255.255.0");
 
  Ipv4AddressHelper P2PAddress;
  P2PAddress.SetBase ("10.100.1.0", "255.255.255.0");
 


 
  
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
 

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
 


      Ptr<OutputStreamWrapper> routingStream2 = Create<OutputStreamWrapper> ("pcap/compare-aodvs350.routes", std::ios::out);
      list.PrintRoutingTableAllAt (Seconds (350), routingStream2);
  
       
  
  
      Ptr<OutputStreamWrapper> routingStream1 = Create<OutputStreamWrapper> ("pcap/compare-aodvs400.routes", std::ios::out);
      list.PrintRoutingTableAllAt (Seconds (400), routingStream1);
      Ptr<OutputStreamWrapper> routingStream3 = Create<OutputStreamWrapper> ("pcap/compare-0-aodvs22.routes", std::ios::out);
      list.PrintRoutingTableAllAt (Seconds (22), routingStream3);


     
    
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",MakeCallback (&CourseChange)); 
  
  Simulator::Stop (Seconds (duration));
  Simulator::Run ();
  Simulator::Destroy ();
 
  uint32_t delta=TimeMillis()-last;
    std::cout<<"run_time:"<<delta<<std::endl;

}
