/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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
// Number of wifi or csma nodes can be increased up to 250
//                          |
//                 Rank 0   |   Rank 1
// -------------------------|----------------------------
//   LeftWifi 10.1.3.0
//               AP1				AP2
//  *    *    *    *				 *   *    *    *
//  |    |    |    |    10.1.1.0     |   |    |    |
// n5   n6   n7   n0                n1   n2   n3   n4
//                   point-to-point
//										RightWifi 10.1.2.0
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

int
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nRightWifi = 2;
  uint32_t nLeftWifi = 2;
  bool tracing = false;

  CommandLine cmd;
  cmd.AddValue ("nRightWifi", "Number of \"extra\" CSMA nodes/devices", nRightWifi);
  cmd.AddValue ("nLeftWifi", "Number of wifi STA devices", nLeftWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  // Check for valid number of csma or wifi nodes
  // 250 should be enough, otherwise IP addresses
  // soon become an issue
 /* if (nWifi > 250 || nCsma > 250)
    {
      std::cout << "Too many wifi or csma nodes, no more than 250 each." << std::endl;
      return 1;
    }
*/
  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);


//These containers belong to right Wifi Nodes
  NodeContainer rightWifiNodes,ap2WifiNodes;
  rightWifiNodes.Create (nRightWifi);
  ap2WifiNodes.Add(p2pNodes.Get(1));
  //These containers belong to left Wifi Nodes
  NodeContainer leftWifiNodes,ap1WifiNodes;
  leftWifiNodes.Create (nLeftWifi);
  ap1WifiNodes = p2pNodes.Get (0);


  NetDeviceContainer rightWifiDevices,ap2WifiDevices;
  NetDeviceContainer leftWifiDevices,ap1WifiDevices;

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();
  Ssid ssid;
  ssid=Ssid ("Right-Wifi");
      mac.SetType ("ns3::StaWifiMac",
                   "Ssid", SsidValue (ssid),
                   "ActiveProbing", BooleanValue (false));

  rightWifiDevices=wifi.Install(phy,mac,rightWifiNodes);
  // Reconfigured MAC for AP then install on ap2WifiNodes
  mac.SetType ("ns3::ApWifiMac",
                     "Ssid", SsidValue (ssid));
  ap2WifiDevices=wifi.Install(phy,mac,ap2WifiNodes);

  ssid=Ssid ("Left-Wifi");
  mac.SetType ("ns3::StaWifiMac",
                     "Ssid", SsidValue (ssid),
                     "ActiveProbing", BooleanValue (false));
  leftWifiDevices=wifi.Install(phy,mac,leftWifiNodes);

  mac.SetType ("ns3::ApWifiMac",
                       "Ssid", SsidValue (ssid));
  ap1WifiDevices=wifi.Install(phy,mac,ap1WifiNodes);


  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (leftWifiNodes);
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                   "MinX", DoubleValue (5.0),
                                   "MinY", DoubleValue (5.0),
                                   "DeltaX", DoubleValue (5.0),
                                   "DeltaY", DoubleValue (10.0),
                                   "GridWidth", UintegerValue (3),
                                   "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install(rightWifiNodes);
  // Start From Here
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (p2pNodes);

 

  InternetStackHelper stack;
  stack.Install(p2pNodes);
  stack.Install (rightWifiNodes);
  stack.Install (leftWifiNodes);


  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  address.Assign (p2pDevices);


  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer rightInterfaces;
  rightInterfaces=address.Assign(rightWifiDevices);
  address.Assign(ap2WifiDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign(leftWifiDevices);
  address.Assign(ap1WifiDevices);

  Ipv4GlobalRoutingHelper g;
    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("dynamic-global-routing.routes", std::ios::out);
    g.PrintRoutingTableAllAt (Seconds (1), routingStream);
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (rightWifiNodes.Get (1));
  serverApps.Start (Seconds (4.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (rightInterfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (5));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps=echoClient.Install (leftWifiNodes.Get (1));
  clientApps.Start (Seconds (5.0));
  clientApps.Stop (Seconds (10.0));

//Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  AnimationInterface anim("wireless-ap.xml");
  anim.EnablePacketMetadata(true);
  Simulator::Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
