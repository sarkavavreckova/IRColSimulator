#include "IRAgents.h"


string ruleTypeToString(RuleType r) {
    switch(r) {
    case EVOLUTION:     return "EVOLUTION";
    case DELETION:      return "DELETION";
    case MULTICAST:     return "MULTICAST";
    case BACKUP:        return "BACKUP";
    case RESTORATION:   return "RESTORATION";
    case PROGRAMMING:   return "PROGRAMMING";
    }
    return "";
}

// ****************************************************************
// class ProgRule - methods:
// ****************************************************************

ProgRule::ProgRule(RuleType t, const IRObject& o) {
    type = t;
    switch(t) {
    case DELETION:
        inObject = o;
        break;
        
    case MULTICAST:
        inObject = o;
        outObject = o;
        break;
        
    case BACKUP:
        outObject = o;
        break;
        
    case RESTORATION:
        outObject = o;
        break;
    
    default:
        throw(ruleTypeToString(t) + ": This rule type needs different parameters.");
    }
}

ProgRule::ProgRule(RuleType t, const IRObject& inObj, const IRObject& outObj) {
    type = t;
    switch(t) {
    case EVOLUTION:
    case MULTICAST:
        inObject = inObj;
        outObject = outObj;
        break;
    
    default:
        throw(ruleTypeToString(t) + ": This rule type needs different parameters.");
    }
}

string ProgRule::getAsString() const {
    switch(type) {
    case EVOLUTION:
        return inObject.getAsString()+" -> "+outObject.getAsString();
    case DELETION:
        return inObject.getAsString()+" -> eps";
    case MULTICAST:
        return inObject.getAsString()+" ->out "+outObject.getAsString();
    case BACKUP:
        return inObject.getAsString()+" ->";
    case RESTORATION:
        return "<- "+outObject.getAsString();
//        case PROGRAMMING:
    }
    return "";
}

void ProgRule::print() const {
    cout << '(' << ruleTypeToString(type) << ": " << this->getAsString() << ')';
}


bool ProgRule::operator<(const ProgRule& other) const {
    if ( (type < other.getType()) || (inObject < other.inObject) || (outObject < other.outObject)) 
        return true;
    return false;
}


bool ProgRule::applicable(const IRSet& agentEnv, const IRSet& IREnv) const {
    switch(type) {
    case EVOLUTION:
    case DELETION:
    case MULTICAST:
    case BACKUP:
        return agentEnv.containsName(inObject.name);
    case RESTORATION:
        return IREnv.containsName(outObject.name);
    default: 
        throw ("Unsupported type of rule.");
        return false;
    }
}

void ProgRule::apply(const string agentLabel, const string progLabel, IRSet& agentEnv, IRSet& IREnv, set<Agent*>& recip) const {
    switch(type) {
    case EVOLUTION: {
        auto itr=agentEnv.findName(inObject.name);
        agentEnv.erase(inObject);
        agentEnv.insert(outObject);
        if (verboseMode)
            cout << endl << "Agent " << agentLabel << ",\tprogram " << progLabel << ",\trule " << ruleTypeToString(type) << ":\t" << itr->second.getAsString() << " -> " << outObject.getAsString();
        break;
    }
    case DELETION: {
        auto itr=agentEnv.findName(inObject.name);
        agentEnv.erase(inObject);
        if (verboseMode)
            cout << endl << "Agent " << agentLabel << ",\tprogram " << progLabel << ",\trule " << ruleTypeToString(type) << ":\t" << itr->second.getAsString() << " -> epsilon";
        break;
    }
    case MULTICAST: {
        IRObject objectToSend;
        if (outObject.hasValue) 
            objectToSend = outObject;
        else if (inObject.hasValue) {
            objectToSend.name = outObject.name;
            objectToSend.hasValue = true;
            objectToSend.value = inObject.value;
        }
        else {
            objectToSend = outObject;
            objectToSend.hasValue = true;
            auto itr= agentEnv.findName(inObject.name);
            objectToSend.value = itr->second.value;
        }    
        for (auto itr = recip.begin(); itr != recip.end(); ++itr) {
            (**itr).acceptMessage(objectToSend);
        }
        if (verboseMode)
            cout << endl << "Agent " << agentLabel << ",\tprogram " << progLabel << ",\trule " << ruleTypeToString(type) << ":\t" << objectToSend.getAsString() << " ->out";
        break;
    }
    case BACKUP: {
        IREnv.insert(inObject);
        if (verboseMode)
            cout << endl << "Agent " << agentLabel << ",\tprogram " << progLabel << ",\trule " << ruleTypeToString(type) << ":\t" << inObject.getAsString() << " -> environment";
        break;
    }
    case RESTORATION: {
        auto iro = IREnv.findName(outObject.name);
        agentEnv.insert(iro->second);
        if (verboseMode)
            cout << endl << "Agent " << agentLabel << ",\tprogram " << progLabel << ",\trule " << ruleTypeToString(type) << ":\t" << iro->second.getAsString() << " <- environment";
        break;
    }
    default: 
        throw ("Unsupported type of rule.");
    }
}



// ****************************************************************
// class IRProgram - methods:
// ****************************************************************


IRProgram::IRProgram() {
    label = "";
}

IRProgram::IRProgram(string lab) {
    label = lab;
}

//enum RuleType { EVOLUTION, DELETION, MULTICAST, BACKUP, RESTORATION, PROGRAMMING };


void IRProgram::setLabel(string lab) {
    label = lab;
}

void IRProgram::print() const {
    cout << "\n** Program " << label << "\nRules:\n";
    for (const auto& itr : rules) {
        itr.print();
        cout << '\t';
    }
    cout << "\nReactants (" << reactants.size() <<  " effectors):\t";
    reactants.print();
    cout << "\nInhibitors (" << inhibitors.size() <<  " effectors):\t";
    inhibitors.print();
    cout << endl;
}


void IRProgram::clear() {
    rules.clear();
    reactants.clear();
    inhibitors.clear();
}

bool IRProgram::operator<(const IRProgram& other) const {
    return (label < other.getLabel());
}


void IRProgram::addRule(const ProgRule& rule) {
    rules.insert(rule);
//    RuleType rt = rule.getType();
//    if ((rt == EVOLUTION || rt == DELETION || rt == MULTICAST || rt == BACKUP) && rules.containsName(rule.inObject.name))
//        return false;
//    return true;
}

void IRProgram::addReactant(const Effector& efr) {
    reactants.insert(efr);
}


void IRProgram::addInhibitor(const Effector& efr) {
    inhibitors.insert(efr);
}

bool IRProgram::applicable(const IRSet& agentEnv, const IRSet& IREnv) const {
    for(auto itr = rules.begin(); itr != rules.end(); ++itr) {
        if (!(itr->applicable(agentEnv, IREnv)))
            return false;
    }
    
    for(auto itr = reactants.begin(); itr != reactants.end(); ++itr) {
        if (!(itr->correspondsToEnv(agentEnv))) {
            return false;
        }
    }
    
    for(auto itr = inhibitors.begin(); itr != inhibitors.end(); ++itr) {
        if (itr->correspondsToEnv(agentEnv)) {
            return false;
        }
    }
    return true;    
}

void IRProgram::apply(const string agentLabel, IRSet& agentEnv, IRSet& IREnv, set<Agent*>& recip) const {
    // suppose the program is applicable, verified before calling this method
    for (auto itr = rules.begin(); itr != rules.end(); ++itr) {
        itr->apply(agentLabel, label, agentEnv, IREnv, recip);
    }
}


// ****************************************************************
// class Agent - methods:
// ****************************************************************


Agent::Agent() {
    label = "";
}

Agent::Agent(string lab) {
    label = lab;
}

Agent::Agent(string lab, const IRSet& st) {
    label = lab;
    env = st;
}

Agent::Agent(string lab, const IRSet& st, const set<IRProgram>& progs) {
    label = lab;
    env = st;
    programs = progs;
}


void Agent::print() const {
    cout << "\n*************************************\nAgent "<< label << ":\nEnvironment (" << env.size() << " objects):\t";
    env.print();
    cout << "\n\nPrograms (" << programs.size() << " programs):\n";
    for (const auto& itr : programs)
        itr.print();
    cout << "\nRecipients of multicast messages: ";
    for (const auto& itr : recipients)
        cout << itr->label << '\t';
    cout << endl;
}

void Agent::printDeviceState() const {
    cout << "\n" << label << ":\t";
    env.print();
}

void Agent::acceptMessage(IRObject& obj) {
    env.insert(obj);
}

void Agent::work(IRSet& IREnv, set<IRProgram>& repoz) {
    for (auto itr = programs.begin(); itr != programs.end(); ++itr) {
        if (itr->applicable(env, IREnv)) {
            itr->apply(label, env, IREnv, recipients);
            break;
        }
    }
}


