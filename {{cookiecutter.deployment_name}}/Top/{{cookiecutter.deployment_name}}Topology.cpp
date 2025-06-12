// ======================================================================
// \title  {{cookiecutter.deployment_name}}Topology.cpp
// \brief cpp file containing the topology instantiation code
//
// ======================================================================
// Provides access to autocoded functions
#include <{{cookiecutter.deployment_name}}/Top/{{cookiecutter.deployment_name}}TopologyAc.hpp>
// Note: Uncomment when using Svc:TlmPacketizer
// #include <{{cookiecutter.deployment_name}}/Top/{{cookiecutter.deployment_name}}PacketsAc.hpp>
#include <config/FppConstantsAc.hpp>
#include <Fw/Logger/Logger.hpp>

// Necessary project-specified types
#include <config/FprimeArduino.hpp>
#include <Fw/Types/MallocAllocator.hpp>
#include <Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector.hpp>

// Allows easy reference to objects in FPP/autocoder required namespaces
using namespace {{cookiecutter.deployment_name}};

// The reference topology uses a malloc-based allocator for components that need to allocate memory during the
// initialization phase.
Fw::MallocAllocator mallocator;

// FprimeFrameDetector is used to configure the FrameAccumulator to detect F Prime frames
Svc::FrameDetectors::FprimeFrameDetector frameDetector;

Svc::ComQueue::QueueConfigurationTable configurationTable;

// The reference topology divides the incoming clock signal (1Hz) into sub-signals: 1/100Hz, 1/200Hz, and 1/1000Hz
{{"Svc::RateGroupDriver::DividerSet rateGroupDivisors{{{100, 0}, {200, 0}, {1000, 0}}};"}}

// Rate groups may supply a context token to each of the attached children whose purpose is set by the project. The
// reference topology sets each token to zero as these contexts are unused in this project.
U32 rateGroup1Context[FppConstant_PassiveRateGroupOutputPorts::PassiveRateGroupOutputPorts] = {};

// A number of constants are needed for construction of the topology. These are specified here.
{{"enum TopologyConstants {"}}
{%- if cookiecutter.file_system_type in ["SD_Card", "MicroFS"] %}
    FILE_DOWNLINK_TIMEOUT = 1000,
    FILE_DOWNLINK_COOLDOWN = 1000,
    FILE_DOWNLINK_CYCLE_TIME = 1000,
    FILE_DOWNLINK_FILE_QUEUE_DEPTH = 10,
    // Buffer manager for Uplink/Downlink
    COMMS_BUFFER_MANAGER_FILE_STORE_SIZE = 3000,
    COMMS_BUFFER_MANAGER_FILE_QUEUE_SIZE = 30,
{%- endif %}
{%- if cookiecutter.com_driver_type == "UART" %}
    COMMS_BUFFER_MANAGER_STORE_SIZE = 140,
{%- else %}
    COMMS_BUFFER_MANAGER_STORE_SIZE = 2048,
{%- endif %}
    COMMS_BUFFER_MANAGER_STORE_COUNT = 3,
    COMMS_BUFFER_MANAGER_ID = 200,
{{"};"}}

/**
 * \brief configure/setup components in project-specific way
 *
 * This is a *helper* function which configures/sets up each component requiring project specific input. This includes
 * allocating resources, passing-in arguments, etc. This function may be inlined into the topology setup function if
 * desired, but is extracted here for clarity.
 */
void configureTopology() {
    // Rate group driver needs a divisor list
    rateGroupDriver.configure(rateGroupDivisors);

    // Rate groups require context arrays.
    rateGroup1.configure(rateGroup1Context, FW_NUM_ARRAY_ELEMENTS(rateGroup1Context));

{% if cookiecutter.file_system_type == "MicroFS" %}
    // Initialize the RAM File system
    Os::Baremetal::MicroFs::MicroFsConfig microFsCfg;
    Os::Baremetal::MicroFs::MicroFsSetCfgBins(microFsCfg, 5);
    Os::Baremetal::MicroFs::MicroFsAddBin(microFsCfg, 0, 1 * 1024, 5);
    Os::Baremetal::MicroFs::MicroFsAddBin(microFsCfg, 1, 10 * 1024, 5);
    Os::Baremetal::MicroFs::MicroFsAddBin(microFsCfg, 2, 10 * 1024, 5);
    Os::Baremetal::MicroFs::MicroFsAddBin(microFsCfg, 3, 10 * 1024, 5);
    Os::Baremetal::MicroFs::MicroFsAddBin(microFsCfg, 4, 10 * 1024, 5);
    Os::Baremetal::MicroFs::MicroFsInit(microFsCfg, 0, mallocator);
{%- endif %}

    // Set up BufferManager
    Svc::BufferManager::BufferBins buffMgrBins;
    memset(&buffMgrBins, 0, sizeof(buffMgrBins));
    buffMgrBins.bins[0].bufferSize = COMMS_BUFFER_MANAGER_STORE_SIZE;
    buffMgrBins.bins[0].numBuffers = COMMS_BUFFER_MANAGER_STORE_COUNT;
{%- if cookiecutter.file_system_type in ["SD_Card", "MicroFS"] %}
    buffMgrBins.bins[1].bufferSize = COMMS_BUFFER_MANAGER_FILE_STORE_SIZE;
    buffMgrBins.bins[1].numBuffers = COMMS_BUFFER_MANAGER_FILE_QUEUE_SIZE;
{%- endif %}
    bufferManager.setup(BUFFER_MANAGER_ID, 0, mallocator, buffMgrBins);

    // FprimeFrameDetector is used to configure the FrameAccumulator to detect F Prime frames
    frameAccumulator.configure(frameDetector, 1, mallocator, 2048);

{% if cookiecutter.file_system_type in ["SD_Card", "MicroFS"] %}
    // File downlink requires some project-derived properties.
    fileDownlink.configure(FILE_DOWNLINK_TIMEOUT, FILE_DOWNLINK_COOLDOWN, FILE_DOWNLINK_CYCLE_TIME,
                           FILE_DOWNLINK_FILE_QUEUE_DEPTH);
{%- endif %}

{% if cookiecutter.file_system_type == "MicroFS" %}
    // Parameter database is configured with a database file name, and that file must be initially read.
    prmDb.configure("/bin4/file1");
    prmDb.readParamFile();
{%- elif cookiecutter.file_system_type == "SD_Card" %}
    // Parameter database is configured with a database file name, and that file must be initially read.
    prmDb.configure("prmDb.dat");
    prmDb.readParamFile();
{%- endif %}

    // Note: Uncomment when using Svc:TlmPacketizer
    // tlmSend.setPacketList(LedBlinkerPacketsPkts, LedBlinkerPacketsIgnore, 1);

    // Events (highest-priority)
    configurationTable.entries[0] = {.depth = 10, .priority = 0};
    // Telemetry
    configurationTable.entries[1] = {.depth = 25, .priority = 2};
    // File Downlink
    configurationTable.entries[2] = {.depth = 1, .priority = 1};
    // Allocation identifier is 0 as the MallocAllocator discards it
    comQueue.configure(configurationTable, 0, mallocator);
}

// Public functions for use in main program are namespaced with deployment name {{cookiecutter.deployment_name}}
namespace {{cookiecutter.deployment_name}} {
void setupTopology(const TopologyState& state) {
    // Autocoded initialization. Function provided by autocoder.
    initComponents(state);
    // Autocoded id setup. Function provided by autocoder.
    setBaseIds();
    // Autocoded connection wiring. Function provided by autocoder.
    connectComponents();
    // Autocoded configuration. Function provided by autocoder.
    configComponents(state);
    // Project-specific component configuration. Function provided above. May be inlined, if desired.
    configureTopology();
    // Autocoded command registration. Function provided by autocoder.
    regCommands();
    // Autocoded parameter loading. Function provided by autocoder.
{%- if cookiecutter.file_system_type in ["SD_Card", "MicroFS"] %}
    loadParameters();
{%- else %}
    // DISABLED FOR ARDUINO BOARDS. Loading parameters are not supported because there is typically no file system.
    // loadParameters();
{%- endif %}
    // Autocoded task kick-off (active components). Function provided by autocoder.
    startTasks(state);

{% if cookiecutter.com_driver_type == "UART" %}
    commDriver.configure(&Serial);
{%- elif cookiecutter.com_driver_type == "TcpServer" %}
    Arduino::SocketIpStatus stat = commDriver.configure("SSID", "PASSWORD", 50000);
    if (stat != Arduino::SocketIpStatus::SOCK_SUCCESS) {
        Fw::Logger::log("[commDriver] Failed to connect to network\n");
    }
{%- elif cookiecutter.com_driver_type == "TcpClient" %}
    Arduino::SocketIpStatus stat = commDriver.configure("SSID", "PASSWORD", "IP_ADDRESS", 50000);
    if (stat != Arduino::SocketIpStatus::SOCK_SUCCESS) {
        Fw::Logger::log("[commDriver] Failed to connect to network\n");
    }
{%- endif %}
    
    rateDriver.configure(1);
    rateDriver.start();
}

void teardownTopology(const TopologyState& state) {
    // Autocoded (active component) task clean-up. Functions provided by topology autocoder.
    stopTasks(state);
    freeThreads(state);
}
};  // namespace {{cookiecutter.deployment_name}}
