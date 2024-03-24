#include <cxxopts.hpp>

#include "client.h"
#include "guest_client.h"
#include "host_client.h"
#include "user_io.h"

using namespace std::chrono_literals;

int main(int argc, char** argv) {
  cxxopts::Options options("Battleship", "description");

  options.add_options()  // clang-format off
      ("a,address", "Address to connect to", cxxopts::value<std::string>())
      ("p,port", "Port to connect to", cxxopts::value<uint16_t>())
      ("launch-mode", "description", cxxopts::value<std::string>());  // clang-format on
  options.parse_positional("launch-mode");
  auto parsed_args = options.parse(argc, argv);

  std::shared_ptr<Client> c;

  if (parsed_args["launch-mode"].as<std::string>() == "guest") {
    c = GuestClient::Create(parsed_args["address"].as<std::string>(),
                            parsed_args["port"].as<uint16_t>());
  } else {
    c = HostClient::Create(parsed_args["port"].as<uint16_t>());
  }
  c->Connect();
  UserIO io_object;
  UserIO::Run(c);
}