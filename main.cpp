#include <iostream>
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.hpp>
#include "analytical.h"
#include "utils.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>
#include <iomanip>

using namespace std;

enum Type {Call, Put, Call2, Put2};

typedef struct Simulation {
	cl::Context context;
	cl::Program program;
	cl::Device device;
	cl::Buffer spot;
	const size_t nbr_traj;
} Simulation;

typedef struct Option {
	const float value;
	const float error;
} Option;

vector<float> get_value(const Type& option_type, const vector<float>& strikes, const Simulation& simulation);

vector<Option> get_error(const Type& option_type, const vector<float>& strikes, const Simulation& simulation);

int main() {
	vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);

	if (all_platforms.size() == 0) {
		cout << "No platform found." << endl;
		exit(-1);
	}
	else {
		cout << "Number of platforms found: " << all_platforms.size() << endl;
	}

	cl::Platform default_platform = all_platforms[0];
	cout << default_platform.getInfo<CL_PLATFORM_NAME>() << endl;

	vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, & all_devices);

	if (all_devices.size() == 0) {
		cout << "No device found" << endl;
		exit(-1);
	}

	cl::Device default_device = all_devices[0];
	cout << "Using device " << default_device.getInfo<CL_DEVICE_NAME>() << endl;
	cout << "OpenCL version " << default_device.getInfo<CL_DEVICE_OPENCL_C_VERSION>() << endl;

	cl::Context context({default_device});

	cl::Program::Sources sources;

	ifstream kernel_file("kernel_test.c");
	
	if (kernel_file.fail()) {
		cout << "Failed to open kernel file" << endl;
		exit(-1);
	}

	std::stringstream kernel_buffer;
	kernel_buffer << kernel_file.rdbuf();
	kernel_file.close();

	const string kernel_code = kernel_buffer.str();
	sources.push_back({kernel_code.c_str(), kernel_code.length()}); 

	cl::Program program(context, sources);

	if (program.build({default_device}) != CL_SUCCESS) {
		cout << "Error building the program " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << endl;
		exit(-1);
	}
	cl_int error;
	
	cl::CommandQueue queue(context, default_device);

	const size_t N = size_t(pow(2, 20));
	const float S0 = 100.f;
	const float T = 0.5f;
	const int nbr_timesteps = int(pow(2, 15));
	const float dt = T / (float)nbr_timesteps;
	const float sigma0 = 0.1f;
	const float sigbar = sigma0 * sqrt(dt);
	const float S0_sigbar = S0 * sigbar;

	auto startTime = std::chrono::high_resolution_clock::now();

	vector<float> S(N, S0);
	vector<size_t> counter(N, 0);

	cl::Buffer buffer_counter(context, CL_MEM_READ_WRITE, sizeof(size_t)*N, NULL, &error);
	check_error(error, "Create and copy the buffer for streams");

	cl::Buffer buffer_S(context, CL_MEM_READ_WRITE, sizeof(float)*N, NULL, &error);
	check_error(error, "Create buffer S");

	cl::Buffer buffer_S0(context, CL_MEM_READ_ONLY, sizeof(float), NULL, &error);
	check_error(error, "Create buffer S0");

	cl::Buffer buffer_sigbar(context, CL_MEM_READ_ONLY, sizeof(float), NULL, &error);
	check_error(error, "Create buffer sigbar");

	cl::Buffer buffer_nbr_timesteps(context, CL_MEM_READ_ONLY, sizeof(int), NULL, &error);
	check_error(error, "Create buffer nbr_timesteps");

	queue.enqueueWriteBuffer(buffer_counter, CL_TRUE, 0, sizeof(size_t)*N, counter.data());
	queue.enqueueWriteBuffer(buffer_S, CL_TRUE, 0, sizeof(float)*N, S.data());
	queue.enqueueWriteBuffer(buffer_S0, CL_TRUE, 0, sizeof(float), &S0);
	queue.enqueueWriteBuffer(buffer_sigbar, CL_TRUE, 0, sizeof(float), &S0_sigbar);
	queue.enqueueWriteBuffer(buffer_nbr_timesteps, CL_TRUE, 0, sizeof(int), &nbr_timesteps);
	
	cl::Event event;
	cl::Kernel kernel_bachelier_vanilla = cl::Kernel(program, "bachelier_vanilla", &error);
	check_error(error, "Kernel bachelier vanilla");
	kernel_bachelier_vanilla.setArg(0, buffer_counter);	
	kernel_bachelier_vanilla.setArg(1, buffer_S);	
	kernel_bachelier_vanilla.setArg(2, buffer_S0);	
	kernel_bachelier_vanilla.setArg(3, buffer_sigbar);	
	kernel_bachelier_vanilla.setArg(4, buffer_nbr_timesteps);	
	error = queue.enqueueNDRangeKernel(kernel_bachelier_vanilla, 
					   cl::NullRange, 
					   cl::NDRange(N), 
					   cl::NullRange,
					   NULL,
					   &event);
	queue.finish();
	check_error(error, "Queue");

	event.wait();
	auto endTime = std::chrono::high_resolution_clock::now();
	cout << "N: " << N << endl;
	if (event.getInfo<CL_EVENT_COMMAND_EXECUTION_STATUS>() == CL_COMPLETE)
		cout << "MC finished" << endl;
	
	const Simulation simulation = {.context = context, 
				       .program = program,
				       .device = default_device,
				       .spot = buffer_S,
				       .nbr_traj = N
	};

	const size_t N_K = 32;
	const float K_start = 95.f;
	const float K_end   = 105.f;
	const float K_step = (K_end - K_start) / (N_K-1);
	vector<float> Ks;

	for (size_t i=0; i < N_K; ++i) {
		Ks.push_back( K_start + K_step * i);
	}
	
	//vector<float> call_values = get_value(Call, Ks, simulation);
	//vector<float> put_values = get_value(Put, Ks, simulation);
	vector<Option> call_ve = get_error(Call, Ks, simulation);
	vector<Option> put_ve = get_error(Put, Ks, simulation);

	cout << setw(10) << "Strike: " << setw(37) << "MC call(error) | put(error)" << setw(28) << "Analytical" << endl;
	for (size_t i=0; i<N_K; ++i) {
		cout << setw(8) <<
			   Ks[i] << ": " << setw(13) << 
			   call_ve[i].value << "(" << call_ve[i].error << ")" << 
			   "|" << 
			   put_ve[i].value  << "(" << put_ve[i].error  << ")" << setw(14) << 
			   bachelier_call(S0, sigma0, T, Ks[i]) << "|" << bachelier_put(S0, sigma0, T, Ks[i]) << setw(5) << endl;
	}
	
	cout << "Time elpased (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() <<endl;

	/* Save some data for the plot
	queue.enqueueReadBuffer(buffer_S, CL_FALSE, 0, sizeof(float) * N, S.data());

	auto output = fstream("S.dat", ios::out | ios::binary);
	output.write((char*)S.data(), sizeof(float)*N);
	output.close();

	auto output_option = fstream("option.dat", ios::out | ios::binary);
	output_option.write((char*)Ks.data(), sizeof(float)*N_K);
	output_option.write((char*)call_ve.data(), sizeof(float)*N_K*2);
	output_option.write((char*)put_ve.data(), sizeof(float)*N_K*2);
	output_option.close();
	*/

	cout << "GRN/s: " << double(N * nbr_timesteps)*1e-6 / chrono::duration_cast<chrono::milliseconds>(endTime -startTime).count() << endl;

	return 0;
}

///////////////////////// FUNCTION DEFINITION ///////////////////////////////////////////////////

vector<float> get_value(const Type& option_type, const vector<float>& strikes, const Simulation& simulation) {
	int error;
	vector<float> result(strikes.size(), 0);
	const size_t nbr_work_group = 128;
	const size_t nbr_work_item_per_group = 128;
	const size_t nbr_work_item = nbr_work_group * nbr_work_item_per_group;

	vector<vector<float>> first_reduction(strikes.size(), vector<float>(nbr_work_group, 0));
	vector<cl::CommandQueue> queues(strikes.size(), cl::CommandQueue(simulation.context, simulation.device));

	vector<cl::Buffer> buffer_reductions;
	for (size_t i=0; i<strikes.size(); ++i) {
		buffer_reductions.push_back( cl::Buffer(simulation.context, 
						        CL_MEM_COPY_HOST_PTR,
							sizeof(float),
							&result[i],
							&error) );
	}
	
	for (size_t i=0; i<strikes.size(); ++i) {
		cl::Buffer buffer_reduction_vanilla(simulation.context,
						    CL_MEM_COPY_HOST_PTR, 
						    sizeof(float)*nbr_work_group, 
						    first_reduction[i].data(), 
						    &error);
		//check_error(error, "Create buffer reduction vanilla");
		
		cl::Kernel kernel_vanilla;
		if (option_type == Call)
			kernel_vanilla = cl::Kernel(simulation.program, "vanilla_call", &error);
		else if (option_type == Put)
			kernel_vanilla = cl::Kernel(simulation.program, "vanilla_put", &error);
		else if (option_type == Call2)
			kernel_vanilla = cl::Kernel(simulation.program, "vanilla_call_squared", &error);
		else if (option_type == Put2)
			kernel_vanilla = cl::Kernel(simulation.program, "vanilla_put_squared", &error);
		else {
			cout << "Unknown option type" << endl;
			exit(-1);
		}
		//check_error(error, "Kernel reduction vanilla");
		kernel_vanilla.setArg(0, simulation.spot);
		kernel_vanilla.setArg(1, buffer_reduction_vanilla);
		kernel_vanilla.setArg(2, sizeof(cl_float)*nbr_work_item_per_group, NULL);
		kernel_vanilla.setArg(3, (int)simulation.nbr_traj);
		kernel_vanilla.setArg(4, strikes[i]);

		error = queues[i].enqueueNDRangeKernel(kernel_vanilla,
						       cl::NullRange, 
						       cl::NDRange(nbr_work_item), 
						       cl::NDRange(nbr_work_item_per_group));
		//check_error(error, "Queue vanilla");

		cl::Kernel kernel_reduction = cl::Kernel(simulation.program, "reduction", &error);
		//check_error(error, "Kernel reduction");
		kernel_reduction.setArg(0, buffer_reduction_vanilla);
		kernel_reduction.setArg(1, buffer_reductions[i]);
		kernel_reduction.setArg(2, sizeof(float)*nbr_work_group, NULL);
		kernel_reduction.setArg(3, (int)nbr_work_group);
		error = queues[i].enqueueNDRangeKernel(kernel_reduction,
						       cl::NullRange,
						       cl::NDRange(nbr_work_group),
						       cl::NDRange(nbr_work_group));
		//check_error(error, "Queue reduction");
	}

	for (size_t i=0; i<strikes.size(); ++i) {
		queues[i].finish();
		queues[i].enqueueReadBuffer(buffer_reductions[i], 
					    CL_TRUE, 
					    0, 
					    sizeof(float), 
					    &result[i]);
	}
	
	for (auto& el : result)
		el /= simulation.nbr_traj;

	return result;
}

vector<Option> get_error(const Type& option_type, const vector<float>& strikes, const Simulation& simulation) {
	if ((option_type == Call2) || (option_type == Put2)) {
		cout << "Cannot compute the error of the error" << endl;
		exit(-1);
	}

	vector<float> option_value;
	vector<float> option_value2;
	if (option_type == Call) {
		option_value  = get_value(option_type, strikes, simulation);
		option_value2 = get_value(Call2, strikes, simulation);
	}
	else if (option_type == Put) {
		option_value  = get_value(option_type, strikes, simulation);
		option_value2 = get_value(Put2, strikes, simulation);
	}
	else {
		cout << "Unknown option type" << endl;
		exit(-1);
	}

	vector<Option> result;

	for (size_t i=0; i<strikes.size(); ++i) {
		const float std = sqrt((option_value2[i] - pow(option_value[i], 2))/simulation.nbr_traj);
		result.push_back({.value = option_value[i], .error = std});
	}
		
	return result;
}

