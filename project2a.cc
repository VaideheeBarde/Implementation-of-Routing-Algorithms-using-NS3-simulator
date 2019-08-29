/*
Template for Project 2 

Based on ns3 example /src/dsdv/examples/dsdv-manet.cc

Prepared by: Pedro Santacruz

*/

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/dsdv-helper.h"
#include "ns3/aodv-helper.h"
#include "ns3/dsr-helper.h"
#include "ns3/dsr-main-helper.h"
#include "ns3/netanim-module.h"
#include <iostream>
#include <cmath>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ProjectTwoTemplate");

class ProjectTwoSimulation
{
public:
  ProjectTwoSimulation ();
  void RunSim (uint32_t nWifis,
                uint32_t nSinks,
                double totalTime,
                std::string rate,
                std::string phyMode,
                uint32_t nodeSpeed,
		uint32_t nodePause,
                uint32_t periodicUpdateInterval,
                uint32_t settlingTime,
                double dataStart,
                bool printRoutes,
                std::string CSVfileName);

private:
  uint32_t m_nWifis;
  uint32_t m_nSinks;
  double m_totalTime;
  std::string m_rate;
  std::string m_phyMode;
  uint32_t m_nodeSpeed;
  uint32_t m_nodePause;
  uint32_t m_periodicUpdateInterval;
  uint32_t m_settlingTime;
  double m_dataStart;
  uint32_t bytesTotal;
  uint32_t packetsReceived;
  bool m_printRoutes;
  std::string m_CSVfileName;

  NodeContainer nodes;
  NetDeviceContainer devices;
  Ipv4InterfaceContainer interfaces;

private:
  void CreateNodes ();
  void CreateDevices (std::string tr_name);
  void InstallInternetStack (std::string tr_name);
  void InstallApplications ();
  void SetupMobility ();
  void ReceivePacket (Ptr <Socket> );
  Ptr <Socket> SetupPacketReceive (Ipv4Address, Ptr <Node> );
  void CheckThroughput ();

};

uint16_t port = 9;

int main (int argc, char **argv)
{

	//Setup Simulation Parameters
	ProjectTwoSimulation project;
	uint32_t nWifis = 25;
  	uint32_t nSinks = 5;
  	double totalTime = 50.0;
  	std::string rate ("8kbps");
  	std::string phyMode ("DsssRate11Mbps");
  	uint32_t nodeSpeed = 50; // in m/s
        uint32_t nodePause = 5;
  	std::string appl = "all";
  	uint32_t periodicUpdateInterval = 10;
  	uint32_t settlingTime = 6;
  	double dataStart = 5.0;
  	bool printRoutingTable = true;
  	std::string CSVfileName = "Project2Results.csv";

	SeedManager::SetSeed (12345);

	Config::SetDefault ("ns3::OnOffApplication::PacketSize", StringValue ("1000"));
  	Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue (rate));
  	Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));
  	Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2000"));

	//Control parameters from command line
	CommandLine cmd;
	cmd.AddValue ("nWifis", "Number of wifi nodes[Default:25]", nWifis);
	cmd.AddValue ("nSinks", "Number of wifi sink nodes[Default:5]", nSinks);
	cmd.AddValue ("totalTime", "Total Simulation time[Default:50]", totalTime);
	cmd.AddValue ("phyMode", "Wifi Phy mode[Default:DsssRate11Mbps]", phyMode);
	cmd.AddValue ("rate", "CBR traffic rate[Default:8kbps]", rate);
	cmd.AddValue ("nodeSpeed", "Node speed in RandomWayPoint model[Default:50]", nodeSpeed);
	cmd.AddValue ("nodePause", "Node pause in RandomWayPoint model[Default:5]", nodePause);
	cmd.AddValue ("periodicUpdateInterval", "Periodic Interval Time[Default=10]", periodicUpdateInterval);
	cmd.AddValue ("settlingTime", "Settling Time before sending out an update for changed metric[Default=6]", settlingTime);
	cmd.AddValue ("dataStart", "Time at which nodes start to transmit data[Default=50.0]", dataStart);
	cmd.AddValue ("printRoutingTable", "print routing table for nodes[Default:1]", printRoutingTable);
	cmd.AddValue ("CSVfileName", "The name of the CSV output file name[Default:DsdvManetExample.csv]", CSVfileName);
	cmd.Parse (argc, argv);

	//Print first line of trace file	
	std::ofstream out (CSVfileName.c_str ());
  	out << "SimulationSecond," <<  "ReceiveRate," <<  "PacketsReceived," <<  "NumberOfSinks," <<  std::endl;
  	out.close ();

	//Run the simulation	
	project = ProjectTwoSimulation ();
	
	project.RunSim (nWifis, nSinks, totalTime, rate, phyMode, nodeSpeed, nodePause, periodicUpdateInterval,
                settlingTime, dataStart, printRoutingTable, CSVfileName);

	return 0;
}

//Constructor with initialized values
ProjectTwoSimulation::ProjectTwoSimulation () : bytesTotal (0), packetsReceived (0)
	{
	}

void ProjectTwoSimulation::ReceivePacket (Ptr <Socket> socket)
{
  	//NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << " Received one packet!");
 	Ptr <Packet> packet;
  	while ((packet = socket->Recv ()))
    		{
      		bytesTotal += packet->GetSize ();
      		packetsReceived += 1;
    		}
}

void ProjectTwoSimulation::CheckThroughput ()
{
  	double kbs = (bytesTotal * 8.0) / 1000;

  	std::ofstream out (m_CSVfileName.c_str (), std::ios::app);
  	out << (Simulator::Now ()).GetSeconds () << "," << kbs << "," << packetsReceived << "," << m_nSinks << std::endl;
  	out.close ();

	bytesTotal = 0;
  	packetsReceived = 0;
  	Simulator::Schedule (Seconds (1.0), &ProjectTwoSimulation::CheckThroughput, this);
}

Ptr <Socket>  ProjectTwoSimulation::SetupPacketReceive (Ipv4Address addr, Ptr <Node> node)
{
  	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  	Ptr <Socket> sink = Socket::CreateSocket (node, tid);
	InetSocketAddress local = InetSocketAddress (addr, port);
  	sink->Bind (local);
  	sink->SetRecvCallback (MakeCallback ( &ProjectTwoSimulation::ReceivePacket, this));

  	return sink;
}

void  ProjectTwoSimulation::RunSim (uint32_t nWifis, uint32_t nSinks, double totalTime, std::string rate,
                           std::string phyMode, uint32_t nodeSpeed, uint32_t nodePause, uint32_t periodicUpdateInterval, uint32_t settlingTime,
                           double dataStart, bool printRoutes, std::string CSVfileName)
{
	m_nWifis = nWifis;
	m_nSinks = nSinks;
	m_totalTime = totalTime;
	m_rate = rate;
	m_phyMode = phyMode;
	m_nodeSpeed = nodeSpeed;
	m_nodePause = nodePause;
	m_periodicUpdateInterval = periodicUpdateInterval;
	m_settlingTime = settlingTime;
	m_dataStart = dataStart;
	m_printRoutes = printRoutes;
	m_CSVfileName = CSVfileName;

	//Creating Strings for Tracefile naming	
	std::stringstream ss;
	ss << m_nWifis;
	std::string t_nodes = ss.str ();
	std::stringstream ss3;
	ss3 << m_nodePause;
	std::string snodePause = ss3.str ();
	std::string tr_name = "Project2_" + t_nodes + "Nodes_" + snodePause + "Pause" ;
	std::cout << "Trace file generated is " << tr_name << ".tr\n";

	
	CreateNodes ();
	CreateDevices (tr_name);
	SetupMobility ();
	InstallInternetStack (tr_name);
	InstallApplications ();

	std::cout << "\nStarting simulation for " << m_totalTime << " s ...\n";

	CheckThroughput ();


	//CODE FOR NETWORK ANIMATION 
        //AnimationInterface anim ("animation.xml");
	//anim.SetMaxPktsPerTraceFile(500000);
        //anim.SetMobilityPollInterval (Seconds (1));

	Simulator::Stop (Seconds (m_totalTime));
	Simulator::Run ();
	Simulator::Destroy ();
}

void  ProjectTwoSimulation::CreateNodes ()
{
  	std::cout << "Creating " << (unsigned) m_nWifis << " nodes.\n";
  	nodes.Create (m_nWifis);
}

void  ProjectTwoSimulation::SetupMobility ()
{
	MobilityHelper mobility;
	ObjectFactory pos;
	pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");
	pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
	pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));

	std::ostringstream speedConstantRandomVariableStream;
	speedConstantRandomVariableStream << "ns3::ConstantRandomVariable[Constant="
		                   << m_nodeSpeed
		                   << "]";

	std::ostringstream pauseConstantRandomVariableStream;
	pauseConstantRandomVariableStream << "ns3::ConstantRandomVariable[Constant="
		                   << m_nodePause
		                   << "]";

	Ptr <PositionAllocator> taPositionAlloc = pos.Create ()->GetObject <PositionAllocator> ();
	mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel", "Speed", StringValue (speedConstantRandomVariableStream.str ()),
		             "Pause", StringValue (pauseConstantRandomVariableStream.str ()), "PositionAllocator", PointerValue (taPositionAlloc));
	mobility.SetPositionAllocator (taPositionAlloc);
	mobility.Install (nodes);
}

void  ProjectTwoSimulation::CreateDevices (std::string tr_name)
{
	WifiMacHelper wifiMac;
	wifiMac.SetType ("ns3::AdhocWifiMac");
	YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
	wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue (-80) );      
  	wifiPhy.Set ("CcaMode1Threshold", DoubleValue(-81)); 
	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
	wifiPhy.SetChannel (wifiChannel.Create ());
	WifiHelper wifi;
	wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue (m_phyMode), "ControlMode",
		                StringValue (m_phyMode));
	devices = wifi.Install (wifiPhy, wifiMac, nodes);
	
	AsciiTraceHelper ascii;
  	wifiPhy.EnableAsciiAll (ascii.CreateFileStream (tr_name + ".tr"));


	//PCAP FILE CODE  	
	//wifiPhy.EnablePcapAll (tr_name);

  
}


/////////////////////////DSDV ROUTING///////////

void  ProjectTwoSimulation::InstallInternetStack (std::string tr_name)
{
  	DsdvHelper dsdv;
  	dsdv.Set ("PeriodicUpdateInterval", TimeValue (Seconds (m_periodicUpdateInterval)));
  	dsdv.Set ("SettlingTime", TimeValue (Seconds (m_settlingTime)));
  	InternetStackHelper stack;
  	stack.SetRoutingHelper (dsdv);
  	stack.Install (nodes);
  	Ipv4AddressHelper address;
  	address.SetBase ("10.1.1.0", "255.255.255.0");
  	interfaces = address.Assign (devices);
  	if (m_printRoutes)
    		{
      		Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ((tr_name + ".routes"), std::ios::out);
      		dsdv.PrintRoutingTableAllAt (Seconds (m_periodicUpdateInterval), routingStream);
    		}
    
    AsciiTraceHelper ascii2;
    Ptr<OutputStreamWrapper> stream = ascii2.CreateFileStream("IP_Trace.tr");
    stack.EnableAsciiIpv4All (stream);

}


//////////////////////////DSR ROUTING/////////////
/*
void  ProjectTwoSimulation::InstallInternetStack (std::string tr_name)
{
  	DsrHelper dsr;
	DsrMainHelper dsrMain;
  	InternetStackHelper stack;
  	stack.Install (nodes);
	dsrMain.Install (dsr, nodes);
  	Ipv4AddressHelper address;
  	address.SetBase ("10.1.1.0", "255.255.255.0");
  	interfaces = address.Assign (devices);

    AsciiTraceHelper ascii2;
    Ptr<OutputStreamWrapper> stream = ascii2.CreateFileStream("IP_Trace.tr");
    stack.EnableAsciiIpv4All (stream);
}
*/
///////////////////////AODV ROUTIING//////////////
/*
void  ProjectTwoSimulation::InstallInternetStack (std::string tr_name)
{
  	AodvHelper aodv;
  	InternetStackHelper stack;
  	stack.SetRoutingHelper (aodv);
  	stack.Install (nodes);
  	Ipv4AddressHelper address;
  	address.SetBase ("10.1.1.0", "255.255.255.0");
  	interfaces = address.Assign (devices);
  	if (m_printRoutes)
    		{
      		Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ((tr_name + ".routes"), std::ios::out);
      		aodv.PrintRoutingTableAllAt (Seconds (m_periodicUpdateInterval), routingStream);
    		}

    AsciiTraceHelper ascii2;
    Ptr<OutputStreamWrapper> stream = ascii2.CreateFileStream("IP_Trace.tr");
    stack.EnableAsciiIpv4All (stream);
}
*/

void  ProjectTwoSimulation::InstallApplications ()
{
	for (uint32_t i = 0; i <= m_nSinks - 1; i++ )
	   	{
	      	Ptr<Node> node = NodeList::GetNode (i);
	      	Ipv4Address nodeAddress = node->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal ();
	      	Ptr<Socket> sink = SetupPacketReceive (nodeAddress, node);
	    	}

  	for (uint32_t clientNode = 0; clientNode <= m_nWifis - 1; clientNode++ )
    		{
      		for (uint32_t j = 0; j <= m_nSinks - 1; j++ )
        		{
          		OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address (InetSocketAddress (interfaces.GetAddress (j), port)));
          		onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
          		onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));

          		if (j != clientNode)
            			{
              			ApplicationContainer apps1 = onoff1.Install (nodes.Get (clientNode));
              			Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable> ();
              			apps1.Start (Seconds (var->GetValue (m_dataStart, m_dataStart + 1)));
              			apps1.Stop (Seconds (m_totalTime));
            			}
        		}
    		}
}






