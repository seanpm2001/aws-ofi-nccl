/*
 * Copyright (c) 2017-2022, NVIDIA CORPORATION. All rights reserved.
 * Copyright (c) 2022 Amazon.com, Inc. or its affiliates. All rights reserved.
 */

#ifndef NCCL_HEADERS_NET_V4_H_
#define NCCL_HEADERS_NET_V4_H_

#ifdef _cplusplus
extern "C" {
#endif

#define NCCL_NET_HANDLE_MAXSIZE_V4 64

typedef struct {
	char* name;     // Used mostly for logging.
	char* pciPath;  // Path to the PCI device in /sys.
	uint64_t guid;  // Unique identifier for the NIC chip. Important for
			// cards with multiple PCI functions (Physical or virtual).
	int ptrSupport; // NCCL_PTR_HOST or NCCL_PTR_HOST|NCCL_PTR_CUDA
	int speed;      // Port speed in Mbps.
	int port;       // Port number.
	int maxComms;   // Maximum number of comms we can create
} ncclNetProperties_v4_t;

// v4 struct for backwards compatibility
typedef struct {
	// Name of the network (mainly for logs)
	const char* name;
	// Initialize the network.
	ncclResult_t (*init)(ncclDebugLogger_t logFunction);
	// Return the number of adapters.
	ncclResult_t (*devices)(int* ndev);
	// Get various device properties.
	ncclResult_t (*getProperties)(int dev, ncclNetProperties_v4_t* props);
	// Create a receiving object and provide a handle to connect to it. The
	// handle can be up to NCCL_NET_HANDLE_MAXSIZE bytes and will be exchanged
	// between ranks to create a connection.
	ncclResult_t (*listen)(int dev, void* handle, void** listenComm);
	// Connect to a handle and return a sending comm object for that peer.
	ncclResult_t (*connect)(int dev, void* handle, void** sendComm);
	// Finalize connection establishment after remote peer has called connectHandle
	ncclResult_t (*accept)(void* listenComm, void** recvComm);
	// Register/Deregister memory. Comm can be either a sendComm or a recvComm.
	// Type is either NCCL_PTR_HOST or NCCL_PTR_CUDA.
#if HAVE_NEURON
	ncclResult_t (*regMr)(void* comm, void* data, size_t size, int type, void** mhandle);
#elif HAVE_CUDA
	ncclResult_t (*regMr)(void* comm, void* data, int size, int type, void** mhandle);
#endif
	ncclResult_t (*deregMr)(void* comm, void* mhandle);
	// Asynchronous send to a peer.
	// May return request == NULL if the call cannot be performed (or would block)
	ncclResult_t (*isend)(void* sendComm, void* data, int size, void* mhandle, void** request);
	// Asynchronous recv from a peer.
	// May return request == NULL if the call cannot be performed (or would block)
	ncclResult_t (*irecv)(void* recvComm, void* data, int size, void* mhandle, void** request);
	// Perform a flush/fence to make sure all data received with NCCL_PTR_CUDA is
	// visible to the GPU
	ncclResult_t (*iflush)(void* recvComm, void* data, int size, void* mhandle, void** request);
	// Test whether a request is complete. If size is not NULL, it returns the
	// number of bytes sent/received.
	ncclResult_t (*test)(void* request, int* done, int* size);
	// Close and free send/recv comm objects
	ncclResult_t (*closeSend)(void* sendComm);
	ncclResult_t (*closeRecv)(void* recvComm);
	ncclResult_t (*closeListen)(void* listenComm);
} ncclNet_v4_t;

#ifdef _cplusplus
}
#endif // End extern "C"

#endif // End NCCL_HEADERS_NET_V4_H_
