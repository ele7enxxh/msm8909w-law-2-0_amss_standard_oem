
/*! @defgroup a2_core_taskq A2 Common Taskq Functionality

  The files interact below as shown in the diagram.

  @image html a2_core_taskq_overview.jpg

  <table>
    <tr>
      <td>No.</td>
      <td>Interaction</td>
      <td>Description</td>
      <td>Interface</td>
    </tr>
    <tr>
      <td>1.</td>
      <td>Task Writer -> a2_common_tasks.c</td>
      <td>Write tasks (eg. DMA, Fill, Cipher etc)</td>
      <td>a2_common_tasks.h</td>
    </tr>
    <tr>
      <td>2.</td>
      <td>a2_common_tasks.c -> a2_taskq.c</td>
      <td>Write task bytestream to taskq, commit write pointer, update rd ptr.</td>
      <td>@ref a2_taskq</td>
    </tr>
    <tr>
      <td>3.</td>
      <td>a2_taskq.c -> a2_taskq_hal.c</td>
      <td>Update write pointer, read rd pointer</td>
      <td>a2_taskq_hal.h</td>
    </tr>
    <tr>
      <td>4.</td>
      <td>a2_taskq_hal.c -> A2 HW </td>
      <td>Write write pointer, read read pointer registers</td>
      <td>HW SWI</td>
    </tr>
    <tr>
      <td>5.</td>
      <td>a2_taskq.c -> a2_taskq_data.c</td>
      <td>Taskq uses the buffers defined in a2_taskq_data.c, following initialization.</td>
      <td>No direct interface.</td>
    </tr>
    <tr>
      <td>6.</td>
      <td>Task Writer -> a2_taskq.c</td>
      <td>Update read and write pointers to/from hardware.</td>
      <td>@ref a2_taskq</td>
    </tr>
    <tr>
      <td>7.</td>
      <td>Status Queue Reader -> a2_taskq.c</td>
      <td>@copybrief a2_status_taskq</td>
      <td>@ref a2_status_taskq </td>
    </tr>
    <tr>
      <td>8.</td>
      <td>Status Queue Reader -> a2_taskq.c</td>
      <td>Update read pointer, read write pointer</td>
      <td>a2_taskq_hal.h</td>
    </tr>
    <tr>
      <td>9.</td>
      <td>Status TaskQ Interpreter-> a2_status_util.c</td>
      <td>Build DSM chain</td>
      <td>a2_status_util.h</td>
    </tr>

  </table>

  */

/*!
  @defgroup per_implementation Peripheral A2 Driver Implementation

  @brief 
  The files which implement the downlink and uplink peripheral portions
  of the a2 driver.

  The files interact below as shown in the diagram.

  @image html a2_per_code_overview.jpg

  <table>
    <tr>
      <td>No.</td>
      <td>Interaction</td>
      <td>Description</td>
      <td>Interface</td>
    </tr>
    <tr>
      <td>1.</td>
      <td> a2_sio.c -> a2_dl_per.c </td>
      <td> Sends messages to the a2 task via a2_dl_per.c </td>
      <td> @ref dl_per_ext_task</td>
    </tr>
    <tr>
      <td>2.</td>
      <td> a2_dl_per.c -> a2_task.c </td>
      <td> Send configure/notify messages to a2 task.</td>
      <td> @ref a2_task_messages</td>
    </tr>
    <tr>
      <td>3.</td>
      <td> a2_task.c -> a2_dl_per.c </td>
      <td> A2 task commands the a2_dl_per.c module to perform actions.  (eg send SIO/PDCP data, bridge stream, commit filter rules etc.)</td>
      <td> @ref dl_per_int_task</td>
    </tr>
    <tr>
      <td>4.</td>
      <td> a2_dl_per.c -> @ref a2_core_taskq </td>
      <td> Write tasks and commit tasks to the hardware.</td>
      <td> @ref a2_core_taskq </td>
    </tr>
    <tr>
      <td>5.</td>
      <td> @ref a2_core_taskq-> A2 HW </td>
      <td> Reads/writes task queue HW registers.</td>
      <td> A2 HW SWI</td>
    </tr>
    <tr>
      <td>6.</td>
      <td> A2 HW -> @ref a2_core_taskq </td>
      <td> Reads/writes task queue HW registers.</td>
      <td> A2 HW SWI</td>
    </tr>
    <tr>
      <td>7.</td>
      <td> @ref a2_core_taskq -> a2_dl_per.c </td>
      <td> @ref dl_per_status_processing</td>
      <td> @ref dl_per_status_processing</td>
    </tr>
    <tr>
      <td>8.</td>
      <td> a2_dl_per.c -> a2_task.c </td>
      <td> Status queue processing notifies the A2 task when certain tasks are processed.</td>
      <td> @ref a2_task_messages, @ref a2_task_signals</td>
    </tr>
    <tr>
      <td>9.</td>
      <td> a2_task.c -> a2_sio.c </td>
      <td> Flush complete callback</td>
      <td> a2_sio.h</td>
    </tr>
    <tr>
      <td>10.</td>
      <td> a2_sio.c -> a2_ul_per.c </td>
      <td> Configures the ul_per block (ppp, flow enable, register data callback)</td>
      <td> a2_ul_peri.h</td>
    </tr>
    <tr>
      <td>11.</td>
      <td> a2_ul_per.c -> @ref a2_core_taskq </td>
      <td> Writes configuration tasks to the task queue</td>
      <td> a2_common_tasks.h</td>
    </tr>
    <tr>
      <td>12.</td>
      <td> @ref a2_core_taskq -> a2_ul_per.c </td>
      <td> Reads tasks from status task queue</td>
      <td>@ref a2_status_taskq </td>
    </tr>
    <tr>
      <td>13.</td>
      <td> a2_ul_per.c -> a2_sio.c </td>
      <td> Notify A2 SIO that data has arrived so data can be enqueued into the appropriate watermark.</td>
      <td> @ref a2_ul_per_set_data_cb</td>
    </tr>
    <tr>
      <td>14.</td>
      <td> PDCP/DS -> a2_dl_per.c </td>
      <td> Commonly known as the Epsid interface.  Includes configuration of the data flowing from PDCP, as well as sending PDCP data.</td>
      <td> a2_dl_per.h</td>
    </tr>
    <tr>
      <td>15.</td>
      <td> PS -> a2_ipfilter.c </td>
      <td> @copybrief a2_ipfilter.h</td>
      <td> a2_ipfilter.h</td>
    </tr>
    <tr>
      <td>16.</td>
      <td> a2_ipfilter.c -> a2_dl_per.c </td>
      <td> Notifies the task that there is an update in the ipfilter rules.</td>
      <td> @ref dl_per_ext_task</td>
    </tr>
    <tr>
      <td>17.</td>
      <td> a2_dl_per.c -> a2_ipfilter.c </td>
      <td> @copybrief a2_ipfilteri.h </td>
      <td> a2_ipfilteri.h </td>
    </tr>
  </table>

  */

/*! 
  @page algorithms Algorithms

  @section dl_per_dequeing_algo Downlink Peripheral Dequeuing Algorithm

  @subsection dl_per_dequeuing_algo_data_structures Data Structures

  Queue of watermarks that have pending data to send

  @subsection dl_per_dequeuing_algo_input Input:

  PDCP/PS ->a2 task notification that there is data on a particular watermark.  

  @subsection dl_per_dequeuing_algo_description Algorithm Description

  - Upon receiving a data indication for a watermark, that watermark is added 
    to the pending queue of watermarks (if not already in the queue).
  - After the messages for the task are processed the task processes the 
    watermarks in the pending queue until the queue is filled.  
    The message queue is checked after each watermark to ensure no starvation 
    of the message queue occurs.
    - Whenever a watermark empties, it is removed from the pending watermarks 
      queue.
    - Every n packets the driver places an IRQ task causing the driver to 
      process the status periodically.

  - Status queue processing will begin whenever a dl-per interrupt fires.  
    This will Upon Status-Processing:
    - Notify the driver that there is space available and that it can continue, if there is anything pending.
    - Free transmitted dsm items.

  @subsection dl_per_dequeuing_algo_notes Notes

  - Since we place irq tasks every x packets, the a2 driver will never leave 
    more than x dsm item chains unfreed, after the driver goes idle.
  - HW read pointer is refreshed only upon queue becoming full.  
    This should minimize the number of read pointer reads.

  @subsection dl_per_dequeuing_algo_example Example
  
  @image html a2_per_watermark_deq_algo.jpg

  @section dl_per_ipfilter_ram_update IP Filter RAM Update Algorithm

  @subsection dl_per_ipfilter_ram_update_description Description
  To support the updation of ipfilter RAM, the A2 driver uses external memory
  mirrors of the internal hardware RAM.  When the RAM is prepared it is marked
  as ready to commit and the task is notified that an update to hardware RAM 
  update needs to be performed.  The A2 task will get the most recent buffer
  from ipfilter module.  That buffer will be marked as reserved, and the other
  buffer will need to be used for any other filter updates while the set of
  rules is being applied.
  
  If there is a pending ipfilter commit or the taskq is full when the task is
  requested to commit the filter rules the a2 task will mark the rule update
  pending and will commit the changes when possible.

  @subsection dl_per_ipfilter_ram_update_example Example

  @image html a2_ipfilter_ram_update.jpg

  @section dl_per_taskq_programming_strategy DL PER Taskq Programming Strategy
  
  The DL PER taskq is programmed as other task queues, with a few exceptions.
  The DL PER taskq is expected to fill up, since the amount of data that is
  sent from the protocol stack can be very large and bursty.  To deal with this
  each operation (sending an ip packet, ip filterram update) has to check to 
  see all the tasks for that operation can be programmed prior to performing 
  that operation.  Also enough extra space is reserved for a final irq and 
  tag task, so that the task can be notified that the queue is no longer full.
  
  IRQ tasks are also placed periodically every "n" ip packets.  These are 
  placed to ensure that the status is processed and no more than "n" dsm chains
  are sitting in the task queue waiting to be freed.  IRQ tasks need to be 
  placed after any task where the status needs to be processed for it to 
  complete.
  
  The taskq read pointer is only updated when the taskq appears full to the 
  driver.  The driver will read the pointer one time, before deciding whether
  the taskq is truly full.

  */

/*! 
  @defgroup a2_ext_interface A2 External Interface

  This group defines the external interface to the A2 driver.

 */

/*!
  @defgroup a2_internal_doc A2 Internal Documentation

  This section describes the internal A2 documentation and is broken down 
  into several categories.

  - @ref features
  - @ref per_implementation
  - @ref a2_core_taskq
  - @subpage algorithms
  */
/*!
  @mainpage

  - @ref a2_ext_interface
  - @ref a2_internal_doc

  */
