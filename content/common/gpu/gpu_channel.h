// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_GPU_CHANNEL_H_
#define CONTENT_COMMON_GPU_GPU_CHANNEL_H_

#include <string>

#include "base/containers/hash_tables.h"
#include "base/containers/scoped_ptr_hash_map.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/process/process.h"
#include "base/trace_event/memory_dump_provider.h"
#include "build/build_config.h"
#include "content/common/content_export.h"
#include "content/common/gpu/gpu_command_buffer_stub.h"
#include "content/common/gpu/gpu_memory_manager.h"
#include "content/common/gpu/gpu_result_codes.h"
#include "content/common/gpu/gpu_stream_priority.h"
#include "content/common/message_router.h"
#include "gpu/command_buffer/service/valuebuffer_manager.h"
#include "ipc/ipc_sync_channel.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gl/gl_share_group.h"
#include "ui/gl/gpu_preference.h"

#include "ui/opencl/opencl_include.h"
struct GPUCreateCommandBufferConfig;

namespace base {
class WaitableEvent;
}

namespace gpu {
class PreemptionFlag;
class SyncPointOrderData;
class SyncPointManager;
union ValueState;
class ValueStateMap;
namespace gles2 {
class SubscriptionRefSet;
}
}

namespace gfx {
class CLApi;
}

namespace IPC {
class MessageFilter;
}

namespace content {
class GpuChannelManager;
class GpuChannelMessageFilter;
class GpuChannelMessageQueue;
class GpuJpegDecodeAccelerator;
class GpuWatchdog;

// Encapsulates an IPC channel between the GPU process and one renderer
// process. On the renderer side there's a corresponding GpuChannelHost.
class CONTENT_EXPORT GpuChannel
    : public IPC::Listener,
      public IPC::Sender,
      public gpu::gles2::SubscriptionRefSet::Observer {
 public:
  // Takes ownership of the renderer process handle.
  GpuChannel(GpuChannelManager* gpu_channel_manager,
             gpu::SyncPointManager* sync_point_manager,
             GpuWatchdog* watchdog,
             gfx::GLShareGroup* share_group,
             gpu::gles2::MailboxManager* mailbox_manager,
             gpu::PreemptionFlag* preempting_flag,
             base::SingleThreadTaskRunner* task_runner,
             base::SingleThreadTaskRunner* io_task_runner,
             int client_id,
             uint64_t client_tracing_id,
             bool allow_future_sync_points,
             bool allow_real_time_streams);
  ~GpuChannel() override;

  // Initializes the IPC channel. Caller takes ownership of the client FD in
  // the returned handle and is responsible for closing it.
  virtual IPC::ChannelHandle Init(base::WaitableEvent* shutdown_event);

  // Get the GpuChannelManager that owns this channel.
  GpuChannelManager* gpu_channel_manager() const {
    return gpu_channel_manager_;
  }

  const std::string& channel_id() const { return channel_id_; }

  virtual base::ProcessId GetClientPID() const;

  int client_id() const { return client_id_; }

  uint64_t client_tracing_id() const { return client_tracing_id_; }

  scoped_refptr<base::SingleThreadTaskRunner> io_task_runner() const {
    return io_task_runner_;
  }

  // IPC::Listener implementation:
  bool OnMessageReceived(const IPC::Message& msg) override;
  void OnChannelError() override;

  // IPC::Sender implementation:
  bool Send(IPC::Message* msg) override;

  // SubscriptionRefSet::Observer implementation
  void OnAddSubscription(unsigned int target) override;
  void OnRemoveSubscription(unsigned int target) override;

  // This is called when a command buffer transitions between scheduled and
  // descheduled states. When any stub is descheduled, we stop preempting
  // other channels.
  void OnStubSchedulingChanged(GpuCommandBufferStub* stub, bool scheduled);

  CreateCommandBufferResult CreateViewCommandBuffer(
      const gfx::GLSurfaceHandle& window,
      const GPUCreateCommandBufferConfig& init_params,
      int32 route_id);

  gfx::GLShareGroup* share_group() const { return share_group_.get(); }

  GpuCommandBufferStub* LookupCommandBuffer(int32 route_id);
  unsigned int LookupGLServiceId(unsigned int resource_id, GLResourceType glResourceType);

  void LoseAllContexts();
  void MarkAllContextsLost();

  // Called to add a listener for a particular message routing ID.
  // Returns true if succeeded.
  bool AddRoute(int32 route_id, IPC::Listener* listener);

  // Called to remove a listener for a particular message routing ID.
  void RemoveRoute(int32 route_id);

  void SetPreemptingFlag(gpu::PreemptionFlag* flag);

  // If |preemption_flag->IsSet()|, any stub on this channel
  // should stop issuing GL commands. Setting this to NULL stops deferral.
  void SetPreemptByFlag(
      scoped_refptr<gpu::PreemptionFlag> preemption_flag);

  void CacheShader(const std::string& key, const std::string& shader);

  void AddFilter(IPC::MessageFilter* filter);
  void RemoveFilter(IPC::MessageFilter* filter);

  uint64 GetMemoryUsage();

  scoped_refptr<gl::GLImage> CreateImageForGpuMemoryBuffer(
      const gfx::GpuMemoryBufferHandle& handle,
      const gfx::Size& size,
      gfx::BufferFormat format,
      uint32 internalformat);

  bool allow_future_sync_points() const { return allow_future_sync_points_; }

  void HandleUpdateValueState(unsigned int target,
                              const gpu::ValueState& state);

  // Visible for testing.
  const gpu::ValueStateMap* pending_valuebuffer_state() const {
    return pending_valuebuffer_state_.get();
  }

  // Visible for testing.
  GpuChannelMessageFilter* filter() const { return filter_.get(); }

  // Returns the global order number for the last processed IPC message.
  uint32_t GetProcessedOrderNum() const;

  // Returns the global order number for the last unprocessed IPC message.
  uint32_t GetUnprocessedOrderNum() const;

  // Returns the shared sync point global order data.
  scoped_refptr<gpu::SyncPointOrderData> GetSyncPointOrderData();

  void HandleMessage();

  // Some messages such as WaitForGetOffsetInRange and WaitForTokenInRange are
  // processed as soon as possible because the client is blocked until they
  // are completed.
  void HandleOutOfOrderMessage(const IPC::Message& msg);

#if defined(OS_ANDROID)
  const GpuCommandBufferStub* GetOneStub() const;
#endif

 protected:
  // The message filter on the io thread.
  scoped_refptr<GpuChannelMessageFilter> filter_;

  // Map of routing id to command buffer stub.
  base::ScopedPtrHashMap<int32, scoped_ptr<GpuCommandBufferStub>> stubs_;

 private:
  class StreamState {
   public:
    StreamState(int32 id, GpuStreamPriority priority);
    ~StreamState();

    int32 id() const { return id_; }
    GpuStreamPriority priority() const { return priority_; }

    void AddRoute(int32 route_id);
    void RemoveRoute(int32 route_id);
    bool HasRoute(int32 route_id) const;
    bool HasRoutes() const;

   private:
    int32 id_;
    GpuStreamPriority priority_;
    base::hash_set<int32> routes_;
  };

  void OnDestroy();

  bool OnControlMessageReceived(const IPC::Message& msg);

  void ScheduleHandleMessage();

  void OnCallclGetPlatformIDs(
		    const cl_uint& num_entries,
		    const std::vector<bool>& return_variable_null_status,
		    std::vector<cl_point>* point_platform_list,
		    cl_uint* num_platforms,
		    cl_int* errcode_ret);

  void OnCallclGetPlatformInfo(
		cl_point platform,
		cl_platform_info param_name,
		size_t param_value_size,
		std::vector<bool> null_param_status,
		cl_int* errcode_ret,
		std::string* param_value,
		size_t* param_value_size_ret);

  void OnCallclGetDeviceIDs(
      const cl_point&,
      const cl_device_type&,
      const cl_uint&,
      const std::vector<bool>&,
      std::vector<cl_point>*,
      cl_uint*,
      cl_int*);

  void OnCallclGetDeviceInfo_string(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      std::string*,
      size_t*,
      cl_int*);
  void OnCallclGetDeviceInfo_cl_uint(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_uint*,
      size_t*,
      cl_int*);

  void OnCallclGetDeviceInfo_size_t_list(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      std::vector<size_t>*,
      size_t*,
      cl_int*);

  void OnCallclGetDeviceInfo_size_t(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      size_t*,
      size_t*,
      cl_int*);

  void OnCallclGetDeviceInfo_cl_ulong(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_ulong*,
      size_t*,
      cl_int*);

  void OnCallclGetDeviceInfo_cl_point(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_point*,
      size_t*,
      cl_int*);

  void OnCallclGetDeviceInfo_intptr_t_list(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      std::vector<intptr_t>*,
      size_t*,
      cl_int*);

  void OnCallclCreateContextFromType(
      const std::vector<cl_context_properties>&,
      const cl_device_type&,
      const cl_point&,
      const cl_point&,
      const std::vector<bool>&,
      cl_int*,
      cl_point*);

  void OnCallclCreateContext(
      const std::vector<cl_context_properties>&,
	  const std::vector<cl_point>&,
      const cl_point&,
      const cl_point&,
      const std::vector<bool>&,
      cl_int*,
      cl_point*);

  void OnCallclWaitForevents(
      const cl_uint&,
      const std::vector<cl_point>&,
      cl_int*);

  void OnCallclGetMemObjectInfo_cl_int(
      const cl_point&,
      const cl_mem_info&,
      const size_t&,
      const std::vector<bool>&,
	  cl_int*,
	  size_t*,
	  cl_int*);

  void OnCallclGetMemObjectInfo_cl_uint(
      const cl_point&,
      const cl_mem_info&,
      const size_t&,
      const std::vector<bool>&,
	  cl_uint*,
	  size_t*,
	  cl_int*);

  void OnCallclGetMemObjectInfo_cl_ulong(
      const cl_point&,
      const cl_mem_info&,
      const size_t&,
      const std::vector<bool>&,
	  cl_ulong*,
	  size_t*,
	  cl_int*);

  void OnCallclGetMemObjectInfo_size_t(
      const cl_point&,
      const cl_mem_info&,
      const size_t&,
      const std::vector<bool>&,
	  size_t*,
	  size_t*,
	  cl_int*);

  void OnCallclGetMemObjectInfo_cl_point(
      const cl_point&,
      const cl_mem_info&,
      const size_t&,
      const std::vector<bool>&,
	  cl_point*,
	  size_t*,
	  cl_int*);

  void OnCallclCreateSubBuffer(
      const cl_point&,
      const cl_mem_flags&,
      const cl_buffer_create_type&,
	  const size_t,
	  const size_t,
	  cl_point*,
	  cl_int*);

	void OnCallclCreateSampler(
		const cl_point&,
		const cl_bool&,
		const cl_addressing_mode&,
		const cl_filter_mode&,
		const std::vector<bool>&,
		cl_int*,
		cl_point*);

	void OnCallclGetSamplerInfo_cl_uint(
		const cl_point&,
		const cl_sampler_info&,
		const size_t&,
		const std::vector<bool>&,
		cl_uint*,
		size_t*,
		cl_int*);

	void OnCallclGetSamplerInfo_cl_point(
		const cl_point&,
		const cl_sampler_info&,
		const size_t&,
		const std::vector<bool>&,
		cl_point*,
		size_t*,
		cl_int*);

	void OnCallclReleaseSampler(
		const cl_point&,
		cl_int*);

  void OnCallclGetImageInfo_cl_int(
      const cl_point&,
      const cl_image_info&,
      const size_t&,
      const std::vector<bool>&,
	  cl_int*,
	  size_t*,
	  cl_int*);

  void OnCallclGetImageInfo_cl_uint_list(
      const cl_point&,
      const cl_image_info&,
      const size_t&,
      const std::vector<bool>&,
	  std::vector<cl_uint>*,
	  size_t*,
	  cl_int*);

  void OnCallclGetImageInfo_size_t(
      const cl_point&,
      const cl_image_info&,
      const size_t&,
      const std::vector<bool>&,
	  size_t*,
	  size_t*,
	  cl_int*);

  void OnCallclGetImageInfo_cl_point(
      const cl_point&,
      const cl_image_info&,
      const size_t&,
      const std::vector<bool>&,
	  cl_point*,
	  size_t*,
	  cl_int*);

	void OnCallclGetEventInfo_cl_point(
		const cl_point& point_event, 
		const cl_event_info& param_name, 
		const size_t& param_value_size,
		const std::vector<bool>& return_variable_null_status,
		cl_point* cl_point_ret, 
		size_t* param_value_size_ret,
		cl_int* errcode_ret);

	void OnCallclGetEventInfo_cl_uint(
		const cl_point& point_event,
		const cl_event_info& param_name, 
		const size_t& param_value_size,
		const std::vector<bool>& return_variable_null_status,
		cl_uint* cl_uint_ret,
		size_t* param_value_size_ret, 
		cl_int* errcode_ret);

	void OnCallclGetEventInfo_cl_int(
		const cl_point& point_event,
		const cl_event_info& param_name,
		const size_t& param_value_size,
		const std::vector<bool>& return_variable_null_status,
		cl_int* cl_int_ret,
		size_t* param_value_size_ret, 
		cl_int* errcode_ret);

	void OnCallclGetEventProfilingInfo_cl_ulong(
		const cl_point& point_event, 
		const cl_profiling_info& param_name,
		const size_t& param_value_size,
		const std::vector<bool>& return_variable_null_status,
		cl_ulong* cl_ulong_ret,
		size_t* param_value_size_ret, 
		cl_int* errcode_ret);

	void OnCallclSetEventCallback(
		const cl_point& point_event,
		const cl_int& command_exec_callback_type,
		const std::vector<int>& key_list,
		cl_int* errcode_ret);

	void OnCallclReleaseEvent(
		const cl_point& point_event,
		cl_int* errcode_ret);

  void OnCallclGetContextInfo_cl_uint(
      const cl_point&,
      const cl_context_info&,
      const size_t&,
      const std::vector<bool>&,
	  cl_uint*,
	  size_t*,
	  cl_int*);

  void OnCallclGetContextInfo_cl_point(
      const cl_point&,
      const cl_context_info&,
      const size_t&,
      const std::vector<bool>&,
	  cl_point*,
	  size_t*,
	  cl_int*);

  void OnCallclGetContextInfo_cl_point_list(
      const cl_point&,
      const cl_context_info&,
      const size_t&,
      const std::vector<bool>&,
	  std::vector<cl_point>*,
	  size_t*,
	  cl_int*);

	void OnCallclSetUserEventStatus(
		const cl_point&,
		const cl_int&,
		cl_int*);

	void OnCallclCreateUserEvent(
		const cl_point&,
		const std::vector<bool>&,
		cl_int*,
		cl_point*);

	void OnCallclGetSupportedImageFormat(
		const cl_point&,
		const cl_mem_flags&,
		const cl_mem_object_type&,
		const cl_uint&,
		const std::vector<bool>&,
		std::vector<cl_uint>*,
		cl_uint*,
		cl_int*);

	void OnCallclReleaseCommon(
		const cl_point&,
		const int,
		cl_int*);

	void OnCallclCreateCommandQueue(
		const cl_point&,
		const cl_point&,
		const cl_command_queue_properties&,
		const std::vector<bool>&,
		cl_int*,
		cl_point*);

	  void OnCallclGetCommandQueueInfo_cl_ulong(
	      const cl_point&,
	      const cl_context_info&,
	      const size_t&,
	      const std::vector<bool>&,
		  cl_ulong*,
		  size_t*,
		  cl_int*);

	  void OnCallclGetCommandQueueInfo_cl_point(
	      const cl_point&,
	      const cl_context_info&,
	      const size_t&,
	      const std::vector<bool>&,
		  cl_point*,
		  size_t*,
		  cl_int*);

	  void OnCallFlush(
			  const cl_point&,
			  cl_int*);

	void OnCallclGetKernelInfo_string(
		const cl_point&,
		const cl_kernel_info&,
		const size_t&,
		const std::vector<bool>&,
		std::string*,
		size_t*,
		cl_int*);

	void OnCallclGetKernelInfo_cl_uint(
		const cl_point&,
		const cl_kernel_info&,
		const size_t&,
		const std::vector<bool>&,
		cl_uint*,
		size_t*,
		cl_int*);

	void OnCallclGetKernelInfo_cl_point(
		const cl_point&,
		const cl_kernel_info&,
		const size_t&,
		const std::vector<bool>&,
		cl_point*,
		size_t*,
		cl_int*);

	void OnCallclGetKernelWorkGroupInfo_size_t_list(
		const cl_point&,
		const cl_point&,
		const cl_kernel_work_group_info&,
		const size_t&,
		const std::vector<bool>&,
		std::vector<size_t>*,
		size_t*,
		cl_int*);

	void OnCallclGetKernelWorkGroupInfo_size_t(
		const cl_point&,
		const cl_point&,
		const cl_kernel_work_group_info&,
		const size_t&,
		const std::vector<bool>&,
		size_t*,
		size_t*,
		cl_int*);

	void OnCallclGetKernelWorkGroupInfo_cl_ulong(
		const cl_point&,
		const cl_point&,
		const cl_kernel_work_group_info&,
		const size_t&,
		const std::vector<bool>&,
		cl_ulong*,
		size_t*,
		cl_int*);

	void OnCallclGetKernelArgInfo_string(
		const cl_point&,
		const cl_uint&,
		const cl_kernel_arg_info&,
		const size_t&,
		const std::vector<bool>&,
		std::string*,
		size_t*,
		cl_int*);

	void OnCallclGetKernelArgInfo_cl_uint(
		const cl_point&,
		const cl_uint&,
		const cl_kernel_arg_info&,
		const size_t&,
		const std::vector<bool>&,
		cl_uint*,
		size_t*,
		cl_int*);

	void OnCallclGetKernelArgInfo_cl_ulong(
		const cl_point&,
		const cl_uint&,
		const cl_kernel_arg_info&,
		const size_t&,
		const std::vector<bool>&,
		cl_ulong*,
		size_t*,
		cl_int*);

	void OnCallclReleaseKernel(
		const cl_point&,
		cl_int*);

	void OnCallclGetProgramInfo_cl_uint(
		const cl_point&,
		const cl_program_info&,
		const size_t&,
		const std::vector<bool>&,
		cl_uint*,
		size_t*,
		cl_int*);

	void OnCallclGetProgramInfo_cl_point(
		const cl_point&,
		const cl_program_info&,
		const size_t&,
		const std::vector<bool>&,
		cl_point*,
		size_t*,
		cl_int*);

	void OnCallclGetProgramInfo_cl_point_list(
		const cl_point&,
		const cl_program_info&,
		const size_t&,
		const std::vector<bool>&,
		std::vector<cl_point>*,
		size_t*,
		cl_int*);

	void OnCallclGetProgramInfo_string(
		const cl_point&,
		const cl_program_info&,
		const size_t&,
		const std::vector<bool>&,
		std::string*,
		size_t*,
		cl_int*);

	void OnCallclGetProgramInfo_size_t_list(
		const cl_point&,
		const cl_program_info&,
		const size_t&,
		const std::vector<bool>&,
		std::vector<size_t>*,
		size_t*,
		cl_int*);

	void OnCallclGetProgramInfo_string_list(
		const cl_point&,
		const cl_program_info&,
		const size_t&,
		const std::vector<bool>&,
		std::vector<std::string>*,
		size_t*,
		cl_int*);

	void OnCallclGetProgramInfo_size_t(
		const cl_point&,
		const cl_program_info&,
		const size_t&,
		const std::vector<bool>&,
		size_t*,
		size_t*,
		cl_int*);

	void OnCallclCreateProgramWithSource(
		const cl_point&,
		const cl_uint&,
		const std::vector<std::string>&,
		const std::vector<size_t>&,
		const std::vector<bool>&,
		cl_int*,
		cl_point*);

	void OnCallclGetProgramBuildInfo_cl_int(
		const cl_point&,
		const cl_point&,
		const cl_program_build_info&,
		const size_t&,
		const std::vector<bool>&,
		cl_int*,
		size_t*,
		cl_int*);

	void OnCallclGetProgramBuildInfo_string(
		const cl_point&,
		const cl_point&,
		const cl_program_build_info&,
		const size_t&,
		const std::vector<bool>&,
		std::string*,
		size_t*,
		cl_int*);

	void OnCallclGetProgramBuildInfo_cl_uint(
		const cl_point&,
		const cl_point&,
		const cl_program_build_info&,
		const size_t&,
		const std::vector<bool>&,
		cl_uint*,
		size_t*,
		cl_int*);

	void OnCallclBuildProgram(
		const cl_point&,
		const cl_uint&,
		const std::vector<cl_point>&,
		const std::string&,
		const std::vector<cl_point>& key_list,
		cl_int*);

	void OnCallclEnqueueMarker(
		const cl_point&,
		cl_point*,
		cl_int*);

	void OnCallclEnqueueBarrier(
		const cl_point&,
		cl_int*);

	void OnCallclEnqueueWaitForEvents(
		const cl_point&,
		const std::vector<cl_point>&,
		const cl_uint&,
		cl_int*);

	void OnCallclCreateKernel(
		const cl_point&,
		const std::string&,
		const std::vector<bool>&,
		cl_int*,
		cl_point*);

	void OnCallclCreateKernelsInProgram(
		const cl_point&,
		const cl_uint&,
		const std::vector<cl_point>&,
		const std::vector<bool>&,
		std::vector<cl_point>*,
		cl_uint*,
		cl_int*);

	void OnCallclReleaseProgram(
		const cl_point&,
		cl_int*);

  // gl/cl sharing
  void OnCallGetGLContext(cl_point*, cl_point*);

	void OnCallCtrlSetSharedHandles(
		const base::SharedMemoryHandle& data_handle,
		const base::SharedMemoryHandle& operation_handle,
		const base::SharedMemoryHandle& result_handle,
		const base::SharedMemoryHandle& events_handle,
		bool* result);

	void OnCallCtrlClearSharedHandles(
		bool* result);

  // Message handlers.
  void OnCreateOffscreenCommandBuffer(
      const gfx::Size& size,
      const GPUCreateCommandBufferConfig& init_params,
      int32 route_id,
      bool* succeeded);
  void OnDestroyCommandBuffer(int32 route_id);
  void OnCreateJpegDecoder(int32 route_id, IPC::Message* reply_msg);

  // The lifetime of objects of this class is managed by a GpuChannelManager.
  // The GpuChannelManager destroy all the GpuChannels that they own when they
  // are destroyed. So a raw pointer is safe.
  GpuChannelManager* gpu_channel_manager_;

  // Sync point manager. Outlives the channel and is guaranteed to outlive the
  // message loop.
  gpu::SyncPointManager* sync_point_manager_;

  scoped_ptr<IPC::SyncChannel> channel_;

  // Uniquely identifies the channel within this GPU process.
  std::string channel_id_;

  // Used to implement message routing functionality to CommandBuffer objects
  MessageRouter router_;

  // Whether the processing of IPCs on this channel is stalled and we should
  // preempt other GpuChannels.
  scoped_refptr<gpu::PreemptionFlag> preempting_flag_;

  // If non-NULL, all stubs on this channel should stop processing GL
  // commands (via their GpuScheduler) when preempted_flag_->IsSet()
  scoped_refptr<gpu::PreemptionFlag> preempted_flag_;

  scoped_refptr<GpuChannelMessageQueue> message_queue_;

  // The id of the client who is on the other side of the channel.
  int client_id_;

  // The tracing ID used for memory allocations associated with this client.
  uint64_t client_tracing_id_;

  // The task runners for the main thread and the io thread.
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  scoped_refptr<base::SingleThreadTaskRunner> io_task_runner_;

  // The share group that all contexts associated with a particular renderer
  // process use.
  scoped_refptr<gfx::GLShareGroup> share_group_;

  scoped_refptr<gpu::gles2::MailboxManager> mailbox_manager_;

  scoped_refptr<gpu::gles2::SubscriptionRefSet> subscription_ref_set_;

  scoped_refptr<gpu::ValueStateMap> pending_valuebuffer_state_;

  scoped_ptr<GpuJpegDecodeAccelerator> jpeg_decoder_;

  gpu::gles2::DisallowedFeatures disallowed_features_;
  GpuWatchdog* watchdog_;

  size_t num_stubs_descheduled_;

  // Map of stream id to stream state.
  base::hash_map<int32, StreamState> streams_;

  bool allow_future_sync_points_;
  bool allow_real_time_streams_;

  gfx::CLApi* clApiImpl;

  // Member variables should appear before the WeakPtrFactory, to ensure
  // that any WeakPtrs to Controller are invalidated before its members
  // variable's destructors are executed, rendering them invalid.
  base::WeakPtrFactory<GpuChannel> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(GpuChannel);
};

// This filter does three things:
// - it counts and timestamps each message forwarded to the channel
//   so that we can preempt other channels if a message takes too long to
//   process. To guarantee fairness, we must wait a minimum amount of time
//   before preempting and we limit the amount of time that we can preempt in
//   one shot (see constants above).
// - it handles the GpuCommandBufferMsg_InsertSyncPoint message on the IO
//   thread, generating the sync point ID and responding immediately, and then
//   posting a task to insert the GpuCommandBufferMsg_RetireSyncPoint message
//   into the channel's queue.
// - it generates mailbox names for clients of the GPU process on the IO thread.
class GpuChannelMessageFilter : public IPC::MessageFilter {
 public:
  GpuChannelMessageFilter(const base::WeakPtr<GpuChannel>& gpu_channel,
                          GpuChannelMessageQueue* message_queue,
                          base::SingleThreadTaskRunner* task_runner,
                          gpu::PreemptionFlag* preempting_flag,
                          bool future_sync_points);

  // IPC::MessageFilter implementation.
  void OnFilterAdded(IPC::Sender* sender) override;
  void OnFilterRemoved() override;
  void OnChannelConnected(int32 peer_pid) override;
  void OnChannelError() override;
  void OnChannelClosing() override;
  bool OnMessageReceived(const IPC::Message& message) override;

  void AddChannelFilter(scoped_refptr<IPC::MessageFilter> filter);
  void RemoveChannelFilter(scoped_refptr<IPC::MessageFilter> filter);

  void OnMessageProcessed();

  void UpdateStubSchedulingState(bool a_stub_is_descheduled);

  bool Send(IPC::Message* message);

  void setCLApi(gfx::CLApi* api) { cl_api_ = api; };

 protected:
  ~GpuChannelMessageFilter() override;

 private:
  enum PreemptionState {
    // Either there's no other channel to preempt, there are no messages
    // pending processing, or we just finished preempting and have to wait
    // before preempting again.
    IDLE,
    // We are waiting kPreemptWaitTimeMs before checking if we should preempt.
    WAITING,
    // We can preempt whenever any IPC processing takes more than
    // kPreemptWaitTimeMs.
    CHECKING,
    // We are currently preempting (i.e. no stub is descheduled).
    PREEMPTING,
    // We would like to preempt, but some stub is descheduled.
    WOULD_PREEMPT_DESCHEDULED,
  };

  void UpdatePreemptionState();

  void TransitionToIdleIfCaughtUp();
  void TransitionToIdle();
  void TransitionToWaiting();
  void TransitionToChecking();
  void TransitionToPreempting();
  void TransitionToWouldPreemptDescheduled();

  PreemptionState preemption_state_;

  // Maximum amount of time that we can spend in PREEMPTING.
  // It is reset when we transition to IDLE.
  base::TimeDelta max_preemption_time_;
  
  // The message_queue_ is used to handle messages on the main thread.
  scoped_refptr<GpuChannelMessageQueue> message_queue_;
  IPC::Sender* sender_;
  base::ProcessId peer_pid_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  scoped_refptr<gpu::PreemptionFlag> preempting_flag_;
  std::vector<scoped_refptr<IPC::MessageFilter>> channel_filters_;

  // This timer is created and destroyed on the IO thread.
  scoped_ptr<base::OneShotTimer> timer_;

  bool a_stub_is_descheduled_;

  // True if this channel can create future sync points.
  bool future_sync_points_;

  gfx::CLApi* cl_api_;

  base::WeakPtr<GpuChannel> gpu_channel_;
};

struct GpuChannelMessage {
  uint32_t order_number;
  base::TimeTicks time_received;
  IPC::Message message;

  // TODO(dyen): Temporary sync point data, remove once new sync point lands.
  bool retire_sync_point;
  uint32 sync_point;

  GpuChannelMessage(const IPC::Message& msg)
      : order_number(0),
        time_received(base::TimeTicks()),
        message(msg),
        retire_sync_point(false),
        sync_point(0) {}

 private:
  DISALLOW_COPY_AND_ASSIGN(GpuChannelMessage);
};

class GpuChannelMessageQueue
    : public base::RefCountedThreadSafe<GpuChannelMessageQueue> {
 public:
  static scoped_refptr<GpuChannelMessageQueue> Create(
      const base::WeakPtr<GpuChannel>& gpu_channel,
      base::SingleThreadTaskRunner* task_runner,
      gpu::SyncPointManager* sync_point_manager);

  scoped_refptr<gpu::SyncPointOrderData> GetSyncPointOrderData();

  // Returns the global order number for the last unprocessed IPC message.
  uint32_t GetUnprocessedOrderNum() const;

  // Returns the global order number for the last unprocessed IPC message.
  uint32_t GetProcessedOrderNum() const;

  bool HasQueuedMessages() const;

  base::TimeTicks GetNextMessageTimeTick() const;

  GpuChannelMessage* GetNextMessage() const;

  void BeginMessageProcessing(const GpuChannelMessage* msg);

  // Should be called after a message returned by GetNextMessage is processed.
  // Returns true if there are more messages on the queue.
  bool MessageProcessed();

  void PushBackMessage(const IPC::Message& message);

  bool GenerateSyncPointMessage(const IPC::Message& message,
                                bool retire_sync_point,
                                uint32_t* sync_point_number);

  void DeleteAndDisableMessages();

 private:
  friend class base::RefCountedThreadSafe<GpuChannelMessageQueue>;

  GpuChannelMessageQueue(const base::WeakPtr<GpuChannel>& gpu_channel,
                         base::SingleThreadTaskRunner* task_runner,
                         gpu::SyncPointManager* sync_point_manager);
  ~GpuChannelMessageQueue();

  void ScheduleHandleMessage();

  void PushMessageHelper(scoped_ptr<GpuChannelMessage> msg);

  bool enabled_;

  // Both deques own the messages.
  std::deque<GpuChannelMessage*> channel_messages_;

  // This lock protects enabled_ and channel_messages_.
  mutable base::Lock channel_messages_lock_;

  // Keeps track of sync point related state such as message order numbers.
  scoped_refptr<gpu::SyncPointOrderData> sync_point_order_data_;

  base::WeakPtr<GpuChannel> gpu_channel_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  gpu::SyncPointManager* sync_point_manager_;

  DISALLOW_COPY_AND_ASSIGN(GpuChannelMessageQueue);
};

}  // namespace content

#endif  // CONTENT_COMMON_GPU_GPU_CHANNEL_H_
