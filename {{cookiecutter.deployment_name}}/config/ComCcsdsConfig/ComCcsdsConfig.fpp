module ComCcsdsConfig {
    #Base ID for the ComCcsds Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x02000000
    
    module QueueSizes {
        constant comQueue    = 10
        constant aggregator  = 3
    }
    
    module StackSizes {
        constant comQueue   = 64 * 1024
        constant aggregator = 64 * 1024
    }

    module Priorities {
        constant aggregator = 30
        constant comQueue   = 101
    }

    module CpuAffinities {
        constant aggregator = Os.TASK_DEFAULT
        constant comQueue   = Os.TASK_DEFAULT
    }

    # Queue configuration constants
    module QueueDepths {
        constant events      = 10             
        constant tlm         = 25            
        constant file        = 1            
    }

    module QueuePriorities {
        constant events      = 0                 
        constant tlm         = 2                 
        constant file        = 1                   
    }

    # Aggregator configuration constants
    module Aggregator {
        constant aggregationSize = Svc.Ccsds.TmDataFieldSize
        constant enablePacketSpanning = false
    }

    # Buffer management constants
    module BuffMgr {
        constant frameAccumulatorSize  = 2048     
{%- if cookiecutter.file_system_type in ["SD_Card", "MicroFS"] %}
        # File uplink and downlink frames (256-byte GDS chunks, 255-byte downlink packets) need larger buffers
        constant commsBuffSize         = 512      
        constant commsFileBuffSize     = 512      
{%- else %}
        constant commsBuffSize         = 140      
        constant commsFileBuffSize     = 140      
{%- endif %}
        constant commsBuffCount        = 3        
        constant commsFileBuffCount    = 3       
        constant commsBuffMgrId        = 200      
    }
}
