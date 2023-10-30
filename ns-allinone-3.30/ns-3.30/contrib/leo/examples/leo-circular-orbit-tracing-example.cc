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
 *
 * Author: Tim Schubert <ns-3-leo@timschubert.net>
 */

#include <fstream>

#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/leo-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LeoCircularOrbitTracingExample");

void CourseChange (std::string context, Ptr<const MobilityModel> position)
{
  Vector pos = position->GetPosition ();
  Ptr<const Node> node = position->GetObject<Node> ();
  std::cout << Simulator::Now () << ":" << node->GetId () << ":" << pos.x << ":" << pos.y << ":" << pos.z << ":" << position->GetVelocity ().GetLength() << std::endl;
}

int main(int argc, char *argv[])
{
  CommandLine cmd;
  std::string orbitFile;
  std::string traceFile;
  std::string duration = "60s";
  cmd.AddValue("orbitFile", "CSV file with orbit parameters", orbitFile);
  cmd.AddValue("traceFile", "CSV file to store mobility trace in", traceFile);
  cmd.AddValue("precision", "ns3::LeoCircularOrbitMobilityModel::Precision");
  cmd.AddValue("duration", "Duration of the simulation in seconds", duration);
  cmd.Parse (argc, argv);

  LeoOrbitNodeHelper orbit;
  NodeContainer satellites;
  if (!orbitFile.empty())
    {
      satellites = orbit.Install (orbitFile);
    }
  else
    {
      satellites = orbit.Install ({ LeoOrbit (1200, 20, 32, 16),
      				  LeoOrbit (1180, 30, 12, 10) });
    }

  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));

  std::streambuf *coutbuf = std::cout.rdbuf();
  // redirect cout if traceFile is specified
  std::ofstream out;
  out.open (traceFile);
  if (out.is_open ())
    {
      std::cout.rdbuf(out.rdbuf());
    }

  std::cout << "Time,Satellite,x,y,z,Speed" << std::endl;

  Simulator::Stop (Time (duration));
  Simulator::Run ();
  Simulator::Destroy ();

  out.close ();
  std::cout.rdbuf(coutbuf);
}
