/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 12:59:40 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/12 13:26:31 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "webserv.hpp"
# include "Response.hpp"
# include "Request.hpp"
# include "Webserver.hpp"

class Webserver;

class Client
{
	private:
		int					_socket;
		struct sockaddr_in	_addr;
		Webserver 			*_server;
		Request				*_request;
		Response			*_response;

		Client();

	public:
		Client(int socket, struct sockaddr_in addr);
		Client( Client const & src );
		~Client();

		Client &			operator=( Client const & rhs );

		/* Setters */
		void				reset();
		void				setRequest(Request* request);
		void				setResponse(Response* response);
		void				deleteRequest(void);
		void				setServer(Webserver* server);

		/* Accessors */
		Request*			getRequest();
		Response*			getResponse();
		Webserver*			getServer();
		int					getSocket();
		unsigned short		getPort();
		unsigned long		getIPAddress();
};