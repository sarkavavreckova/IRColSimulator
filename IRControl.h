#ifndef IRCONTROL_H
#define IRCONTROL_H

#include <iostream>
#include <set>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>
#include <map>

#include "IRObj.h"
#include "IRAgents.h"

using namespace std;


class IRColony {
private:
    unsigned sleepTime;
    atomic<bool> working{false};
    thread workerThread;
    
    void worker();
    
public:
    IRSet env;
    map<string,Agent> agents;
    set<IRProgram> progRepository;
    
    IRColony();
    IRColony(const IRSet& st);
    
    void addAgent(const string agentLab, const IRSet& agentEnv, const set<IRProgram>& progs);
    void addAgent(const Agent& agent);
    void addLinkBetweenAgents(const string aFrom, const string aTo);
    
    void start();
    void stop();
    
    void printMenu() const;
    void communication();
};



#endif