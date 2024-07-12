/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:41:44 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/11 15:48:05 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserver.hpp"

Webserver* Webserver::_instance = NULL;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

void	Webserver::initialize(int domain, int type, int protocol, int port, u_long interface, int backlog)
{
	// Address
	_address.sin_family = domain;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = htonl(interface);
	
	// Create socket
	_server_socket = socket(domain, type, protocol);
	check(_server_socket);

	// Options to be able to reuse address.
	int	tr = 1;
	check(setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)));

	// Bind the socket to an address and a port
	check(bind(_server_socket, (struct sockaddr*)&_address, sizeof(_address)));

	// Listen: wait for the client to make a connection
	check(listen(_server_socket, backlog) < 0);
}

Webserver::Webserver()
{
	signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
	_instance = this;
	
	initialize(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 1024);
}

Webserver::Webserver(int domain, int type, int protocol, int port, u_long interface, int backlog)
{
	signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
	_instance = this;

	initialize(domain, type, protocol, port, interface, backlog);
}

Webserver::Webserver( const Webserver & src ):
_server_socket(src._server_socket),
_address(src._address),
_current_sockets(src._current_sockets),
_clients(src._clients)
{

}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Webserver::~Webserver()
{
	close(_server_socket);
	FD_CLR(_server_socket, &_current_sockets);
	FD_ZERO(&_current_sockets);

	std::map<int, Client*>::iterator	it;
	
	for (it = _clients.begin() ; it != _clients.end(); it++)
	{
		delete (it->second);
	}
	_clients.clear();
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Webserver &				Webserver::operator=( Webserver const & rhs )
{
	if ( this != &rhs )
	{
		this->_server_socket = rhs._server_socket;
		this->_address = rhs._address;
		this->_current_sockets = rhs._current_sockets;
		this->_instance = rhs._instance;
		this->_clients = rhs._clients;
	}
	return (*this);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

int	Webserver::accept_new_connections(void)
{
	socklen_t addrlen = sizeof(_address);

	int	new_socket = accept(_server_socket, (struct sockaddr*)&_address, &addrlen);
	if (new_socket < 0)
	{
		std::cerr << strerror(errno);
		exit(1);
	}
	_clients[new_socket] = new Client(new_socket);
	return (new_socket);
}

void	Webserver::check(int num)
{
	if (num < 0)
	{
		std::cerr << strerror(errno);
		exit(1);
	}
}

void Webserver::signal_handler(int signum)
{
	if (_instance)
	{
		delete (_instance);
	}
    std::cout << "\nSignal received, webserver closed. Bye bye!" << std::endl;
    exit(signum);
}

// void	Webserver::run(void)
// {
// 	fd_set	read_sockets, write_sockets;
// 	int		client_socket;
// 	int		max_socket = _server_socket;

// 	FD_ZERO(&_current_sockets);
// 	FD_SET(_server_socket, &_current_sockets);

// 	while (true)
// 	{
// 		read_sockets = _current_sockets;
// 		write_sockets = _current_sockets;
		
// 		if (select(max_socket + 1, &read_sockets, &write_sockets, NULL, NULL) < 0)
// 		{
// 			std::cerr << strerror(errno);
// 			exit(1);
// 		}
// 		int	i = 0;
// 		while (i <= max_socket)
// 		{
// 			if (FD_ISSET(i, &read_sockets))
// 			{
// 				if (i == _server_socket) // New connection
// 				{
// 					client_socket = accept_new_connections();
// 					FD_SET(client_socket, &_current_sockets);
// 					if (client_socket > max_socket)
// 						max_socket = client_socket;
// 				}
// 				else
// 				{
// 					handle_read_connection(i);
// 					FD_CLR(i, &_current_sockets);
// 				}
// 			}
// 			if (FD_ISSET(i, &write_sockets))
// 			{
// 				handle_write_connection(i);
// 				//  Then delete client ???
// 			}
// 			i++;
// 		}
// 	}
// }

void	Webserver::run(void)
{
    std::vector<pollfd> poll_fds;
    pollfd server_fd;
    server_fd.fd = _server_socket;
    server_fd.events = POLLIN;
    poll_fds.push_back(server_fd);

	while (true)
	{
	    int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);

	    if (poll_count < 0)
	    {
	    	std::cout << "poll count is < 0" << std::endl;
	        std::cerr << strerror(errno);
	        exit(1);
	    }

	    for (size_t i = 0; i < poll_fds.size(); i++)
	    {
	        if (poll_fds[i].revents & POLLIN)
	        {
	            if (poll_fds[i].fd == _server_socket) // New connection
	            {
	            	int client_socket = accept_new_connections();
	            	if (client_socket >= 0)
	            	{
		                pollfd client_fd;
		                client_fd.fd = client_socket;
		                client_fd.events = POLLIN | POLLOUT;
		                poll_fds.push_back(client_fd);
	               	}
	            }
	            else
	            {
	                if (handle_read_connection(poll_fds[i].fd) == -1)
	                {
	                    close(poll_fds[i].fd);
	                    poll_fds.erase(poll_fds.begin() + i);
	                    i--; // adjust index after erase
	                }
	                else
						poll_fds[i].events &= ~POLLIN; // Clear the read event
	            }
	        }

	        if (poll_fds[i].revents & POLLOUT)
	        {
	        	std::cout << "writing to client" << std::endl;
	            handle_write_connection(poll_fds[i].fd);
	            poll_fds[i].events &= ~POLLOUT; // Clear the write event
	        }
	    }
	}
}

int	Webserver::handle_read_connection(int client_socket)
{
	char	buffer[BUFFER_SIZE];
	int		bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);

	if (bytes_read < 0)
	{
		std::cerr << strerror(errno) << std::endl;
		return -1;
	}
	if (bytes_read == 0)
	{
		std::cout << "Client closed the connection\n";
		return -1;
	}
	Request*	request = new Request(buffer);
	getClient(client_socket)->setRequest(*request);

	std::cout << request->getFullRequest() << std::endl;

	Response	*response = new Response(request);
	getClient(client_socket)->setResponse(*response);
	return 0;
}

void		Webserver::handle_write_connection(int client_socket)
{
	Client	*client = getClient(client_socket);

	if (!client->getResponse())
	{
		return ;
	}

	send(client->getSocket(), client->getResponse()->getFullResponse().c_str(), client->getResponse()->getFullResponse().size() + 1, 0);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int	Webserver::getServerSocket()
{
	return (_server_socket);
}

struct sockaddr_in	Webserver::getAddress()
{
	return (_address);
}

fd_set	Webserver::getCurrentSockets()
{
	return (_current_sockets);
}

Webserver* Webserver::getInstance()
{
	return (_instance);
}

std::map<int, Client*>		Webserver::getClients()
{
	return (_clients);
}

Client*		Webserver::getClient(int socket)
{
	return (_clients[socket]);
}

/* ************************************************************************** */