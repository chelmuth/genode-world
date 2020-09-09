/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

#include <base/mutex.h>
#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

#include <libc/component.h>
#include <timer_session/connection.h>
#include "greeter_server.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ResourceQuota;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {

		Genode::Env& _env;
		unsigned int _next_session_id { 0 };
		Genode::Mutex _session_id_mutex { };

		unsigned int _get_session_id()
		{
			Genode::Mutex::Guard guard { _session_id_mutex };
			++_next_session_id;
			return _next_session_id;
		}

  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    printf("say hello num=%u\n", _get_session_id());
    std::string prefix(50000, 'a');
    reply->set_message(prefix + request->name());
//    usleep(500'000);
    return Status::OK;
  }

	public:
		GreeterServiceImpl(Genode::Env& env)
			: _env(env)
		{
		}
};

void RunServer(Genode::Env& env, const char* server_address) {
  GreeterServiceImpl service(env);

  ServerBuilder builder;
  ResourceQuota quota;
  quota.SetMaxThreads(32);
  builder.SetResourceQuota(quota);
  builder.AddChannelArgument(GRPC_ARG_MAX_CONCURRENT_STREAMS, 24);
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

