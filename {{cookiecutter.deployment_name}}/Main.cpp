// ======================================================================
// \title  Main.cpp
// \brief main program for the F' application. Intended for CLI-based systems (Linux, macOS)
//
// ======================================================================
// Used to access topology functions
#include <{{cookiecutter.deployment_name}}/Top/{{cookiecutter.deployment_name}}TopologyAc.hpp>
#include <{{cookiecutter.deployment_name}}/Top/{{cookiecutter.deployment_name}}Topology.hpp>

#include <Os/Console.hpp>
#include <Arduino/Os/Task.hpp>

// Task Runner
Os::Arduino::Task::TaskRunner taskrunner;

/**
 * \brief setup the program
 *
 * This is an extraction of the Arduino setup() function.
 * 
 */
void setup() {
    // Initialize OSAL
    Os::init();

    // Setup Serial
    Serial.begin(115200);
    Os::Task::delay(Fw::TimeInterval(1, 0));
    Fw::Logger::logMsg("Program Started\n");

    // Object for communicating state to the reference topology
    {{cookiecutter.deployment_name}}::TopologyState inputs;
    inputs.uartNumber = 0;
    inputs.uartBaud = 115200;

    // Setup topology
    {{cookiecutter.deployment_name}}::setupTopology(inputs);
}

/**
 * \brief run the program
 *
 * This is an extraction of the Arduino loop() function.
 * 
 */
void loop() {
#ifdef USE_BASIC_TIMER
    rateDriver.cycle();
#endif
    taskrunner.run();
}