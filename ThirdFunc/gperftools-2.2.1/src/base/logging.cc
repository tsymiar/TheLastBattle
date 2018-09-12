// Copyright (c) 2007, Google Inc.
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// ---
// This file just provides storage for FLAGS_verbose.

#include <config.h>
#include "base/logging.h"
#include "base/commandlineflags.h"

DEFINE_int32(verbose, EnvToInt("PERFTOOLS_VERBOSE", 0),
	"Set to numbers >0 for more verbose output, or <0 for less.  "
	"--verbose == -4 means we log fatal errors only.");


#if defined(_WIN32) 

#include "base/spinlock.h"

enum {
	kSharedMemorySize = 1024 * 1024 * 2  // 2 MB
};

enum OperationType {
	kCreateFile = 1,
	kWriteFile  = 2,
	kCloseFile  = 3
};

struct Operation {
	OperationType type;	
	HANDLE file_handle;
	size_t data_legnth;
	char filebame_or_data[256];
};

static SpinLock logging_file_lock(SpinLock::LINKER_INITIALIZED);
static Operation * file_operation = NULL;
static RawFD tcmalloc_shared_memory = NULL;
static RawFD tcmalloc_shared_memory_mutex = NULL;
static RawFD tcmalloc_file_op_event = NULL;
static RawFD tcmalloc_file_op_finished_event = NULL;

void logging_init()
{
	SpinLockHolder l(&logging_file_lock);
	// 1. mutex
	tcmalloc_shared_memory_mutex = OpenMutex(
		SYNCHRONIZE | MUTEX_ALL_ACCESS,
		true,
		L"tcmalloc_shared_memory_mutex"
		);
	if (tcmalloc_shared_memory_mutex == NULL) {
		RAW_LOG(ERROR, "failed to OpenMutex tcmalloc_shared_memory_mutex!\n");
		return;
	}

	// 2. start event
	tcmalloc_file_op_event = OpenEvent(
		SYNCHRONIZE | EVENT_ALL_ACCESS,
		true,
		L"tcmalloc_file_op_event"
		);
	if (tcmalloc_file_op_event == NULL) {
		RAW_LOG(ERROR, "failed to OpenEvent tcmalloc_file_op_event!\n");
		return;
	}

	// 3. end event
	tcmalloc_file_op_finished_event = OpenEvent(
		SYNCHRONIZE | EVENT_ALL_ACCESS,
		true,
		L"tcmalloc_file_op_finished_event"
		);
	if (tcmalloc_file_op_finished_event == NULL) {
		RAW_LOG(ERROR, "failed to OpenEvent tcmalloc_file_op_finished_event!\n");
		return;
	}

	// 4. share memory
	tcmalloc_shared_memory = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		true,
		L"tcmalloc_shared_memory");
	if (tcmalloc_shared_memory == NULL) {
		RAW_LOG(ERROR, "failed to OpenViewOfFile tcmalloc_shared_memory!\n");
		return;
	}

	file_operation = (Operation *) MapViewOfFile(
		tcmalloc_shared_memory,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		kSharedMemorySize
		);
	if (file_operation == NULL) {
		RAW_LOG(ERROR, "failed to MapViewOfFile file_operation!\n");
		return;
	}

	RAW_VLOG(0, "logging_init()");	
}

void logging_exit()
{
	SpinLockHolder l(&logging_file_lock);
	if (file_operation == NULL) {
		return;
	}

	if (!UnmapViewOfFile(file_operation)) {
		RAW_LOG(ERROR,"failed to UnmapViewOfFile tcmalloc_shared_memory_address!\n");
		return;
	}
	file_operation = NULL;

	if (!CloseHandle(tcmalloc_file_op_event)) {
		RAW_LOG(ERROR, "failed to CloseHandle tcmalloc_file_op_event!\n");
		return;
	}
	tcmalloc_file_op_event = NULL;

	if (!CloseHandle(tcmalloc_file_op_finished_event)) {
		RAW_LOG(ERROR, "failed to CloseHandle tcmalloc_file_op_finished_event!\n");
		return;
	}
	tcmalloc_file_op_finished_event = NULL;

	if (!CloseHandle(tcmalloc_shared_memory_mutex)) {
		RAW_LOG(ERROR,"failed to CloseHandle tcmalloc_shared_memory_mutex!\n");
		return;
	}
	tcmalloc_shared_memory_mutex = NULL;

	RAW_VLOG(0, "logging_exit()");	
}


RawFD RawOpenForWriting(const char* filename) 
{
	SpinLockHolder l(&logging_file_lock);
	if (file_operation == NULL) {
		return NULL;
	}

	// acquire the shared memory mutex
	if (WAIT_OBJECT_0 != WaitForSingleObject(tcmalloc_shared_memory_mutex, INFINITE)) {
		RAW_LOG(ERROR,"RawOpenForWriting failed to acquire the shared memory mutex\n");
		return NULL;
	}

	file_operation->file_handle = NULL;
	file_operation->type = kCreateFile;
	memcpy(file_operation->filebame_or_data,filename,strlen(filename)+1);
	SetEvent(tcmalloc_file_op_event);

	if (WAIT_OBJECT_0 != WaitForSingleObject(tcmalloc_file_op_finished_event, INFINITE)) {
		RAW_LOG(ERROR,"RawOpenForWriting failed to WaitForSingleObject tcmalloc_file_op_finished_event\n");
	}
	RawFD fd = file_operation->file_handle;

	// release the shared memory mutex
	if (!ReleaseMutex(tcmalloc_shared_memory_mutex)) {
		RAW_LOG(ERROR,  "RawOpenForWriting fialed to  ReleaseMutextcmalloc_shared_memory_mutex!\n");
	}
	return fd;
}

void RawWrite(RawFD handle, const char* buf, size_t len) {
	if (len > kSharedMemorySize - sizeof(struct Operation) * 2) {
		RAW_LOG(ERROR,  "RawWrite buffer size > kSharedMemorySize!\n");
		return;
	}
	SpinLockHolder l(&logging_file_lock);
	if (file_operation == NULL) {
		return;
	}
	// acquire the shared memory mutex
	if (WAIT_OBJECT_0 != WaitForSingleObject(tcmalloc_shared_memory_mutex, INFINITE)) {
		RAW_LOG(ERROR,"RawWrite failed to acquire the shared memory mutex\n");
		return;
	}
	file_operation->file_handle = handle;
	file_operation->type = kWriteFile;
	file_operation->data_legnth = len;	
	memcpy(file_operation->filebame_or_data, buf, len);
	SetEvent(tcmalloc_file_op_event);
	if (WAIT_OBJECT_0 != WaitForSingleObject(tcmalloc_file_op_finished_event, INFINITE)) {
		RAW_LOG(ERROR,"fRawWrite ailed to WaitForSingleObject tcmalloc_file_op_finished_event\n");
	}

	// release the shared memory mutex
	if (!ReleaseMutex(tcmalloc_shared_memory_mutex)) {
		RAW_LOG(ERROR,  "RawWrite fialed to  ReleaseMutextcmalloc_shared_memory_mutex!\n");
	} 
}

void RawClose(RawFD handle) {
	SpinLockHolder l(&logging_file_lock);
	if (file_operation == NULL) {
		return;
	}
	// acquire the shared memory mutex
	if (WAIT_OBJECT_0 != WaitForSingleObject(tcmalloc_shared_memory_mutex, INFINITE)) {
		RAW_LOG(ERROR,"RawClose failed to acquire the shared memory mutex\n");
		return;
	}
	file_operation->file_handle = handle;
	file_operation->type = kCloseFile;
	SetEvent(tcmalloc_file_op_event);
	if (WAIT_OBJECT_0 != WaitForSingleObject(tcmalloc_file_op_finished_event, INFINITE)) {
		RAW_LOG(ERROR,"RawClose failed to WaitForSingleObject tcmalloc_file_op_finished_event\n");
	}
	// release the shared memory mutex
	if (!ReleaseMutex(tcmalloc_shared_memory_mutex)) {
		RAW_LOG(ERROR,  "RawClose fialed to  ReleaseMutextcmalloc_shared_memory_mutex!\n");
	}
}



#elif  defined(__CYGWIN__) || defined(__CYGWIN32__)

// While windows does have a POSIX-compatible API
// (_open/_write/_close), it acquires memory.  Using this lower-level
// windows API is the closest we can get to being "raw".
RawFD RawOpenForWriting(const char* filename) {
	// CreateFile allocates memory if file_name isn't absolute, so if
	// that ever becomes a problem then we ought to compute the absolute
	// path on its behalf (perhaps the ntdll/kernel function isn't aware
	// of the working directory?)
	RawFD fd = CreateFileA(filename, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, 0, NULL);
	if (fd != kIllegalRawFD && GetLastError() == ERROR_ALREADY_EXISTS)
		SetEndOfFile(fd);    // truncate the existing file
	return fd;
}

void RawWrite(RawFD handle, const char* buf, size_t len) {
	while (len > 0) {
		DWORD wrote;
		BOOL ok = WriteFile(handle, buf, len, &wrote, NULL);
		// We do not use an asynchronous file handle, so ok==false means an error
		if (!ok) break;
		buf += wrote;
		len -= wrote;
	}
}

void RawClose(RawFD handle) {
	CloseHandle(handle);
}

#else  // _WIN32 || __CYGWIN__ || __CYGWIN32__

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

// Re-run fn until it doesn't cause EINTR.
#define NO_INTR(fn)  do {} while ((fn) < 0 && errno == EINTR)

RawFD RawOpenForWriting(const char* filename) {
	return open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0664);
}

void RawWrite(RawFD fd, const char* buf, size_t len) {
	while (len > 0) {
		ssize_t r;
		NO_INTR(r = write(fd, buf, len));
		if (r <= 0) break;
		buf += r;
		len -= r;
	}
}

void RawClose(RawFD fd) {
	NO_INTR(close(fd));
}

#endif  // _WIN32 || __CYGWIN__ || __CYGWIN32__
