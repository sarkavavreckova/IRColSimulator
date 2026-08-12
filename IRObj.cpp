#include <map>
#include "IRObj.h"

// ****************************************************************
// class IRObject - methods:
// ****************************************************************

IRObject::IRObject() {
    name = "";
    hasValue = false;
    value = -1;
}

IRObject::IRObject(string n) {
    name = n;
    hasValue = false;
    value = -1;
}

IRObject::IRObject(string n, int val) {
    name = n;
    hasValue = true;
    value = val;
}

bool IRObject::operator<(const IRObject& other) const {
    if(name < other.name)  
        return true;
    if(name == other.name) {
        if(!other.hasValue)
            return false;
        if(!hasValue && other.hasValue)
            return true;
        if(hasValue && !other.hasValue)
            return false;
        return(value < other.value);
    }
    return false;
}

void IRObject::print() const {
    cout << name;
    if(hasValue)
        cout << '(' << value << ')';
}

string IRObject::getAsString() const {
    if(hasValue)
        return name+"("+to_string(value)+")";
    else return name;
}


// ****************************************************************
// class IRSet - methods:
// ****************************************************************


IRSet::IRSet(const IRObject& iro) {
    if (iro.name != "") data[iro.name]=iro;
}

IRSet::IRSet(const IRObject& iro1, const IRObject& iro2) {
    if (iro1.name != "") data[iro1.name]=iro1;
    if (iro2.name != "") data[iro2.name]=iro2;
}

IRSet::IRSet(const map<string,IRObject>& d) {
    data = d;
}

void IRSet::insert(const IRObject& iro) {
    if (iro.name != "") data[iro.name] = iro;
}

void IRSet::erase(const IRObject& iro) {
    auto iterIR = findName(iro.name);
    if (iterIR != data.end()) 
        data.erase(iterIR);
}

map<string,IRObject>::iterator IRSet::find(const IRObject& iro) {
    auto itr = data.find(iro.name);
    if ((itr != data.end()) && (iro.hasValue == itr->second.hasValue) && (iro.value ==itr->second.value))
        return itr;
    return data.end();
}

map<string,IRObject>::const_iterator IRSet::findName(const string n) const {
    return data.find(n);
}

bool IRSet::contains(const IRObject& iro) const {
    auto itr = data.find(iro.name);
    if ((itr != data.end()) && (iro.hasValue == itr->second.hasValue) && (iro.value ==itr->second.value))
        return true;
    return false;
}

bool IRSet::containsName(const string n) const {
    return(data.find(n) != data.end());
}

void IRSet::clear() {
    data.clear();
}

map<string,IRObject>::const_iterator IRSet::begin() const {
    return data.begin();
}

map<string,IRObject>::const_iterator IRSet::end() const {
    return data.end();
}

/*
void IRSet::intersection(const IRSet& leftset, const IRSet& rightset) {
    data.clear();
    for(auto itr = leftset.begin(); itr != leftset.end(); ++itr) {
        auto itr2 = rightset.findName(itr->name);
        if(itr2 != rightset.end()) {
            if(!itr->hasValue) {
                this->insert(*itr2);
            }
            else if(!itr2->hasValue) {
                this->insert(*itr);
            }
        }
    }
//    throw("IRSet: Error in the intersection operation"); 
}

void IRSet::unionSet(const IRSet& leftset, const IRSet& rightset) {
    data.clear();
    for(auto itr = rightset.begin(); itr != rightset.end(); ++itr) 
        data.insert(*itr);
    for(auto itr = leftset.begin(); itr != leftset.end(); ++itr) 
        this->insert(*itr);
//        cout << "Error in the union operation" << endl;
}
*/

void IRSet:: print() const {
    for(auto itr = this->begin(); itr != this->end(); ++itr) {
        itr->second.print();
        cout << '\t';
    }
}


// ****************************************************************
// class Effector - methods:
// ****************************************************************


string effectorTypeToString(EffectorType t) {
    switch(t) {
    case OBJECT:            return "OBJECT";
    case OBJECTWITHREL:     return "OBJECTWITHREL";
    case RELBETWEENOBJECTS: return "RELBETWEENOBJECTS";
    default: return "";
    }
}


Effector::Effector() {
    type = NONE;
}

Effector::Effector(const string o) {
    type = OBJECT;
    objectName = o;
}

Effector::Effector(const string o, const EffectorRelation rel, const int relVal) {
    type = OBJECTWITHREL;
    objectName = o;
    relation = rel;
    relValue = relVal;
}

Effector::Effector(const string o1, const EffectorRelation rel, const string o2) {
    type = RELBETWEENOBJECTS;
    objectName = o1;
    relation = rel;
    objectName2 = o2;
}

string Effector::getAsString() const {
    switch(type) {
    case OBJECT:
        return effectorTypeToString(OBJECT) + " " + objectName;
        
    case OBJECTWITHREL: {
        string pom = effectorTypeToString(OBJECTWITHREL) + " ";
        switch(relation) {
        case EQUAL:
            return pom + objectName + '(' + to_string(relValue) + ')';
        case LESS:
            return pom + objectName + "(<" + to_string(relValue) + ')';
        case GREATER:
            return pom + objectName + "(>" + to_string(relValue) + ')';
        default:
            throw("Unsupported type of relation.");
            return " ";
        }}
        
    case RELBETWEENOBJECTS: {
        string pom = effectorTypeToString(OBJECTWITHREL) + " ";
        switch(relation) {
        case EQUAL:
            return pom + objectName + "(val) == " + objectName2 + "(val)";
        case LESS:
            return pom + objectName + "(val) < " + objectName2 + "(val)";
        case GREATER:
            return pom + objectName + "(val) > " + objectName2 + "(val)";
        default:
            throw("Unsupported type of relation.");
            return " ";
        }}
    default:
        throw("Unsupported type of effector.");
    }
}

bool Effector::operator<(const Effector& other) const {
    if ((type < other.type) || (objectName < other.objectName) || (objectName2 < other.objectName2) 
        || (relation < other.relation) || (relValue < other.relValue))
        return true;
    return false;
}

bool Effector::correspondsToEnv(const IRSet& irset) const {
    switch(type) {
    case OBJECT: {
        auto itr = irset.findName(objectName);
        if(itr != irset.end()) 
            return true;
        else return false;
    }
    
    case OBJECTWITHREL: {
        auto itr = irset.findName(objectName);
        if(itr != irset.end()) {
            switch(relation) {
            case EQUAL:
                if((itr->second.hasValue) &&((itr->second.value)==relValue))
                    return true;
                else return false;
            
            case LESS:
                if((itr->second.hasValue) &&((itr->second.value)<relValue))
                    return true;
                else return false;
            
            case GREATER:
                if((itr->second.hasValue) &&((itr->second.value)>relValue))
                    return true;
                else return false;
            
            default:
                throw("Unsupported type of relation.");
                return false;
            }
        }
        else return false;
    }
    
    case RELBETWEENOBJECTS: {
        auto itr = irset.findName(objectName);
        auto itr2 = irset.findName(objectName2);
        if(itr != irset.end() && itr2 != irset.end()) {
            switch(relation) {
            case EQUAL:
                if((itr->second.hasValue) &&(itr2->second.hasValue) &&((itr->second.value)==itr2->second.value))
                    return true;
                else return false;
            
            case LESS:
                if((itr->second.hasValue) &&(itr2->second.hasValue) &&((itr->second.value)<itr2->second.value))
                    return true;
                else return false;
            
            case GREATER:
                if((itr->second.hasValue) &&(itr2->second.hasValue) &&((itr->second.value)>itr2->second.value))
                    return true;
                else return false;
            
            default:
                throw("Unsupported type of relation.");
                return false;
            }
        }
        else return false;
    }
    
    default:
        throw("Unsupported type of effector.");
    }
}


// ****************************************************************
// class EffectorSet - methods:
// ****************************************************************

EffectorSet::EffectorSet() {
}

EffectorSet::EffectorSet(const Effector efr) {
    data.insert(efr);
}

EffectorSet::EffectorSet(const set<Effector> d) {
    data = d;
}

bool EffectorSet::empty() const {
    return data.empty();
}

void EffectorSet::clear() {
    data.clear();
}

set<Effector>::iterator EffectorSet::begin() const {
    return data.begin();
}

set<Effector>::iterator EffectorSet::end() const {
    return data.end();
}

void EffectorSet::print() const {
    for(auto itr = data.begin(); itr != data.end(); ++itr) {
        cout << itr->getAsString() << '\t';
    }
}

void EffectorSet::insert(const Effector& efr) {
    if(find(efr) == data.end())
         data.insert(efr);
}

set<Effector>::iterator EffectorSet::find(const Effector& efr) const {
    return data.find(efr);
}

set<Effector>::iterator EffectorSet::findName(const string n) const {
    for(auto itr = data.begin(); itr != data.end(); ++itr)
        if(itr->objectName == n)
            return itr;
    return data.end();
}

set<Effector>::iterator EffectorSet::findName2(const string n) const {
    for(auto itr = data.begin(); itr != data.end(); ++itr)
        if(itr->objectName2 == n)
            return itr;
    return data.end();
}

bool EffectorSet::contains(const Effector& efr) const {
    return data.find(efr) != data.end();
}

bool EffectorSet::containsName(const string n) const {
    return (findName(n) != data.end());
}

bool EffectorSet::containsName2(const string n) const {
    return (findName2(n) != data.end());
}


