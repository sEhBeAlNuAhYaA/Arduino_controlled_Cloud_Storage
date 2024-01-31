#pragma once
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include<fstream>
#include <string>
#include <unordered_map>

static int HTTP_BUFFER = 10000;

enum requests_types {
    ArduinoInfo,
    Authorisation,
    Registration,
    SendingAFile,
    TakingAFile,
    DeleteAFile,
    ErrorFromServer,
    RequestsError,
    RequestAnswer
};

inline requests_types req_converter(std::string str_req) {
    if (str_req == "ArduinoInfo") return ArduinoInfo;
    if (str_req == "Authorisation") return Authorisation;
    if (str_req == "Registration") return Registration;
    if (str_req == "SendingAFile") return SendingAFile;
    if (str_req == "TakingAFile") return TakingAFile;
    if (str_req == "ErrorFromServer") return ErrorFromServer;
    if (str_req == "RequestAnswer") return RequestAnswer;
    if (str_req == "DeleteAFile") return DeleteAFile;
    return RequestsError;
}

inline std::string req_back_converter(requests_types type) {
    if (type == ArduinoInfo) return "ArduinoInfo";
    if (type == Authorisation) return "Authorisation";
    if (type == Registration) return "Registration";
    if (type == SendingAFile) return "SendingAFile";
    if (type == TakingAFile) return "TakingAFile";
    if (type == ErrorFromServer) return "ErrorFromServer";
    if (type == RequestAnswer) return "RequestAnswer";
    if (type == DeleteAFile) return "DeleteAFile";
    return "RequestsError";
}

inline void client_or_server_color(std::string var) {
    if (var == "CLIENT") {
        std::cout << "\x1B[38;5;46m-->[CLIENT]\x1B[0m ";
    }
    if (var == "SERVER") {
        std::cout << "\x1B[38;5;200m-->[SERVER]\x1B[0m ";
    }
}

inline void color_client_id(int id) {
    std::cout << "\x1b[38;5;46m- [ID:" << id << "]\x1b[0m ";
}

class Http_Builder {
    char* http_builded;
    int current_length;
public:

    Http_Builder() {
        this->http_builded = new char[HTTP_BUFFER];
        memset(this->http_builded, '\0', HTTP_BUFFER);
        this->current_length = 0;
    }
    
    void filling_an_array(std::string input_string) {
        for (int i = this->current_length; i < input_string.size() + this->current_length; i++) {
            this->http_builded[i] = input_string[i - this->current_length];
        }
        this->current_length += input_string.size();
    }

    void binary_insert(char* binary) {
        for (int i = this->current_length; i < 9000 + this->current_length; i++) {
            this->http_builded[i] = binary[i - this->current_length];
        }
        this->current_length += 9000;
    }

    //RequestsError SendingAFile TakingAFile DeleteAFile
    char* Sending_A_File(std::string name, char* binary_file,size_t size, std::string part) {
		filling_an_array("POST / ");
		filling_an_array(req_back_converter(SendingAFile));
		filling_an_array(" HTTP/1.1\n");
		filling_an_array("\nContent-Name: ");
		filling_an_array(name);
        filling_an_array("\nContent-Length: ");
        filling_an_array(std::to_string(size));
		filling_an_array("\nPart-File: ");
		filling_an_array(part);
		filling_an_array("\n\n{\n");
		binary_insert(binary_file);
		filling_an_array("\n}\n");
		return this->http_builded;
    }

	char* Take_or_Del(requests_types type_of_request, std::string& direction) {
		if (type_of_request == TakingAFile) {
			filling_an_array("GET / ");
			filling_an_array(req_back_converter(type_of_request));
		}

		if (type_of_request == DeleteAFile) {
			filling_an_array("DELETE / ");
			filling_an_array(req_back_converter(type_of_request));
		}
		filling_an_array(" HTTP/1.1\n");
		filling_an_array("Content-Name: ");
		filling_an_array(direction);
		filling_an_array("\n\n");
		return this->http_builded;
	}

	//answer from the server(RequestAnswer)
	char* Builder_Answer(std::string message) {
		filling_an_array("HEAD / ");
		filling_an_array(req_back_converter(RequestAnswer));
		filling_an_array(" HTTP/1.1\n");
		filling_an_array("\n{\n");
		filling_an_array(message);
		filling_an_array("\n}\n");
		return this->http_builded;
	}

    char* Authentification(requests_types type_of_request, std::string login, std::string password) {
		filling_an_array("PUT / ");
        if (type_of_request == Authorisation) filling_an_array(req_back_converter(Authorisation));
        if (type_of_request == Registration) filling_an_array(req_back_converter(Registration));
	    filling_an_array(" HTTP/1.1\n");
		filling_an_array("\n{\nlogin=");
		filling_an_array(login);
		filling_an_array("\npassword=");
		filling_an_array(password);
		filling_an_array("\n}\n");
		return this->http_builded;
    }


	char* Arduino() {
		filling_an_array("HEAD / ");
		filling_an_array(req_back_converter(ArduinoInfo));
		filling_an_array(" HTTP/1.1\n");
		return this->http_builded;
	}

	char* get_HTTP() {
		return this->http_builded;
	}

    int getSize() {
        return this->current_length;
    }

    void clearBuilder() {
        memset(this->http_builded, '\0', HTTP_BUFFER);
        this->current_length = 0;
    }

};
//parsed struct 
struct parsed_request {
    requests_types type;
    std::unordered_map <std::string, std::string> keys_map;
    char* binary_part;

    parsed_request() {
        this->binary_part = new char[9000];
        this->type = Authorisation;
        this->keys_map["Content-Name"] = "";
        this->keys_map["Content-Length"] = "";
        this->keys_map["Part-File"] = "";
        this->keys_map["login"] = "";
        this->keys_map["password"] = "";
        this->keys_map["info"] = "";
    }

    void clear_struct() {
        memset(this->binary_part, '\0', 9000);
        this->keys_map["Content-Name"] = "";
        this->keys_map["Content-Length"] = "";
        this->keys_map["Part-File"] = "";
        this->keys_map["login"] = "";
        this->keys_map["password"] = "";
        this->keys_map["info"] = "";
    }
};

class Http_Parser {
    char* http_parsed;
    parsed_request pars_req;
public:

    Http_Parser() {
        this->http_parsed = new char[HTTP_BUFFER];
    }
    
    void Extract_parts(const char* search_start, const char end_char, const char* map_position) {
        int login_poz = strstr(this->http_parsed, search_start) - this->http_parsed + strlen(search_start);
        for (int i = login_poz; i < strlen(this->http_parsed); i++) {
            if (this->http_parsed[i] == end_char) {
                break;
            }
            this->pars_req.keys_map[map_position] += this->http_parsed[i];
        }
    }

    int find_end_of_binary() {
        for (int i = HTTP_BUFFER; i > 0; i--) {
            if (this->http_parsed[i] == '}') return i;
        }
    }

    void Extract_Binary(const char* search_start) {
        int login_poz = strstr(this->http_parsed, search_start) - this->http_parsed + strlen(search_start);
        for (int i = login_poz; i < find_end_of_binary(); i++) {
            this->pars_req.binary_part[i - login_poz] = this->http_parsed[i];
        }
    }

    parsed_request getPars() {
        return this->pars_req;
    }

    parsed_request Parsing() {
        std::string parsed_request_type = "";
        bool is_parsed_request_type = false;
        //parsing a request type
        for (int i = 4; i < strlen(this->http_parsed); i++) {
            if (http_parsed[i] == ' ') continue;
            if (http_parsed[i] == '/') {
                is_parsed_request_type = true;
                continue;
            }
            if (http_parsed[i] == 'H') {
                is_parsed_request_type = false;
                break;
            }

            if (is_parsed_request_type) {
                parsed_request_type += http_parsed[i];
            }
        }
        this->pars_req.type = req_converter(parsed_request_type);

        if (this->pars_req.type == Authorisation || this->pars_req.type == Registration) {
            this->Extract_parts("login=", '\n', "login");
            this->Extract_parts("password=", '\n', "password");
            return this->pars_req;
        }
        
        if (this->pars_req.type == SendingAFile) {
            this->Extract_parts("Content-Name: ", '\n', "Content-Name");
            this->Extract_parts("Content-Length: ", '\n', "Content-Length");
            this->Extract_parts("Part-File: ", '\n', "Part-File");
            this->Extract_Binary("{\n");
            return this->pars_req;
        }

        if (this->pars_req.type == TakingAFile) {
            this->Extract_parts("Content-Name: ", '\n', "Content-Name");
            return this->pars_req;
        }

        if (this->pars_req.type == DeleteAFile) {
            this->Extract_parts("Content-Name: ", '\n', "Content-Name");
            return this->pars_req;
        }

        if (this->pars_req.type == RequestAnswer) { 
            this->Extract_parts("{\n", '\n', "info");
            return this->pars_req;   
        }
        return this->pars_req;
    }
    
    void setRequest(char* http_parsed) {
        memcpy_s(this->http_parsed, HTTP_BUFFER, http_parsed, HTTP_BUFFER);
    }

    void clearRequest() {
        memset(this->http_parsed, '\0', HTTP_BUFFER);
        memset(this->pars_req.binary_part, '\0', 9000);
        this->pars_req.keys_map["Content-Name"] = "";
        this->pars_req.keys_map["Content-Length"] = "";
        this->pars_req.keys_map["Part-File"] = "";
        this->pars_req.keys_map["login"] = "";
        this->pars_req.keys_map["password"] = "";
        this->pars_req.keys_map["info"] = "";
    }


};
