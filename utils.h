#include <CL/cl.hpp>
#include <string>

void check_error(const int& error_code, const std::string& error_message) {
	if (error_message != "No message") {
		std::cout << error_message << ": ";
		switch (error_code) {
			case CL_SUCCESS: std::cout << "CL_SUCCESS" << std::endl; break;
			case CL_BUILD_PROGRAM_FAILURE: std::cout << "CL_BUILD_PROGRAM_FAILURE" << std::endl; break;
			case CL_COMPILE_PROGRAM_FAILURE: std::cout << "CL_COMPILE_PROGRAM_FAILURE" << std::endl; break;
			case CL_COMPILER_NOT_AVAILABLE: std::cout << "CL_COMPILER_NOT_AVAILABLE" << std::endl; break;
			case CL_DEVICE_NOT_FOUND: std::cout << "CL_DEVICE_NOT_FOUND" << std::endl; break;
			case CL_DEVICE_NOT_AVAILABLE: std::cout << "CL_DEVICE_NOT_AVAILABLE" << std::endl; break;
			case CL_DEVICE_PARTITION_FAILED: std::cout << "CL_DEVICE_PARTITION_FAILED" << std::endl; break;
			case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: std::cout << "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST" << std::endl; break;
			case CL_IMAGE_FORMAT_MISMATCH: std::cout << "CL_IMAGE_FORMAT_MISMATCH" << std::endl; break;
			case CL_IMAGE_FORMAT_NOT_SUPPORTED: std::cout << "CL_IMAGE_FORMAT_NOT_SUPPORTED" << std::endl; break;
			case CL_INVALID_ARG_INDEX: std::cout << "CL_INVALID_ARG_INDEX" << std::endl; break;
			case CL_INVALID_ARG_SIZE: std::cout << "CL_INVALID_ARG_SIZE" << std::endl; break;
			case CL_INVALID_ARG_VALUE: std::cout << "CL_INVALID_ARG_VALUE" << std::endl; break;
			case CL_INVALID_BINARY: std::cout << "CL_INVALID_BINARY" << std::endl; break;
			case CL_INVALID_BUFFER_SIZE: std::cout << "CL_INVALID_BUFFER_SIZE" << std::endl; break;
			case CL_INVALID_BUILD_OPTIONS: std::cout << "CL_INVALID_BUILD_OPTIONS" << std::endl; break;
			case CL_INVALID_COMMAND_QUEUE: std::cout << "CL_INVALID_COMMAND_QUEUE" << std::endl; break;
			case CL_INVALID_COMPILER_OPTIONS: std::cout << "CL_INVALID_COMPILER_OPTIONS" << std::endl; break;
			case CL_INVALID_CONTEXT: std::cout << "CL_INVALID_CONTEXT" << std::endl; break;
			case CL_INVALID_DEVICE: std::cout << "CL_INVALID_DEVICE" << std::endl; break;
			case CL_INVALID_DEVICE_PARTITION_COUNT: std::cout << "CL_INVALID_DEVICE_PARTITION_COUNT" << std::endl; break;
			case CL_INVALID_DEVICE_QUEUE: std::cout << "CL_INVALID_DEVICE_QUEUE" << std::endl; break;
			case CL_INVALID_DEVICE_TYPE: std::cout << "CL_INVALID_DEVICE_TYPE" << std::endl; break;
			case CL_INVALID_EVENT: std::cout << "CL_INVALID_EVENT" << std::endl; break;
			case CL_INVALID_EVENT_WAIT_LIST: std::cout << "CL_INVALID_EVENT_WAIT_LIST" << std::endl; break;
			case CL_INVALID_GLOBAL_OFFSET: std::cout << "CL_INVALID_GLOBAL_OFFSET" << std::endl; break;
			case CL_INVALID_GLOBAL_WORK_SIZE: std::cout << "CL_INVALID_GLOBAL_WORK_SIZE" << std::endl; break;
			case CL_INVALID_HOST_PTR: std::cout << "CL_INVALID_HOST_PTR" << std::endl; break;
			case CL_INVALID_IMAGE_DESCRIPTOR: std::cout << "CL_INVALID_IMAGE_DESCRIPTOR" << std::endl; break;
			case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: std::cout << "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR" << std::endl; break;
			case CL_INVALID_IMAGE_SIZE: std::cout << "CL_INVALID_IMAGE_SIZE" << std::endl; break;
			case CL_INVALID_KERNEL: std::cout << "CL_INVALID_KERNEL" << std::endl; break;
			case CL_INVALID_KERNEL_ARGS: std::cout << "CL_INVALID_KERNEL_ARGS" << std::endl; break;
			case CL_INVALID_KERNEL_DEFINITION: std::cout << "CL_INVALID_KERNEL_DEFINITION" << std::endl; break;
			case CL_INVALID_KERNEL_NAME: std::cout << "CL_INVALID_KERNEL_NAME" << std::endl; break;
			case CL_INVALID_LINKER_OPTIONS: std::cout << "CL_INVALID_LINKER_OPTIONS" << std::endl; break;
			case CL_INVALID_MEM_OBJECT: std::cout << "CL_INVALID_MEM_OBJECT" << std::endl; break;
			case CL_INVALID_OPERATION: std::cout << "CL_INVALID_OPERATION" << std::endl; break;
			case CL_INVALID_PIPE_SIZE: std::cout << "CL_INVALID_PIPE_SIZE" << std::endl; break;
			case CL_INVALID_PLATFORM: std::cout << "CL_INVALID_PLATFORM" << std::endl; break;
			case CL_INVALID_PROGRAM: std::cout << "CL_INVALID_PROGRAM" << std::endl; break;
			case CL_INVALID_PROGRAM_EXECUTABLE: std::cout << "CL_INVALID_PROGRAM_EXECUTABLE" << std::endl; break;
			case CL_INVALID_PROPERTY: std::cout << "CL_INVALID_PROPERTY" << std::endl; break;
			case CL_INVALID_QUEUE_PROPERTIES: std::cout << "CL_INVALID_QUEUE_PROPERTIES" << std::endl; break;
			case CL_INVALID_SAMPLER: std::cout << "CL_INVALID_SAMPLER" << std::endl; break;
			case CL_INVALID_SPEC_ID: std::cout << "CL_INVALID_SPEC_ID" << std::endl; break;
			case CL_INVALID_VALUE: std::cout << "CL_INVALID_VALUE" << std::endl; break;
			case CL_INVALID_WORK_DIMENSION: std::cout << "CL_INVALID_WORK_DIMENSION" << std::endl; break;
			case CL_INVALID_WORK_GROUP_SIZE: std::cout << "CL_INVALID_WORK_GROUP_SIZE" << std::endl; break;
			case CL_INVALID_WORK_ITEM_SIZE: std::cout << "CL_INVALID_WORK_ITEM_SIZE" << std::endl; break;
			case CL_KERNEL_ARG_INFO_NOT_AVAILABLE: std::cout << "CL_KERNEL_ARG_INFO_NOT_AVAILABLE" << std::endl; break;
			case CL_LINK_PROGRAM_FAILURE: std::cout << "CL_LINK_PROGRAM_FAILURE" << std::endl; break;
			case CL_LINKER_NOT_AVAILABLE: std::cout << "CL_LINKER_NOT_AVAILABLE" << std::endl; break;
			case CL_MAP_FAILURE: std::cout << "CL_MAP_FAILURE" << std::endl; break;
			case CL_MEM_COPY_OVERLAP: std::cout << "CL_MEM_COPY_OVERLAP" << std::endl; break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE: std::cout << "CL_MEM_OBJECT_ALLOCATION_FAILURE" << std::endl; break;
			case CL_MISALIGNED_SUB_BUFFER_OFFSET: std::cout << "CL_MISALIGNED_SUB_BUFFER_OFFSET" << std::endl; break;
			case CL_OUT_OF_HOST_MEMORY: std::cout << "CL_OUT_OF_HOST_MEMORY" << std::endl; break;
			case CL_OUT_OF_RESOURCES: std::cout << "CL_OUT_OF_RESOURCES" << std::endl; break;
			case CL_MAX_SIZE_RESTRICTION_EXCEEDED: std::cout << "CL_MAX_SIZE_RESTRICTION_EXCEEDED" << std::endl; break;
			case CL_PROFILING_INFO_NOT_AVAILABLE: std::cout << "CL_PROFILING_INFO_NOT_AVAILABLE" << std::endl; break;
			default: std::cout << "Unknown error " << error_code << std::endl;
		}
	}

	if (error_code != 0) 
		exit(1);
}
