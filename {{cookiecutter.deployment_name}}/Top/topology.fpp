module {{cookiecutter.deployment_namespace}} {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

    enum Ports_RateGroups {
      rateGroup1
    }

  deployment topology {{cookiecutter.deployment_name}} {

    # ----------------------------------------------------------------------
    # Subtopology imports
    # ----------------------------------------------------------------------

{% if cookiecutter.framing_selection == "CCSDS" %}
    import ComCcsds.Subtopology
{%- else %}
    import ComFprime.Subtopology
{%- endif %}

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance cmdDisp
    instance comDriver
    instance eventLogger
    instance fatalHandler
{%- if cookiecutter.file_system_type in ["SD_Card", "MicroFS"] %}
    instance fileDownlink
    instance fileManager
    instance fileUplink
    instance prmDb
{%- endif %}
    instance rateDriver
    instance rateGroup1
    instance rateGroupDriver
    instance systemResources
    instance textLogger
    instance timeHandler
    instance tlmSend

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance cmdDisp

    event connections instance eventLogger
{%- if cookiecutter.file_system_type in ["SD_Card", "MicroFS"] %}

    param connections instance prmDb
{%- endif %}

    telemetry connections instance tlmSend

    text event connections instance textLogger

    time connections instance timeHandler

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections RateGroups {
      # Block driver
      rateDriver.CycleOut -> rateGroupDriver.CycleIn

      # Rate group 1
      rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup1] -> rateGroup1.CycleIn
      rateGroup1.RateGroupMemberOut[0] -> tlmSend.Run
      rateGroup1.RateGroupMemberOut[1] -> systemResources.run
      rateGroup1.RateGroupMemberOut[2] -> comDriver.schedIn
      rateGroup1.RateGroupMemberOut[3] -> cmdDisp.run
{%- if cookiecutter.file_system_type in ["SD_Card", "MicroFS"] %}
      rateGroup1.RateGroupMemberOut[4] -> fileDownlink.Run
{%- endif %}
{%- if cookiecutter.framing_selection == "CCSDS" %}
      # Send partly filled TM frames instead of waiting for them to fill
      rateGroup1.RateGroupMemberOut[{{ 5 if cookiecutter.file_system_type in ["SD_Card", "MicroFS"] else 4 }}] -> ComCcsds.aggregator.timeout
{%- endif %}
    }

    connections FaultProtection {
      eventLogger.FatalAnnounce -> fatalHandler.FatalReceive
    }

{% if cookiecutter.framing_selection == "CCSDS" %}
    connections Communications {
      # Inputs to ComQueue (events, telemetry, file)
      eventLogger.PktSend -> ComCcsds.comQueue.comPacketQueueIn[ComCcsds.Ports_ComPacketQueue.EVENTS]
      tlmSend.PktSend     -> ComCcsds.comQueue.comPacketQueueIn[ComCcsds.Ports_ComPacketQueue.TELEMETRY]

      # ComDriver buffer allocations
      comDriver.allocate      -> ComCcsds.commsBufferManager.bufferGetCallee
      comDriver.deallocate    -> ComCcsds.commsBufferManager.bufferSendIn
      
      # ComDriver <-> ComStub (Uplink)
      comDriver.$recv                     -> ComCcsds.comStub.drvReceiveIn
      ComCcsds.comStub.drvReceiveReturnOut -> comDriver.recvReturnIn
      
      # ComStub <-> ComDriver (Downlink)
      ComCcsds.comStub.drvSendOut      -> comDriver.$send
      comDriver.ready         -> ComCcsds.comStub.drvConnected

      # Router <-> CmdDispatcher
      ComCcsds.fprimeRouter.commandOut  -> cmdDisp.seqCmdBuff
      cmdDisp.seqCmdStatus     -> ComCcsds.fprimeRouter.cmdResponseIn
{%- if cookiecutter.file_system_type in ["SD_Card", "MicroFS"] %}

      # FileDownlink <-> ComQueue
      fileDownlink.bufferSendOut -> ComCcsds.comQueue.bufferQueueIn[ComCcsds.Ports_ComBufferQueue.FILE]
      ComCcsds.comQueue.bufferReturnOut[ComCcsds.Ports_ComBufferQueue.FILE] -> fileDownlink.bufferReturn

      # Router <-> FileUplink
      ComCcsds.fprimeRouter.fileOut     -> fileUplink.bufferSendIn
      fileUplink.bufferSendOut -> ComCcsds.fprimeRouter.fileBufferReturnIn
{%- endif %}
    }
{%- else %}
    connections Communications {
      # Inputs to ComQueue (events, telemetry, file)
      eventLogger.PktSend -> ComFprime.comQueue.comPacketQueueIn[ComFprime.Ports_ComPacketQueue.EVENTS]
      tlmSend.PktSend     -> ComFprime.comQueue.comPacketQueueIn[ComFprime.Ports_ComPacketQueue.TELEMETRY]

      # ComDriver buffer allocations
      comDriver.allocate      -> ComFprime.commsBufferManager.bufferGetCallee
      comDriver.deallocate    -> ComFprime.commsBufferManager.bufferSendIn
      
      # ComDriver <-> ComStub (Uplink)
      comDriver.$recv                     -> ComFprime.comStub.drvReceiveIn
      ComFprime.comStub.drvReceiveReturnOut -> comDriver.recvReturnIn
      
      # ComStub <-> ComDriver (Downlink)
      ComFprime.comStub.drvSendOut      -> comDriver.$send
      comDriver.ready         -> ComFprime.comStub.drvConnected

      # Router <-> CmdDispatcher
      ComFprime.fprimeRouter.commandOut  -> cmdDisp.seqCmdBuff
      cmdDisp.seqCmdStatus     -> ComFprime.fprimeRouter.cmdResponseIn
{%- if cookiecutter.file_system_type in ["SD_Card", "MicroFS"] %}

      # FileDownlink <-> ComQueue
      fileDownlink.bufferSendOut -> ComFprime.comQueue.bufferQueueIn[ComFprime.Ports_ComBufferQueue.FILE]
      ComFprime.comQueue.bufferReturnOut[ComFprime.Ports_ComBufferQueue.FILE] -> fileDownlink.bufferReturn

      # Router <-> FileUplink
      ComFprime.fprimeRouter.fileOut     -> fileUplink.bufferSendIn
      fileUplink.bufferSendOut -> ComFprime.fprimeRouter.fileBufferReturnIn
{%- endif %}
    }
{%- endif %}

    connections {{cookiecutter.deployment_name}} {
      # Add here connections to user-defined components
    }

  }

}
