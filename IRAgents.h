#ifndef IRAGENTS_H
#define IRAGENTS_H

#include <iostream>
#include <set>
#include <string>

#include "IRObj.h"

using namespace std;

enum RuleType { EVOLUTION, DELETION, MULTICAST, BACKUP, RESTORATION, PROGRAMMING };


class Agent;


class ProgRule {
private:
    RuleType type;
public:
    IRObject inObject, outObject;
    
    ProgRule(RuleType t, const IRObject& o);
    ProgRule(RuleType t, const IRObject& inObj, const IRObject& outObj);
    string getAsString() const;
    RuleType getType() const { return type; } 
    void print() const;
    bool operator<(const ProgRule& other) const;
    bool applicable(const IRSet& agentEnv, const IRSet& IREnv) const;
    void apply(const string agentLabel, const string progLabel, IRSet& agentEnv, IRSet& IREnv, set<Agent*>& recip) const;
};


class IRProgram {
private:
    string label;
    set<ProgRule> rules;
    EffectorSet reactants, inhibitors;
public:
    IRProgram();
    IRProgram(string lab);
    bool operator<(const IRProgram& other) const;
    string getLabel() const { return label; }
    void print() const;
    void setLabel(string lab);
    void clear();
    void addRule(const ProgRule& rule);
    void addReactant(const Effector& efr);
    void addInhibitor(const Effector& efr);
    bool applicable(const IRSet& agentEnv, const IRSet& IREnv) const;
    void apply(const string agentLabel, IRSet& agentEnv, IRSet& IREnv, set<Agent*>& recip) const;
};


class Agent {
private:
public:
    string label;
    IRSet env;
    set<IRProgram> programs;
    set<Agent*> recipients;          // to apply multicast rules (to send objects to recipients)
    
    Agent();
    Agent(string lab);
    Agent(string lab, const IRSet& st);
    Agent(string lab, const IRSet& st, const set<IRProgram>& progs);
    
    bool operator<(const Agent& other) const { return (label < other.label) ; }
    void print() const;
    void printDeviceState() const; 
    void acceptMessage(IRObject& obj);
    void work(IRSet& IREnv, set<IRProgram>& repoz);
};



#endif