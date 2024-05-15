#include "client.h"
#include "guest_client.h"
#include "host_client.h"
#include "user_io.h"

using namespace std::chrono_literals;

int main() {
  UserIO io_object;
  UserIO::Run();
/*  std::shared_ptr<Client> c;

  if (parsed_args["launch-mode"].as<std::string>() == "guest") {
    c = std::make_shared<GuestClient>(parsed_args["address"].as<std::string>(),
                            parsed_args["port"].as<uint16_t>());
  } else {
    c = std::make_shared<HostClient>(parsed_args["port"].as<uint16_t>());
  }
  c->Connect();
*/
}