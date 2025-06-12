/*
 * CmdDispatcherImplCfg.hpp
 *
 *  Created on: May 6, 2015
 *      Author: tcanham
 */

#ifndef CMDDISPATCHER_COMMANDDISPATCHERIMPLCFG_HPP_
#define CMDDISPATCHER_COMMANDDISPATCHERIMPLCFG_HPP_

// Define configuration values for dispatcher

{{enum {}}
{%- if cookiecutter.file_system_type in ["SD_Card", "MicroFS"] %}
    CMD_DISPATCHER_DISPATCH_TABLE_SIZE = 23, // !< The size of the table holding opcodes to dispatch
{%- else %}
    CMD_DISPATCHER_DISPATCH_TABLE_SIZE = 12, // !< The size of the table holding opcodes to dispatch
{%- endif %}
    CMD_DISPATCHER_SEQUENCER_TABLE_SIZE = 4, // !< The size of the table holding commands in progress
{{};}}



#endif /* CMDDISPATCHER_COMMANDDISPATCHERIMPLCFG_HPP_ */
