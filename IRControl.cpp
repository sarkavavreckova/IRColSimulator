#include "IRControl.h"

#include <cctype>


// ****************************************************************
// class IRColony - methods:
// ****************************************************************


IRColony::IRColony() {
    sleepTime = 4;
}

IRColony::IRColony(const IRSet& st) {
    sleepTime = 4;
    env = st;
}

void IRColony::addAgent(const string agentLab, const IRSet& agentEnv, const set<IRProgram>& progs) {
    auto itr = agents.find(agentLab);
    if (itr != agents.end()) {
        throw("There is existing agent with the same label.");
    }
    else {
        agents.emplace(agentLab,Agent(agentLab,agentEnv,progs));
    }
}

void IRColony::addAgent(const Agent& agent) {
    agents.emplace(agent.label,agent);      // or agents[agent.label] = agent;
}


void IRColony::worker() {
    while(working) {
        for (auto& [label, agent] : agents) {
            agent.work(env, progRepository);
        }
        this_thread::sleep_for(chrono::seconds(sleepTime));
    }
}

void IRColony::addLinkBetweenAgents(const string aFrom, const string aTo) {
    auto itrF = agents.find(aFrom);
    auto itrS = agents.find(aTo);
    if (itrF != agents.end() && itrS != agents.end()) {
        ((itrF->second).recipients).insert(&(itrS->second));
    }
}

void IRColony::start() {
    working = true;
    workerThread = thread(&IRColony::worker, this);
}

void IRColony::stop() {
    working = false;
    if (workerThread.joinable())
        workerThread.join();
}

void IRColony::printMenu() const {
    cout << "\n**********************************************************************************";
    cout << "\nPress one of the following keys (and ENTER), you can use both uppercase/lowercase:\n\n";
    cout << "P ... Print agents info\n";
    cout << "S ... Start (start IRColony)\n";
    cout << "E ... End (stop working)\n";
    cout << "D ... Print state of devices\n";
    cout << "+ ... Increase temperature\n";
    cout << "- ... Decrease temperature\n";
    cout << "V ... Verbose mode (undo: T)\n";
    cout << "T ... Silent mode\n";
    cout << "Q ... Quit this program\n";
    cout << "**********************************************************************************\n";
    cout << "Your choice: ";
}

void IRColony::communication() {
    char key = 'A';
    printMenu();
    
    do {
        cin >> key;
        cin.ignore();
        key = toupper(key);
        switch(key) {
        case 'P': // Print agents info
            for (auto& itr : agents) 
                (itr.second).print();
            printMenu();
            break;
        
        case 'S': // Start
            start();
            cout << "\nIR Colony is working now.\n\n";
            cout << "Your choice: ";
            break;
        
        case 'E': // End 
            stop();
            cout << "\nIR Colony has been stopped.\n\n";
            cout << "Your choice: ";
            break;
        
        case 'D': // Print state of devices
            for (auto& itr : agents) 
                (itr.second).printDeviceState();
            printMenu();
            break;
        
        case '+': { // Increase temperature
            auto agItr = agents.find("Thermometer");
            auto itr = agItr->second.env.findName("temp");
            if (itr != agItr->second.env.end()) {
                IRObject obj("temp",(itr->second.value)+1);
                agItr->second.env.erase(itr->second);
                agItr->second.env.insert(obj);
                if (verboseMode)
                    cout << "\nNew temperature reported by Thermometer: " << obj.getAsString();
            }
            else cout << "The object is not present.";
            cout << "\n\nYour choice: ";
            break;
        }
        case '-': {// Decrease temperature
            auto agItr = agents.find("Thermometer");
            auto itr = agItr->second.env.findName("temp");
            if (itr != agItr->second.env.end()) {
                IRObject obj("temp",(itr->second.value)-1);
                agItr->second.env.erase(itr->second);
                agItr->second.env.insert(obj);
                if (verboseMode)
                    cout << "\nNew temperature reported by Thermometer: " << obj.getAsString();
            }
            else cout << "The object is not present.";
            cout << "\n\nYour choice: ";
            break;
        }    
        
        case 'V': // Verbose mode
            verboseMode = true;
            cout << "\nVerbose mode is on.\n\nYour choice: ";
            break;
        
        case 'T': // Silent mode
            verboseMode = false;
            cout << "\nVerbose mode is off.\n\n";
            printMenu();
           break;
        
        default: continue;
        }
        
    } while (key != 'Q');
}

