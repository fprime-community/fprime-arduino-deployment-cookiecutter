module ComFprimeConfig {
    #Base ID for the ComFprime Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x03000000
    
    module QueueSizes {
        constant comQueue    = 10
    }
    
    module StackSizes {
        constant comQueue   = 64 * 1024
    }

    module Priorities {
        constant comQueue   = 101
    }

    module CpuAffinities {
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

    # Buffer management constants
    module BuffMgr {
{%- if cookiecutter.com_driver_type == "UART" and cookiecutter.file_system_type == "None" %}
        constant frameAccumulatorSize  = 256     
        constant commsBuffSize         = 140      
        constant commsFileBuffSize     = 0      
        constant commsBuffCount        = 3      
        constant commsFileBuffCount    = 0        
{%- elif cookiecutter.file_system_type in ["SD_Card", "MicroFS"] %}
        # File uplink and downlink frames (256-byte GDS chunks, 255-byte downlink packets) need larger buffers
        constant frameAccumulatorSize  = 2048     
        constant commsBuffSize         = 512      
        constant commsFileBuffSize     = 512      
        constant commsBuffCount        = 3      
        constant commsFileBuffCount    = 3        
{%- else %}
        constant frameAccumulatorSize  = 2048     
        constant commsBuffSize         = 140      
        constant commsFileBuffSize     = 140      
        constant commsBuffCount        = 3      
        constant commsFileBuffCount    = 3        
{%- endif %}
        constant commsBuffMgrId        = 200      
    }
}
