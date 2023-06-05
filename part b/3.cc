
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"


// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0


using namespace ns3;
int 
main (int argc, char *argv[])
{
 
  uint32_t nCsma = 3;//number of csma nodes sett to 3


  NodeContainer p2pNodes; //peer to peer nodes
  p2pNodes.Create (2); 



  NodeContainer csmaNodes;//iga ncsma nodes use madi csmaNodes 
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);



// point to point connection madi adhk devices ig kodud
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);


// csma connection na build madi adhk devices ig kodud
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);



  InternetStackHelper stack;
  stack.Install (p2pNodes.Get (0));//internet protocol stack al point to point 0 node matra install aath 
  stack.Install (csmaNodes);//but csma nodes yella install aath



  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");

  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);


  

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (p2pNodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));


//idh use madi nav routing tables na populate madath
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

//idhrinda nam yella point to point protocal capture(pcap) madi store madath
  pointToPoint.EnablePcapAll ("second");//second is just filename
  csma.EnablePcap ("second", csmaDevices.Get (1), true);
  //get 1 andre index 1 al idad csma devices na taknd capture mad
  

AnimationInterface anim ("third.xml");
Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
