/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:06:01 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/29 14:50:22 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <string>
# include <vector>
# include <sstream>
# include <fstream>
# include <map>
# include <cstdlib>
# include <algorithm>
# include "webserv.hpp"
# include "ServerConfig.hpp"

#define DEFAULT_BODY_MAX 300000

enum connection_type {
	KEEP_ALIVE,
	CLOSE
};

enum error_type {
	NO_ERR,
	INVALID_METHOD,
	INVALID, // 400
	NOT_SUPPORTED, // 405
	CHUNK_AND_LENGTH,
	POST_MISSING_BODY,
	INVALID_SIZE,
	INVALID_EMPTY_REQ,
	BODY_TOO_LONG,
	NO_HOST,
	INVALID_PORT,
	TIMEOUT_ERR
};

class Request
{
	private:
		Webserver							*_server;
		time_t								_timeout;
		std::vector<unsigned char>			_raw;
		ssize_t								_header_length;
		bool								_req_complete;
		ssize_t								_body_max_length;
		ssize_t								_content_length;
		bool								_is_chunked;
		bool 								_encoding_chunked;
		std::string							_method;
		std::string							_path;
		std::string							_http_version;
		std::string							_content_type;
		std::string							_boundary;
		std::string							_query;
		std::string							_host;
		int									_port;
		std::map<std::string, std::string>	_headers;
		std::vector<unsigned char>			_body;
		error_type							_error;
		ServerConfig*						_config;
		std::map<std::string, std::string>	_formData;
		std::map<std::string, std::string> 	_bodyMap;
		std::map<std::string, std::string>	_fileMap;


		// Member functions
		int 		parseRequest(std::string header);
		void 		parseHeader(std::string header);
		void 		parsePort(std::string header);
		void		parseContentType();
		void		parseQuery();

		void 		checkMethod();
		void 		checkPath();
		std::string extractHeader();
		void		appendChunkedBody(char *buffer, int bytes_read);
		void		getInitialChunk(std::vector<unsigned char>::iterator body_start);

		void		initBody();
		void 		initRequest();

		// Helper function
		size_t		convert_sizet(std::string str);
		bool 		is_header_complete();
		void		boundary_found();
		void		copyRawRequest(char *buf, int bytes_read);
		bool 		findSequence(const std::vector <unsigned char> &vec, \
			const std::vector<unsigned char>& seq);
		void 		print_error(std::string msg);

		// Error handling
		void 			printError(std::string error_msg);

	public:
		Request();
		Request(char *full_request, int bytes_read);
		Request( Request const & src );
		~Request();

		Request &							operator=( Request const & rhs );
		std::vector<unsigned char>			getRaw() const;
		std::string							getPath() const;
		std::string							getHttpVersion() const;
		std::string							getMethod() const;
		std::vector<unsigned char>			getBody() const;
		std::string							getHost() const;
		int									getPort() const;
		std::map<std::string, std::string>	getHeaders() const;
		ssize_t								getHeaderLength() const;
		bool								getReqComplete() const;
		error_type							getError() const;
		Webserver*							getServer();
		time_t								getTimeout();
		bool								hasQuery() const;
		std::string							getQuery() const;
		void								setBodyMaxLength(size_t len);
		void								setServer(Webserver* server);
		void								setError(error_type err);
		void								setReqComplete(bool complete);

		void		handle_incomplete_header(int bytes_read, char *buffer);
		bool		handle_chunk(char *buffer, int bytes_read);
		static void	parseHostPort(char *buffer, std::string& host, int& port);
		void 		checkBodyLength();

		// Debug
		void 			print_variables() const;
		static void		printMap(std::map<std::string, std::string> map);
		void 			print_vector(std::vector<unsigned char> vec);
};