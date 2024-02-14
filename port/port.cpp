#include <iostream>
#include <boost/asio.hpp>
#include <string>

using namespace boost::asio;
using namespace std;

int main()
{
            io_context io;
            boost::system::error_code com_error;

            serial_port com(io);
            com.open("COM7", com_error);

            if (com_error) {
                if (com_error == errc::permission_denied) {
                    cout << "COM-port is busy!" << endl;
                }
                else if (com_error == errc::no_such_file_or_directory) {
                    cout << "Not connected(Arduino)";
                }
                else {
                    cout << com_error.what();
                }
                return 1;
            }



            com.set_option(serial_port_base::baud_rate(57600));
            string buf;
            while (true) {
                getline(cin, buf);
                if (buf.size()>1) {
                    buf += '$';
                    write(com, boost::asio::buffer(buf));
                }
            }
                  
      
    return 0;
}
