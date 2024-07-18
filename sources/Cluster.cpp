/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 19:18:04 by yliew             #+#    #+#             */
/*   Updated: 2024/07/17 19:18:06 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"

Cluster*	Cluster::_instance = NULL;
ConfigFile*	Cluster::_config_file = NULL;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Cluster::Cluster() {}

Cluster::Cluster(ConfigFile* config_file)
{
	_instance = this;
	_config_file = config_file;
	signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

	std::vector<ServerConfig*>	servers = this->_config_file->getServers();

	for (size_t i = 0; i < servers.size(); i++)
	{
		Webserver *server = new Webserver(servers[i]);
		this->_servers.push_back(server);
	}

	setSocketFds();
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Cluster::~Cluster()
{
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		delete this->_servers[i];
	}
	delete this->_config_file;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
	struct pollfd {
	   int   fd;         //file descriptor
	   short events;     //requested events
	   short revents;    //returned events
	};
*/
void	Cluster::setSocketFds(void)
{
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		struct pollfd	pfd;

		pfd.fd = this->_servers[i]->getServerSocket();
		pfd.events = POLLIN;
		this->_poll_fds.push_back(pfd);
	}
}

void	Cluster::runServers(void)
{
	// while (true)
	// {
	// 	int	no_of_events = poll
	// }
}

void	Cluster::signal_handler(int signum)
{
	if (_instance)
	{
		delete (_instance);
	}
    std::cout << "\nSignal received, webserver closed. Bye bye!" << std::endl;
    exit(signum);
}
