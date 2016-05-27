// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Multiply-included message file, hence no include guard here, but see below
// for a much smaller-than-usual include guard section.

#include <stdint.h>

#include <string>
#include <vector>

#include "base/memory/shared_memory.h"
#include "build/build_config.h"
#include "content/common/content_export.h"
#include "content/common/content_param_traits.h"
#include "content/common/gpu/gpu_memory_uma_stats.h"
#include "content/common/gpu/gpu_process_launch_causes.h"
#include "content/common/gpu/gpu_stream_constants.h"
#include "gpu/command_buffer/common/capabilities.h"
#include "gpu/command_buffer/common/command_buffer.h"
#include "gpu/command_buffer/common/constants.h"
#include "gpu/command_buffer/common/gpu_memory_allocation.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "gpu/command_buffer/common/sync_token.h"
#include "gpu/command_buffer/common/value_state.h"
#include "gpu/config/gpu_info.h"
#include "gpu/ipc/common/gpu_command_buffer_traits.h"
#include "ipc/ipc_channel_handle.h"
#include "ipc/ipc_message_macros.h"
#include "ui/events/latency_info.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "ui/gfx/ipc/gfx_param_traits.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/swap_result.h"
#include "ui/gl/gpu_preference.h"
#include "url/ipc/url_param_traits.h"
#include "ui/opencl/opencl_include.h"
#if defined(OS_ANDROID)
#include "gpu/ipc/common/android/surface_texture_peer.h"
#elif defined(OS_MACOSX)
#include "ui/base/cocoa/remote_layer_api.h"
#include "ui/gfx/mac/io_surface.h"
#endif

#undef IPC_MESSAGE_EXPORT
#define IPC_MESSAGE_EXPORT CONTENT_EXPORT

#define IPC_MESSAGE_START GpuChannelMsgStart

IPC_ENUM_TRAITS_MAX_VALUE(gfx::GpuPreference,
                          gfx::GpuPreferenceLast)
IPC_ENUM_TRAITS_MAX_VALUE(content::GpuStreamPriority,
                          content::GpuStreamPriority::LAST)
IPC_ENUM_TRAITS_MAX_VALUE(gfx::SurfaceType,
                          gfx::SURFACE_TYPE_LAST)
IPC_ENUM_TRAITS_MAX_VALUE(gfx::SwapResult, gfx::SwapResult::SWAP_RESULT_LAST)
IPC_ENUM_TRAITS_MAX_VALUE(gpu::MemoryAllocation::PriorityCutoff,
                          gpu::MemoryAllocation::CUTOFF_LAST)
IPC_ENUM_TRAITS_MAX_VALUE(gpu::error::ContextLostReason,
                          gpu::error::kContextLostReasonLast)
IPC_ENUM_TRAITS_MIN_MAX_VALUE(gpu::CollectInfoResult,
                              gpu::kCollectInfoNone,
                              gpu::kCollectInfoFatalFailure)
IPC_ENUM_TRAITS_MIN_MAX_VALUE(gpu::VideoCodecProfile,
                              gpu::VIDEO_CODEC_PROFILE_MIN,
                              gpu::VIDEO_CODEC_PROFILE_MAX)

IPC_STRUCT_BEGIN(GPUCreateCommandBufferConfig)
  IPC_STRUCT_MEMBER(int32_t, share_group_id)
  IPC_STRUCT_MEMBER(int32_t, stream_id)
  IPC_STRUCT_MEMBER(content::GpuStreamPriority, stream_priority)
  IPC_STRUCT_MEMBER(std::vector<int>, attribs)
  IPC_STRUCT_MEMBER(GURL, active_url)
  IPC_STRUCT_MEMBER(gfx::GpuPreference, gpu_preference)
IPC_STRUCT_END()

IPC_STRUCT_BEGIN(GPUCommandBufferConsoleMessage)
  IPC_STRUCT_MEMBER(int32_t, id)
  IPC_STRUCT_MEMBER(std::string, message)
IPC_STRUCT_END()

#if defined(OS_ANDROID)
IPC_STRUCT_BEGIN(GpuStreamTextureMsg_MatrixChanged_Params)
  IPC_STRUCT_MEMBER(float, m00)
  IPC_STRUCT_MEMBER(float, m01)
  IPC_STRUCT_MEMBER(float, m02)
  IPC_STRUCT_MEMBER(float, m03)
  IPC_STRUCT_MEMBER(float, m10)
  IPC_STRUCT_MEMBER(float, m11)
  IPC_STRUCT_MEMBER(float, m12)
  IPC_STRUCT_MEMBER(float, m13)
  IPC_STRUCT_MEMBER(float, m20)
  IPC_STRUCT_MEMBER(float, m21)
  IPC_STRUCT_MEMBER(float, m22)
  IPC_STRUCT_MEMBER(float, m23)
  IPC_STRUCT_MEMBER(float, m30)
  IPC_STRUCT_MEMBER(float, m31)
  IPC_STRUCT_MEMBER(float, m32)
  IPC_STRUCT_MEMBER(float, m33)
IPC_STRUCT_END()
#endif

IPC_STRUCT_BEGIN(GpuCommandBufferMsg_CreateImage_Params)
  IPC_STRUCT_MEMBER(int32_t, id)
  IPC_STRUCT_MEMBER(gfx::GpuMemoryBufferHandle, gpu_memory_buffer)
  IPC_STRUCT_MEMBER(gfx::Size, size)
  IPC_STRUCT_MEMBER(gfx::BufferFormat, format)
  IPC_STRUCT_MEMBER(uint32_t, internal_format)
  IPC_STRUCT_MEMBER(uint64_t, image_release_count)
IPC_STRUCT_END()

IPC_STRUCT_TRAITS_BEGIN(gpu::DxDiagNode)
  IPC_STRUCT_TRAITS_MEMBER(values)
  IPC_STRUCT_TRAITS_MEMBER(children)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(gpu::GPUInfo::GPUDevice)
  IPC_STRUCT_TRAITS_MEMBER(vendor_id)
  IPC_STRUCT_TRAITS_MEMBER(device_id)
  IPC_STRUCT_TRAITS_MEMBER(active)
  IPC_STRUCT_TRAITS_MEMBER(vendor_string)
  IPC_STRUCT_TRAITS_MEMBER(device_string)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(gpu::GPUInfo)
  IPC_STRUCT_TRAITS_MEMBER(initialization_time)
  IPC_STRUCT_TRAITS_MEMBER(optimus)
  IPC_STRUCT_TRAITS_MEMBER(amd_switchable)
  IPC_STRUCT_TRAITS_MEMBER(lenovo_dcute)
  IPC_STRUCT_TRAITS_MEMBER(gpu)
  IPC_STRUCT_TRAITS_MEMBER(secondary_gpus)
  IPC_STRUCT_TRAITS_MEMBER(adapter_luid)
  IPC_STRUCT_TRAITS_MEMBER(driver_vendor)
  IPC_STRUCT_TRAITS_MEMBER(driver_version)
  IPC_STRUCT_TRAITS_MEMBER(driver_date)
  IPC_STRUCT_TRAITS_MEMBER(pixel_shader_version)
  IPC_STRUCT_TRAITS_MEMBER(vertex_shader_version)
  IPC_STRUCT_TRAITS_MEMBER(max_msaa_samples)
  IPC_STRUCT_TRAITS_MEMBER(machine_model_name)
  IPC_STRUCT_TRAITS_MEMBER(machine_model_version)
  IPC_STRUCT_TRAITS_MEMBER(gl_version)
  IPC_STRUCT_TRAITS_MEMBER(gl_vendor)
  IPC_STRUCT_TRAITS_MEMBER(gl_renderer)
  IPC_STRUCT_TRAITS_MEMBER(gl_extensions)
  IPC_STRUCT_TRAITS_MEMBER(gl_ws_vendor)
  IPC_STRUCT_TRAITS_MEMBER(gl_ws_version)
  IPC_STRUCT_TRAITS_MEMBER(gl_ws_extensions)
  IPC_STRUCT_TRAITS_MEMBER(gl_reset_notification_strategy)
  IPC_STRUCT_TRAITS_MEMBER(can_lose_context)
  IPC_STRUCT_TRAITS_MEMBER(software_rendering)
  IPC_STRUCT_TRAITS_MEMBER(direct_rendering)
  IPC_STRUCT_TRAITS_MEMBER(sandboxed)
  IPC_STRUCT_TRAITS_MEMBER(process_crash_count)
  IPC_STRUCT_TRAITS_MEMBER(in_process_gpu)
  IPC_STRUCT_TRAITS_MEMBER(basic_info_state)
  IPC_STRUCT_TRAITS_MEMBER(context_info_state)
#if defined(OS_WIN)
  IPC_STRUCT_TRAITS_MEMBER(dx_diagnostics_info_state)
  IPC_STRUCT_TRAITS_MEMBER(dx_diagnostics)
#endif
  IPC_STRUCT_TRAITS_MEMBER(video_decode_accelerator_capabilities)
  IPC_STRUCT_TRAITS_MEMBER(video_encode_accelerator_supported_profiles)
  IPC_STRUCT_TRAITS_MEMBER(jpeg_decode_accelerator_supported)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(gpu::MemoryAllocation)
  IPC_STRUCT_TRAITS_MEMBER(bytes_limit_when_visible)
  IPC_STRUCT_TRAITS_MEMBER(priority_cutoff_when_visible)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(gfx::GLSurfaceHandle)
  IPC_STRUCT_TRAITS_MEMBER(handle)
  IPC_STRUCT_TRAITS_MEMBER(transport_type)
IPC_STRUCT_TRAITS_END()

//------------------------------------------------------------------------------
// GPU Channel Messages
// These are messages from a renderer process to the GPU process.

// Tells the GPU process to create a new command buffer that renders directly
// to a native view. A corresponding GpuCommandBufferStub is created.
IPC_SYNC_MESSAGE_CONTROL3_1(GpuChannelMsg_CreateViewCommandBuffer,
                            gfx::GLSurfaceHandle, /* compositing_surface */
                            GPUCreateCommandBufferConfig, /* init_params */
                            int32_t /* route_id */,
                            bool /* succeeded */)

// Tells the GPU process to create a new command buffer that renders to an
// offscreen frame buffer.
IPC_SYNC_MESSAGE_CONTROL3_1(GpuChannelMsg_CreateOffscreenCommandBuffer,
                            gfx::Size,                    /* size */
                            GPUCreateCommandBufferConfig, /* init_params */
                            int32_t,                      /* route_id */
                            bool /* succeeded */)

// The CommandBufferProxy sends this to the GpuCommandBufferStub in its
// destructor, so that the stub deletes the actual CommandBufferService
// object that it's hosting.
IPC_SYNC_MESSAGE_CONTROL1_0(GpuChannelMsg_DestroyCommandBuffer,
                            int32_t /* instance_id */)

// Simple NOP message which can be used as fence to ensure all previous sent
// messages have been received.
IPC_SYNC_MESSAGE_CONTROL0_0(GpuChannelMsg_Nop)

#if defined(OS_ANDROID)
//------------------------------------------------------------------------------
// Stream Texture Messages
// Tells the GPU process create and send the java surface texture object to
// the renderer process through the binder thread.
IPC_MESSAGE_ROUTED2(GpuStreamTextureMsg_EstablishPeer,
                    int32_t, /* primary_id */
                    int32_t /* secondary_id */)

// Tells the GPU process to set the size of StreamTexture from the given
// stream Id.
IPC_MESSAGE_ROUTED1(GpuStreamTextureMsg_SetSize,
                    gfx::Size /* size */)

// Tells the service-side instance to start sending frame available
// notifications.
IPC_MESSAGE_ROUTED0(GpuStreamTextureMsg_StartListening)

// Inform the renderer that a new frame is available.
IPC_MESSAGE_ROUTED0(GpuStreamTextureMsg_FrameAvailable)

// Inform the renderer process that the transform matrix has changed.
IPC_MESSAGE_ROUTED1(GpuStreamTextureMsg_MatrixChanged,
                    GpuStreamTextureMsg_MatrixChanged_Params /* params */)
#endif

//------------------------------------------------------------------------------
// GPU Command Buffer Messages
// These are messages between a renderer process to the GPU process relating to
// a single OpenGL context.
// Initialize a command buffer with the given number of command entries.
// Returns the shared memory handle for the command buffer mapped to the
// calling process.
IPC_SYNC_MESSAGE_ROUTED1_2(GpuCommandBufferMsg_Initialize,
                           base::SharedMemoryHandle /* shared_state */,
                           bool /* result */,
                           gpu::Capabilities /* capabilities */)

// Sets the shared memory buffer used for commands.
IPC_SYNC_MESSAGE_ROUTED1_0(GpuCommandBufferMsg_SetGetBuffer,
                           int32_t /* shm_id */)

// Produces the front buffer into a mailbox. This allows another context to draw
// the output of this context.
IPC_MESSAGE_ROUTED1(GpuCommandBufferMsg_ProduceFrontBuffer,
                    gpu::Mailbox /* mailbox */)

// Wait until the token is in a specific range, inclusive.
IPC_SYNC_MESSAGE_ROUTED2_1(GpuCommandBufferMsg_WaitForTokenInRange,
                           int32_t /* start */,
                           int32_t /* end */,
                           gpu::CommandBuffer::State /* state */)

// Wait until the get offset is in a specific range, inclusive.
IPC_SYNC_MESSAGE_ROUTED2_1(GpuCommandBufferMsg_WaitForGetOffsetInRange,
                           int32_t /* start */,
                           int32_t /* end */,
                           gpu::CommandBuffer::State /* state */)

// Asynchronously synchronize the put and get offsets of both processes.
// Caller passes its current put offset. Current state (including get offset)
// is returned in shared memory. The input latency info for the current
// frame is also sent to the GPU process.
IPC_MESSAGE_ROUTED3(GpuCommandBufferMsg_AsyncFlush,
                    int32_t /* put_offset */,
                    uint32_t /* flush_count */,
                    std::vector<ui::LatencyInfo> /* latency_info */)

// Sent by the GPU process to display messages in the console.
IPC_MESSAGE_ROUTED1(GpuCommandBufferMsg_ConsoleMsg,
                    GPUCommandBufferConsoleMessage /* msg */)

// Register an existing shared memory transfer buffer. The id that can be
// used to identify the transfer buffer from a command buffer.
IPC_MESSAGE_ROUTED3(GpuCommandBufferMsg_RegisterTransferBuffer,
                    int32_t /* id */,
                    base::SharedMemoryHandle /* transfer_buffer */,
                    uint32_t /* size */)

// Destroy a previously created transfer buffer.
IPC_MESSAGE_ROUTED1(GpuCommandBufferMsg_DestroyTransferBuffer, int32_t /* id */)

// Tells the proxy that there was an error and the command buffer had to be
// destroyed for some reason.
IPC_MESSAGE_ROUTED2(GpuCommandBufferMsg_Destroyed,
                    gpu::error::ContextLostReason, /* reason */
                    gpu::error::Error /* error */)

// Tells the browser that SwapBuffers returned and passes latency info
IPC_MESSAGE_ROUTED2(GpuCommandBufferMsg_SwapBuffersCompleted,
                    std::vector<ui::LatencyInfo> /* latency_info */,
                    gfx::SwapResult /* result */)

// Tells the browser about updated parameters for vsync alignment.
IPC_MESSAGE_ROUTED2(GpuCommandBufferMsg_UpdateVSyncParameters,
                    base::TimeTicks /* timebase */,
                    base::TimeDelta /* interval */)

IPC_MESSAGE_ROUTED2(GpuCommandBufferMsg_SignalSyncToken,
                    gpu::SyncToken /* sync_token */,
                    uint32_t /* signal_id */)

// Makes this command buffer signal when a query is reached, by sending
// back a GpuCommandBufferMsg_SignalSyncPointAck message with the same
// signal_id.
IPC_MESSAGE_ROUTED2(GpuCommandBufferMsg_SignalQuery,
                    uint32_t /* query */,
                    uint32_t /* signal_id */)

// Response to SignalSyncPoint, SignalSyncToken, and SignalQuery.
IPC_MESSAGE_ROUTED1(GpuCommandBufferMsg_SignalAck, uint32_t /* signal_id */)

// Create an image from an existing gpu memory buffer. The id that can be
// used to identify the image from a command buffer.
IPC_MESSAGE_ROUTED1(GpuCommandBufferMsg_CreateImage,
                    GpuCommandBufferMsg_CreateImage_Params /* params */)

// Destroy a previously created image.
IPC_MESSAGE_ROUTED1(GpuCommandBufferMsg_DestroyImage, int32_t /* id */)

// Attaches an external image stream to the client texture.
IPC_SYNC_MESSAGE_ROUTED2_1(GpuCommandBufferMsg_CreateStreamTexture,
                           uint32_t, /* client_texture_id */
                           int32_t,  /* stream_id */
                           bool /* succeeded */)
/**
 *
 */
IPC_SYNC_MESSAGE_CONTROL2_3(OpenCLChannelMsg_getPlatformsIDs,
					cl_uint,
					std::vector<bool>,
					std::vector<cl_point>,
					cl_uint,
					cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_getPlatformInfo,
					cl_point,
					cl_platform_info,
					size_t,
					std::vector<bool>,
					cl_int,
					std::string,
					size_t)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetDeviceIDs,
							cl_point,
							cl_device_type,
							cl_uint,
							std::vector<bool>,
							std::vector<cl_point>,
							cl_uint,
							cl_int)

// Call and respond OpenCL API clGetDeviceInfo using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetDeviceInfo_cl_point,
              cl_point,
              cl_device_info,
              size_t,
              std::vector<bool>,
              cl_point,
              size_t,
              cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetDeviceInfo_cl_uint,
							cl_point,
							cl_device_info,
							size_t,
							std::vector<bool>,
							cl_uint,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetDeviceInfo_size_t_list,
							cl_point,
							cl_device_info,
							size_t,
							std::vector<bool>,
							std::vector<size_t>,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetDeviceInfo_size_t,
							cl_point,
							cl_device_info,
							size_t,
							std::vector<bool>,
							size_t,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetDeviceInfo_cl_ulong,
							cl_point,
							cl_device_info,
							size_t,
							std::vector<bool>,
							cl_ulong,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetDeviceInfo_string,
							cl_point,
							cl_device_info,
							size_t,
							std::vector<bool>,
							std::string,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetDeviceInfo_intptr_t_list,
							cl_point,
							cl_device_info,
							size_t,
							std::vector<bool>,
							std::vector<intptr_t>,
							size_t,
							cl_int)

// Call and respond OpenCL API clCreateContextFromType using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL5_2(OpenCLChannelMsg_CreateContextFromType,
							std::vector<cl_context_properties>,
							cl_device_type,
							cl_point,
							cl_point,
							std::vector<bool>,
							cl_int,
							cl_point)

// Call and respond OpenCL API clCreateContextFromType using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL5_2(OpenCLChannelMsg_CreateContext,
							std::vector<cl_context_properties>,
							std::vector<cl_point>,
							cl_point,
							cl_point,
							std::vector<bool>,
							cl_int,
							cl_point)

IPC_SYNC_MESSAGE_CONTROL2_1(OpenCLChannelMsg_WaitForEvents,
							cl_uint,
							std::vector<cl_point>,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetMemObjectInfo_cl_int,
							cl_point,
							cl_mem_info,
							size_t,
							std::vector<bool>,
							cl_int,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetMemObjectInfo_cl_uint,
							cl_point,
							cl_mem_info,
							size_t,
							std::vector<bool>,
							cl_uint,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetMemObjectInfo_cl_ulong,
							cl_point,
							cl_mem_info,
							size_t,
							std::vector<bool>,
							cl_ulong,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetMemObjectInfo_size_t,
							cl_point,
							cl_mem_info,
							size_t,
							std::vector<bool>,
							size_t,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetMemObjectInfo_cl_point,
							cl_point,
							cl_mem_info,
							size_t,
							std::vector<bool>,
							cl_point,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL5_2(OpenCLChannelMsg_CreateSubBuffer,
							cl_point,
							cl_mem_flags,
							cl_buffer_create_type,
							size_t,
							size_t,
							cl_point,
							cl_int)

// Call and respond OpenCL API clCreateSampler using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL5_2(OpenCLChannelMsg_CreateSampler,
							cl_point,
							cl_bool,
							cl_addressing_mode,
							cl_filter_mode,
							std::vector<bool>,
							cl_int,
							cl_point)

// Call and respond OpenCL API clGetSamplerInfo using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetSamplerInfo_cl_uint,
														cl_point,
														cl_sampler_info,
														size_t,
														std::vector<bool>,
														cl_uint,
														size_t,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetSamplerInfo_cl_point,
														cl_point,
														cl_sampler_info,
														size_t,
														std::vector<bool>,
														cl_point,
														size_t,
														cl_int)

// Call and respond OpenCL API clReleaseSampler using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLChannelMsg_ReleaseSampler,
							cl_point,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetImageInfo_cl_int,
							cl_point,
							cl_image_info,
							size_t,
							std::vector<bool>,
							cl_int,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetImageInfo_cl_uint_list,
							cl_point,
							cl_image_info,
							size_t,
							std::vector<bool>,
							std::vector<cl_uint>,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetImageInfo_cl_point,
							cl_point,
							cl_image_info,
							size_t,
							std::vector<bool>,
							cl_point,
							size_t,
							cl_int)


IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetImageInfo_size_t,
							cl_point,
							cl_image_info,
							size_t,
							std::vector<bool>,
							size_t,
							size_t,
							cl_int)
// Call and respond OpenCL API clGetEventInfo using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetEventInfo_cl_point,
														cl_point,
														cl_event_info,
														size_t,
														std::vector<bool>,
														cl_point,
														size_t,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetEventInfo_cl_uint,
														cl_point,
														cl_event_info,
														size_t,
														std::vector<bool>,
														cl_uint,
														size_t,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetEventInfo_cl_int,
														cl_point,
														cl_event_info,
														size_t,
														std::vector<bool>,
														cl_int,
														size_t,
														cl_int)

// Call and respond OpenCL API clGetEventProfilingInfo using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetEventProfilingInfo_cl_ulong,
														cl_point,
														cl_profiling_info,
														size_t,
														std::vector<bool>,
														cl_ulong,
														size_t,
														cl_int)

// Call and respond OpenCL API clSetEventCallback using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL3_1(OpenCLChannelMsg_SetEventCallback,
														cl_point,
														cl_int,
														std::vector<int>,
														cl_int)

// Call and respond OpenCL API clReleaseEvent using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLChannelMsg_ReleaseEvent,
														cl_point,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetContextInfo_cl_point,
							cl_point,
							cl_context_info,
							size_t,
							std::vector<bool>,
							cl_point,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetContextInfo_cl_point_list,
							cl_point,
							cl_context_info,
							size_t,
							std::vector<bool>,
							std::vector<cl_point>,
							size_t,
							cl_int)


IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetContextInfo_cl_uint,
							cl_point,
							cl_context_info,
							size_t,
							std::vector<bool>,
							cl_uint,
							size_t,
							cl_int)

// Call and respond OpenCL API clSetUserEventStatus using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL2_1(OpenCLChannelMsg_SetUserEventStatus,
							cl_point,
							cl_int,
							cl_int)

// Call and respond OpenCL API clCreateUserEvent using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL2_2(OpenCLChannelMsg_CreateUserEvent,
							cl_point,
							std::vector<bool>,
							cl_int,
							cl_point)

IPC_SYNC_MESSAGE_CONTROL5_3(OpenCLChannelMsg_GetSupportedImageFormat,
							cl_point,
							cl_mem_flags,
							cl_mem_object_type,
							cl_uint,
							std::vector<bool>,
							std::vector<cl_uint>,
							cl_uint,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL2_1(OpenCLChannelMsg_ReleaseCommon,
							cl_point,
							int,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_2(OpenCLChannelMsg_CreateCommandQueue,
							cl_point,
							cl_point,
							cl_command_queue_properties,
							std::vector<bool>,
							cl_int,
							cl_point)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetCommandQueueInfo_cl_point,
							cl_point,
							cl_context_info,
							size_t,
							std::vector<bool>,
							cl_point,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetCommandQueueInfo_cl_ulong,
							cl_point,
							cl_context_info,
							size_t,
							std::vector<bool>,
							cl_ulong,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLChannelMsg_Flush,
							cl_point,
							cl_int)

// Call and respond OpenCL API clGetKernelInfo using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetKernelInfo_string,
							cl_point,
							cl_kernel_info,
							size_t,
							std::vector<bool>,
							std::string,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetKernelInfo_cl_uint,
							cl_point,
							cl_kernel_info,
							size_t,
							std::vector<bool>,
							cl_uint,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetKernelInfo_cl_point,
							cl_point,
							cl_kernel_info,
							size_t,
							std::vector<bool>,
							cl_point,
							size_t,
							cl_int)

// Call and respond OpenCL API clGetKernelWorkGroupInfo
// using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL5_3(OpenCLChannelMsg_GetKernelWorkGroupInfo_size_t_list,
							cl_point,
							cl_point,
							cl_kernel_work_group_info,
							size_t,
							std::vector<bool>,
							std::vector<size_t>,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL5_3(OpenCLChannelMsg_GetKernelWorkGroupInfo_size_t,
							cl_point,
							cl_point,
							cl_kernel_work_group_info,
							size_t,
							std::vector<bool>,
							size_t,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL5_3(OpenCLChannelMsg_GetKernelWorkGroupInfo_cl_ulong,
							cl_point,
							cl_point,
							cl_kernel_work_group_info,
							size_t,
							std::vector<bool>,
							cl_ulong,
							size_t,
							cl_int)

// Call and respond OpenCL API clGetKernelArgInfo using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL5_3(OpenCLChannelMsg_GetKernelArgInfo_string,
							cl_point,
							cl_uint,
							cl_kernel_arg_info,
							size_t,
							std::vector<bool>,
							std::string,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL5_3(OpenCLChannelMsg_GetKernelArgInfo_cl_uint,
							cl_point,
							cl_uint,
							cl_kernel_arg_info,
							size_t,
							std::vector<bool>,
							cl_uint,
							size_t,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL5_3(OpenCLChannelMsg_GetKernelArgInfo_cl_ulong,
							cl_point,
							cl_uint,
							cl_kernel_arg_info,
							size_t,
							std::vector<bool>,
							cl_ulong,
							size_t,
							cl_int)

IPC_MESSAGE_ROUTED3(OpenCLChannelMsg_Callback,
#if defined(OS_ANDROID)
              unsigned,
#elif defined(OS_LINUX)
              cl_point,
#endif
							unsigned,
							unsigned)

// Call and respond OpenCL API clReleaseKernel using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLChannelMsg_ReleaseKernel,
														cl_point,
														cl_int)

// Call and respond OpenCL API clGetProgramInfo using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetProgramInfo_cl_uint,
														cl_point,
														cl_program_info,
														size_t,
														std::vector<bool>,
														cl_uint,
														size_t,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetProgramInfo_cl_point,
														cl_point,
														cl_program_info,
														size_t,
														std::vector<bool>,
														cl_point,
														size_t,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetProgramInfo_cl_point_list,
														cl_point,
														cl_program_info,
														size_t,
														std::vector<bool>,
														std::vector<cl_point>,
														size_t,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetProgramInfo_string,
														cl_point,
														cl_program_info,
														size_t,
														std::vector<bool>,
														std::string,
														size_t,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetProgramInfo_size_t_list,
														cl_point,
														cl_program_info,
														size_t,
														std::vector<bool>,
														std::vector<size_t>,
														size_t,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetProgramInfo_string_list,
														cl_point,
														cl_program_info,
														size_t,
														std::vector<bool>,
														std::vector<std::string>,
														size_t,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_GetProgramInfo_size_t,
														cl_point,
														cl_program_info,
														size_t,
														std::vector<bool>,
														size_t,
														size_t,
														cl_int)

// Call and respond OpenCL API clCreateProgramWithSource
// using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL5_2(OpenCLChannelMsg_CreateProgramWithSource,
														cl_point,
														cl_uint,
														std::vector<std::string>,
														std::vector<size_t>,
														std::vector<bool>,
														cl_int,
														cl_point)

// Call and respond OpenCL API clGetProgramBuildInfo using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL5_3(OpenCLChannelMsg_GetProgramBuildInfo_cl_int,
														cl_point,
														cl_point,
														cl_program_build_info,
														size_t,
														std::vector<bool>,
														cl_int,
														size_t,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL5_3(OpenCLChannelMsg_GetProgramBuildInfo_string,
														cl_point,
														cl_point,
														cl_program_build_info,
														size_t,
														std::vector<bool>,
														std::string,
														size_t,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL5_3(OpenCLChannelMsg_GetProgramBuildInfo_cl_uint,
														cl_point,
														cl_point,
														cl_program_build_info,
														size_t,
														std::vector<bool>,
														cl_uint,
														size_t,
														cl_int)

// Call and response OpenCL API clBGuildProgram usin Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL5_1(OpenCLChannelMsg_BuildProgram,
														cl_point,
														cl_uint,
														std::vector<cl_point>,
														std::string,
														std::vector<cl_point>,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL1_2(OpenCLChannelMsg_EnqueueMarker,
							cl_point,
							cl_point,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLChannelMsg_EnqueueBarrier,
							cl_point,
							cl_int)

IPC_SYNC_MESSAGE_CONTROL3_1(OpenCLChannelMsg_EnqueueWaitForEvents,
							cl_point,
							std::vector<cl_point>,
							cl_uint,
							cl_int)

// Call and respond OpenCL API clCreateKernel using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL3_2(OpenCLChannelMsg_CreateKernel,
														cl_point,
														std::string,
														std::vector<bool>,
														cl_int,
														cl_point)

// Call and respond OpenCL API clCreateKernelsInProgram
// using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLChannelMsg_CreateKernelsInProgram,
														cl_point,
														cl_uint,
														std::vector<cl_point>,
														std::vector<bool>,
														std::vector<cl_point>,
														cl_uint,
														cl_int)

// Call and respond OpenCL API clReleaseProgram using Sync IPC Message
IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLChannelMsg_ReleaseProgram,
														cl_point,
														cl_int)

IPC_SYNC_MESSAGE_CONTROL4_1(OpenCLChannelMsg_CTRL_SetSharedHandles,
														base::SharedMemoryHandle,
														base::SharedMemoryHandle,
														base::SharedMemoryHandle,
														base::SharedMemoryHandle,
														bool)

IPC_SYNC_MESSAGE_CONTROL0_1(OpenCLChannelMsg_CTRL_ClearSharedHandles,
														bool)

IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_setArg)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_createBuffer)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_createImage)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueCopyBuffer)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueCopyBufferRect)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueCopyBufferToImage)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueCopyImageToBuffer)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueCopyImage)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueReadBuffer)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueReadBufferRect)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueReadImage)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueWriteBuffer)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueWriteBufferRect)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueWriteImage)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueNDRangeKernel)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_finish)

// gl/cl sharing
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_createBufferFromGLBuffer)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_createImageFromGLRenderbuffer)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_createImageFromGLTexture)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_getGLObjectInfo)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueAcquireGLObjects)
IPC_SYNC_MESSAGE_CONTROL0_0(OpenCLChannelMsg_CTRL_Trigger_enqueueReleaseGLObjects)

IPC_SYNC_MESSAGE_CONTROL0_2(OpenCLChannelMsg_getGLContext,
          cl_point, cl_point)
