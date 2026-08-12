

#ifndef IROBJ_H
#define IROBJ_H

#include <iostream>
#include <set>
#include <string>
#include <map>

using namespace std;

// global variable accessible in the whole project, for verbose mode detection
inline bool verboseMode = false;


class IRObject {
public:
    string name;
    bool hasValue;
    int value;

    IRObject();
    IRObject(string n);
    IRObject(string n, int val);
    bool operator<(const IRObject& other) const;
    void print() const;
    string getAsString() const;
 };

// set of IRObjects, the included objects should be unique by their names.
class IRSet {
private:
    map<string,IRObject> data;
public:
    IRSet() {};
    IRSet(const IRObject& iro);
    IRSet(const IRObject& iro1, const IRObject& iro2);
    IRSet(const map<string,IRObject>& d);
    long unsigned int size() const { return data.size(); }
    void insert(const IRObject& iro);
    void erase(const IRObject& iro);
    map<string,IRObject>::iterator find(const IRObject& iro);
    map<string,IRObject>::const_iterator findName(const string n) const;
    bool contains(const IRObject& iro) const;
    bool containsName(const string name) const;
    void clear();
    map<string,IRObject>::const_iterator begin() const;
    map<string,IRObject>::const_iterator end() const;
//    void intersection(const IRSet& leftset, const IRSet& rightset);
//    void unionSet(const IRSet& leftset, const IRSet& rightset);
    void print() const;
}; 


// Effectors are used as either reactants or inhibitors

enum EffectorType { NONE, OBJECT, OBJECTWITHREL, RELBETWEENOBJECTS, PROG };
// OBJECTWITHREL: e.g.(a,==24) means a.value == 24,(a,<10) means a.value < 10,(a,>0) means a.value >0 
//... if there is(not) this object in the agent's env with the corresponding value...
// OBJECT: if there is(not) this object in the agent's env(with any value, or without value)...
// RELBETWEENOBJECTS: e.g.(actual,>,limit) means that within the agent's env there are(not) two objects with the given relation

enum EffectorRelation { NONEREL, EQUAL, LESS, GREATER };    // nothing, ==, <, >
// objects only with no value => names(strings) 

// Reactants and inhibitors are effectors:
class Effector {
public:
    EffectorType type;
    string objectName;          // for OBJECT, OBJECTWITHREL, RELBETWEENOBJECTS
    string objectName2;         // for RELBETWEENOBJECTS
    EffectorRelation relation;  // for OBJECTWITHREL, RELBETWEENOBJECTS
    int relValue;               // for OBJECTWITHREL
    // IRProgram irprog;
    Effector();
    Effector(const string o);   // OBJECT
    Effector(const string o, const EffectorRelation rel, const int relVal); // OBJECTWITHREL
    Effector(const string o1, const EffectorRelation rel, const string o2); // RELBETWEENOBJECTS
    // Effector(const IRProgram prog);
    
    bool operator<(const Effector& other) const;
    string getAsString() const;
    bool correspondsToEnv(const IRSet& irset) const;
};


class EffectorSet {
private:
    set<Effector> data;
public:
    EffectorSet();
    EffectorSet(const Effector efr);
    EffectorSet(const set<Effector> d);

    bool empty() const;
    long unsigned int size() const { return data.size(); }
    void clear();
    set<Effector>::iterator begin() const;
    set<Effector>::iterator end() const;

    void print() const;
    void insert(const Effector& efr);
    
    set<Effector>::iterator find(const Effector& efr) const;
    set<Effector>::iterator findName(const string n) const;
    set<Effector>::iterator findName2(const string n) const;
    bool contains(const Effector& efr) const;
    bool containsName(const string n) const;
    bool containsName2(const string n) const;
}; 



#endif

