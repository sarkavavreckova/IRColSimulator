/*******************************************************************************

IR Colony Simulator, version 0.1
Sarka Vavreckova
Institute of Computer Science, Faculty of Philosophy na Science,
Silesian University in Opava
last update: 2026-07-10

Simulator of IoT network based on the IR Colony mathematical model

Files:
* IRObj.h, IRObj.cpp ..... objects and effectors
* IRAgents.h, IRAgents.cpp ... programs and their rules, agents
* IRControl.h, IRControl.cpp ... main control module, start/stop, timing, threads

********************************************************************************/

#include <iostream>
#include <set>
#include <string>
#include <map>

using namespace std;

#include "IRObj.h"
#include "IRAgents.h"
#include "IRControl.h"



// %%%%%%%%%%%%%%%%%%%%%%
int main()
{
    // Example IR Colony instance:
    IRColony ircolony;
    ircolony.env.insert(IRObject("lbVersion",1));
    
    IRProgram hProg;
    
    // Smart Light Bulb
    Agent hAgent("LightBulb");
    hAgent.env.insert(IRObject("lbIntensity",8));
    hAgent.env.insert(IRObject("lbState",0));
    ircolony.addAgent(hAgent);
    
    // Control Panel
    hAgent.label = "ControlPanel";
    hAgent.env.clear();
    hAgent.env.insert(IRObject("cLightState",0));
    hAgent.env.insert(IRObject("cWinState",0));
    hAgent.env.insert(IRObject("cWinManual",0));
    hProg.setLabel("cLight");
    hProg.addRule(ProgRule(MULTICAST, IRObject("cLightState"), IRObject("lbState")));
    hAgent.programs.insert(hProg);
    hProg.clear();
    hProg.setLabel("cWindowState");
    hProg.addRule(ProgRule(MULTICAST, IRObject("cWinState"), IRObject("wState")));
    hProg.addReactant(Effector("cWinManual",EQUAL,1));
    hAgent.programs.insert(hProg);
    hProg.clear();
    hProg.setLabel("cWindowManual");
    hProg.addRule(ProgRule(MULTICAST, IRObject("cWinManual"), IRObject("wManual")));
    hAgent.programs.insert(hProg);
    ircolony.addAgent(hAgent);

    // Window Control
    hAgent.label = "WindowControl";
    hAgent.env.clear();
    hAgent.env.insert(IRObject("wState",0));
    hAgent.env.insert(IRObject("wManual",0));
    hAgent.programs.clear();
    hProg.clear();
    hProg.setLabel("wOpenThermo");
    hProg.addRule(ProgRule(EVOLUTION, IRObject("wState",0), IRObject("wState",1)));
    hProg.addReactant(Effector("temp",GREATER,22));
    hProg.addInhibitor(Effector("wManual",EQUAL,1));
    hAgent.programs.insert(hProg);
    hProg.clear();
    hProg.setLabel("wOpenCO2");
    hProg.addRule(ProgRule(EVOLUTION, IRObject("wState",0), IRObject("wState",1)));
    hProg.addReactant(Effector("sCO2State",GREATER,1200));
    hProg.addInhibitor(Effector("wManual",EQUAL,1));
    hAgent.programs.insert(hProg);
    hProg.clear();
    hProg.setLabel("wClose");
    hProg.addRule(ProgRule(EVOLUTION,IRObject("wState",1), IRObject("wState",0)));
    hProg.addInhibitor(Effector("wManual",EQUAL,1));
    hProg.addInhibitor(Effector("temp",GREATER,22));
    hProg.addInhibitor(Effector("sCO2State",GREATER,1200));
    hAgent.programs.insert(hProg);
    ircolony.addAgent(hAgent);
    
    // Thermometer
    hAgent.label = "Thermometer";
    hAgent.env.clear();
    hAgent.env.insert(IRObject("temp",24));
    hAgent.programs.clear();
    hProg.clear();
    hProg.setLabel("tempOut");
    hProg.addRule(ProgRule(MULTICAST, IRObject("temp"), IRObject("temp")));
    hAgent.programs.insert(hProg);
    ircolony.addAgent(hAgent);
    
    ircolony.addLinkBetweenAgents("ControlPanel","LightBulb");
    ircolony.addLinkBetweenAgents("ControlPanel","WindowControl");
    ircolony.addLinkBetweenAgents("Thermometer","WindowControl");

    ircolony.communication();

    return 0;
}
// %%%%%%%%%%%%%%%%%%%%%%


