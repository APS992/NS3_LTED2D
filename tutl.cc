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

#include <iostream>
#include "ns3/lte-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/internet-trace-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include "ns3/spectrum-module.h"
#include "ns3/buildings-module.h"
#include "ns3/flow-monitor-module.h"
#include "sys/timeb.h"
#include "ns3/log.h"
#include "ns3/string.h"
#include <ns3/log.h>
#include <fstream>
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LenaFrequencyReuse");

void
PrintGnuplottableUeListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
    {
      Ptr<Node> node = *it;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; j++)
        {
          Ptr<LteUeNetDevice> uedev = node->GetDevice (j)->GetObject <LteUeNetDevice> ();
          if (uedev)
            {
               Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
               outFile << "set label \"" << uedev->GetImsi ()
                      << "\" at " << pos.x << "," << pos.y << " left font \"Helvetica,4\" textcolor rgb \"grey\" front point pt 1 ps 0.3 lc rgb \"grey\" offset 0,0"
                      << std::endl;
            }
        }
    }
}

void
PrintGnuplottableEnbListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
    {
      Ptr<Node> node = *it;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; j++)
        {
          Ptr<LteEnbNetDevice> enbdev = node->GetDevice (j)->GetObject <LteEnbNetDevice> ();
          if (enbdev)
            {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "set label \"" << enbdev->GetCellId ()
                      << "\" at " << pos.x << "," << pos.y
                      << " left font \"Helvetica,4\" textcolor rgb \"white\" front  point pt 2 ps 0.3 lc rgb \"white\" offset 0,0"
                      << std::endl;
            }
        }
    }
}

AnimationInterface *pAnim=0;

struct rgb {
uint8_t r ;
uint8_t g ;
uint8_t b ;
};

struct rgb colors [] ={
{255, 0, 0},
{0, 255, 0},
{0, 0, 255}
};

uint32_t resourceId1;
uint32_t resourceId2;
uint32_t resourceId3;
uint32_t resourceId4;
uint32_t resourceId5;
uint32_t resourceId6;

void modify ()
{
std::ostringstream oss;
oss << "Update:" << Simulator::Now ().GetSeconds ();
pAnim->UpdateLinkDescription (0, 1, oss.str ());
pAnim->UpdateLinkDescription (0, 2, oss.str ());
pAnim->UpdateLinkDescription (0, 3, oss.str ());
pAnim->UpdateLinkDescription (0, 4, oss.str ());
pAnim->UpdateLinkDescription (0, 5, oss.str ());
pAnim->UpdateLinkDescription (0, 6, oss.str ());
pAnim->UpdateLinkDescription (1, 7, oss.str ());
pAnim->UpdateLinkDescription (1, 8, oss.str ());
pAnim->UpdateLinkDescription (1, 9, oss.str ());
pAnim->UpdateLinkDescription (1, 10, oss.str ());
pAnim->UpdateLinkDescription (1, 11, oss.str ());

std::ostringstream node0Oss;
node0Oss << "-----Node:" << Simulator::Now ().GetSeconds();
pAnim->UpdateNodeDescription (2, node0Oss.str ());
static uint32_t currentResourceId =resourceId1;
static uint32_t currentResourceId2 =resourceId2;
static uint32_t currentResourceId3 =resourceId3;
static uint32_t currentResourceId4 =resourceId4;
static uint32_t currentResourceId5 =resourceId5;
static uint32_t currentResourceId6 =resourceId6;

pAnim->UpdateNodeSize(1, 150, 70);
pAnim->UpdateNodeImage(1, currentResourceId4);
pAnim->UpdateNodeSize(0, 150, 250);
pAnim->UpdateNodeImage(0, currentResourceId3);

for(uint16_t i=2; i<5; i++){
pAnim->UpdateNodeSize(i, 70, 70);
pAnim->UpdateNodeImage(i, currentResourceId);
}
for(uint16_t i=5; i<7; i++){
pAnim->UpdateNodeSize(i, 50, 30);
pAnim->UpdateNodeImage(i, currentResourceId2);
}
for(uint16_t i=7; i<10; i++){
pAnim->UpdateNodeSize(i, 70, 30);
pAnim->UpdateNodeImage(i, currentResourceId6);
}
for(uint16_t i=10; i<15; i++){
pAnim->UpdateNodeSize(i, 50, 30);
pAnim->UpdateNodeImage(i, currentResourceId5);
}

if(Simulator::Now ().GetSeconds ()<10)
Simulator::Schedule (Seconds(0.05), modify);
}

int main(int argc, char *argv[])
{
Config::SetDefault("ns3::LteSpectrumPhy::CtrlErrorModelEnabled", BooleanValue(true));
Config::SetDefault("ns3::LteSpectrumPhy::DataErrorModelEnabled", BooleanValue(true));
Config::SetDefault("ns3::LteHelper::UseIdealRrc", BooleanValue(true));
Config::SetDefault("ns3::LteHelper::UsePdschForCqiGeneration", BooleanValue(true));

Config::SetDefault("ns3::LteUePhy::EnableUplinkPowerControl", BooleanValue(true));
Config::SetDefault("ns3::LteUePowerControl::ClosedLoop", BooleanValue(true));
Config::SetDefault("ns3::LteUePowerControl::AccumulationEnabled", BooleanValue(true));

Time::SetResolution (Time::NS);
LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

uint16_t numberOfNodesENB = 3;
uint16_t numberOfNodesEU = 10;
double simTime = 0.05;
double distance = 250.0;
double interPacketInterval = 150.0;

std::string animFile = "lte-4g.xml";

CommandLine cmd;
cmd.AddValue ("numberOfNodes", "Number of eNodeBS + UE pairs", numberOfNodesENB);
cmd.AddValue ("simTime", "Total duration of the simulation (in seconds)", simTime);
cmd.AddValue ("distance", "Distance between eNBs[m]", distance);
cmd.AddValue ("interPacketInterval", "Inter Packet Interval[ms]", interPacketInterval);
cmd.AddValue ("animFile", "File Name of Animation Output", animFile);
cmd.Parse (argc, argv);
ConfigStore inputConfig;
inputConfig.ConfigureDefaults();
cmd.Parse (argc, argv);

Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
lteHelper->SetAttribute ("FadingModel", StringValue("ns3::TraceFadingLossModel"));

std::ifstream ifTraceFile;
ifTraceFile.open("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad", std::ifstream::in);
if(ifTraceFile.good()){
lteHelper->SetFadingModelAttribute("TraceFilename", StringValue("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
}else
{
lteHelper->SetFadingModelAttribute("TraceFilename", StringValue("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
}
lteHelper->SetFadingModelAttribute ("TraceLength", TimeValue (Seconds(10.0)));
lteHelper->SetFadingModelAttribute ("SamplesNum", UintegerValue (10000));
lteHelper->SetFadingModelAttribute ("WindowSize", TimeValue (Seconds(0.5)));
lteHelper->SetFadingModelAttribute ("RbNum", UintegerValue (100));

lteHelper->SetEnbDeviceAttribute ("DlEarfcn", UintegerValue (100));
lteHelper->SetEnbDeviceAttribute ("UlEarfcn", UintegerValue (18100));

Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
lteHelper->SetEpcHelper (epcHelper);

Ptr<Node> pgw = epcHelper->GetPgwNode ();
NodeContainer remoteHostContainer;
remoteHostContainer.Create(1);

Ptr<Node> remoteHost = remoteHostContainer.Get(0);
InternetStackHelper internet;
internet.Install (remoteHostContainer);

Ptr<ListPositionAllocator> PositionAlloc1 = CreateObject<ListPositionAllocator> ();
  PositionAlloc1->Add (Vector (500.0, -100.0, 20.0));                       
  MobilityHelper mobility1;
  mobility1.SetPositionAllocator (PositionAlloc1);
  mobility1.Install (remoteHostContainer);

PointToPointHelper p2ph;
p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate("150Mb/s")));
p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds(0.010)));

NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
Ipv4AddressHelper ipv4h;
ipv4h.SetBase("1.0.0.0", "255.0.0.0");
Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);

Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);
Ipv4StaticRoutingHelper ipv4RoutingHelper;
Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4> ());
remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

NodeContainer enbNodes;
enbNodes.Create(numberOfNodesENB);
NodeContainer ueNodes;
ueNodes.Create(numberOfNodesEU);

double x_min = 0.0;
double x_max = 10.0;
double y_min = 0.0;
double y_max = 20.0;
double z_min = 0.0;
double z_max = 10.0;

Ptr<Building> b = CreateObject <Building> ();
b->SetBoundaries (Box (x_min, x_max, y_min, y_max, z_min, z_max));
b->SetBuildingType (Building::Residential);
b->SetExtWallsType (Building::ConcreteWithWindows);
b->SetNFloors (3);
b->SetNRoomsX (3);
b->SetNRoomsY (2);

Ptr <GridBuildingAllocator> gridBuildingAllocator;
gridBuildingAllocator = CreateObject <GridBuildingAllocator> ();
gridBuildingAllocator->SetAttribute ("GridWidth", UintegerValue (3));
gridBuildingAllocator->SetAttribute ("LengthX", DoubleValue (7));
gridBuildingAllocator->SetAttribute ("LengthY", DoubleValue (13));
gridBuildingAllocator->SetAttribute ("DeltaX", DoubleValue (3));
gridBuildingAllocator->SetAttribute ("DeltaY", DoubleValue (3));
gridBuildingAllocator->SetAttribute ("Height", DoubleValue (6));
gridBuildingAllocator->SetBuildingAttribute ("NRoomsX", UintegerValue (2));
gridBuildingAllocator->SetBuildingAttribute ("NRoomsY", UintegerValue (4));
gridBuildingAllocator->SetBuildingAttribute ("NFloors", UintegerValue (2));
gridBuildingAllocator->SetAttribute ("MinX", DoubleValue (0));
gridBuildingAllocator->SetAttribute ("MinY", DoubleValue (0));
gridBuildingAllocator->Create (6);

MobilityHelper mobility;
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.SetPositionAllocator ("ns3::GridPositionAllocator","MinX", DoubleValue (-350), "MinY", DoubleValue (-200), "DeltaX", DoubleValue (0.0), "DeltaY", DoubleValue (150.0), "GridWidth", UintegerValue (1), "LayoutType", StringValue ("RowFirst"));

mobility.Install (enbNodes);
BuildingsHelper::Install (enbNodes);
for(uint16_t i=0; i<numberOfNodesEU; i++){
mobility.SetPositionAllocator ("ns3::GridPositionAllocator","MinX", DoubleValue (-900), "MinY", DoubleValue (-250), "DeltaX", DoubleValue (120.0), "DeltaY", DoubleValue (150.0), "GridWidth", UintegerValue (4), "LayoutType", StringValue ("RowFirst"));

mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Mode", StringValue ("Time"), "Time", StringValue ("0.5s"), "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=50.0]"), "Bounds", RectangleValue (Rectangle(-12000.0, 12000.0, -12000.0, 12000.0)));
}

mobility.Install (ueNodes);
AsciiTraceHelper ascii;
MobilityHelper::EnableAsciiAll (ascii.CreateFileStream ("mobility-trace-example.mob"));

NetDeviceContainer enbDevs;
enbDevs = lteHelper->InstallEnbDevice (enbNodes);
NetDeviceContainer ueDevs;
ueDevs = lteHelper->InstallUeDevice (ueNodes);

internet.Install (ueNodes);
Ipv4InterfaceContainer ueIpIface;
ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));
for(uint32_t u=0; u<ueNodes.GetN (); ++u){
Ptr<Node> ueNode = ueNodes.Get (u);
Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(ueNode->GetObject<Ipv4> ());
ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
}
uint16_t j=0;
for(uint16_t i=0; i<numberOfNodesEU; i++){
if(j < numberOfNodesENB){
lteHelper->Attach (ueDevs.Get(i), enbDevs.Get(j));
j++;
}
else{
j=0;
lteHelper->Attach (ueDevs.Get(i), enbDevs.Get(j));
}
}

Ptr<EpcTft> tft = Create <EpcTft> ();
EpcTft::PacketFilter pf ; 
pf.localPortStart = 1234;
pf.localPortEnd = 1234;
tft->Add(pf);
lteHelper->ActivateDedicatedEpsBearer (ueDevs, EpsBearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT), tft);

uint16_t dlPort = 1234;
uint16_t ulPort = 2000;
uint16_t otherPort = 3000;
ApplicationContainer clientApps;
ApplicationContainer serverApps;
for(uint32_t u=0; u<ueNodes.GetN (); ++u){
++ulPort;
++otherPort;
PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
PacketSinkHelper PacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get (u))); 
serverApps.Add (ulPacketSinkHelper.Install (remoteHost)); 
serverApps.Add (PacketSinkHelper.Install (ueNodes.Get (u))); 

UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
dlClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
 
UdpClientHelper ulClient (remoteHostAddr, ulPort);
ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));

UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
client.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
client.SetAttribute ("MaxPackets", UintegerValue(1000000));

clientApps.Add (dlClient.Install (remoteHost));
clientApps.Add (ulClient.Install (ueNodes.Get(u)));
if (u+1 < ueNodes.GetN ())
     {
      clientApps.Add (client.Install (ueNodes.Get(u+1)));
 }
   else
   {
clientApps.Add (client.Install (ueNodes.Get(0)));
 }
}

serverApps.Start (Seconds (0.01));
clientApps.Start (Seconds (0.01));
//lteHelper->EnableTraces ();

Simulator::Stop (Seconds (simTime));

Config::SetDefault ("ns3::LteAmc::AmcModel", EnumValue (LteAmc::PiroEW2010));
Config::SetDefault ("ns3::LteAmc::AmcModel", EnumValue (LteAmc::MiErrorModel));
Config::SetDefault ("ns3::LteAmc::Ber", DoubleValue(0.00005));

lteHelper->EnablePhyTraces();
lteHelper->EnableMacTraces();
lteHelper->EnableRlcTraces();

pAnim = new AnimationInterface (animFile.c_str ());
resourceId2 = pAnim->AddResource ("/home/usrp-1/ns3/ns-allinone-3.25/ns-3.25/scratch/Police-car.png");
resourceId1 = pAnim->AddResource ("/home/usrp-1/ns3/ns-allinone-3.25/ns-3.25/scratch/eNb.png");
resourceId3 = pAnim->AddResource ("/home/usrp-1/ns3/ns-allinone-3.25/ns-3.25/scratch/Serverepc.png");
resourceId4 = pAnim->AddResource ("/home/usrp-1/ns3/ns-allinone-3.25/ns-3.25/scratch/RemoteHost.png");
resourceId5 = pAnim->AddResource ("/home/usrp-1/ns3/ns-allinone-3.25/ns-3.25/scratch/Phone.png");
resourceId6 = pAnim->AddResource ("/home/usrp-1/ns3/ns-allinone-3.25/ns-3.25/scratch/car.png");
pAnim->SetBackgroundImage ("/home/usrp-1/ns3/ns-allinone-3.25/ns-3.25/scratch/4g-lte.jpg", -1, 1, 0.5, 0.5, 1);

Simulator::Schedule (Seconds (simTime), modify);

Simulator::Run ();
Simulator::Destroy ();
delete pAnim;
return 0;
}
