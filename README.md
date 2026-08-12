IR Colonies is a mathematical model designed to simulate communication in IoT networks. IRCol Simulator is a simple application based on this model.
This simulator could be used e.g. when designing a new IoT network, simulating data flows, or optimizing an existing network design. IoT devices are represented 
by agents, each agent has its state (e.g. a thermometer has a state indicating the current temperature; a window control mechanism has an open/closed state)
and the set of programs. Every program has a set of execution rules, a set of reactants and a set of inhibitors. 
The agent regularly runs one of the programmes (a program can be run if all the reactant conditions are met and none of the inhibitor conditions are met).
The project is currently in its early stages and implements only the basic features of the IR Colonies model. The following features will be added gradually:
* input, i.e. the structure of the IoT network, in JSON format (for demonstration purposes, several agents have been hard-coded into the code),
* programming rules intended for updating agents' programs,
* real parallelism (threads for all agents). 

Detailed information about IR Colonies can be found on the website https://ceur-ws.org/Vol-4092/paper21.pdf
