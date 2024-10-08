/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValidConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/12 18:47:56 by yliew             #+#    #+#             */
/*   Updated: 2024/07/26 15:32:23 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ValidConfig.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ValidConfig::ValidConfig()
{
	this->_port = -1;
	this->_host = "";
	this->_body_max_length = 5000;
	this->_autoindex = false;
	this->_address_info = NULL;
	this->_root = "./";
	this->_allowed_methods.push_back("GET");
	this->_allowed_methods.push_back("POST");
	this->_index = "index.html";
}

ValidConfig::ValidConfig(const ValidConfig& other)
{
	*this = other;
}

ValidConfig&	ValidConfig::operator=(const ValidConfig& other)
{
	if (this != &other)
	{
		this->_directives = other._directives;
		this->_port = other._port;
		this->_body_max_length = other._body_max_length;
		this->_autoindex = other._autoindex;
		this->_address_info = other._address_info;
		this->_host = other._host;
		this->_root = other._root;
		this->_redirect = other._redirect;
		this->_server_name = other._server_name;
		this->_index = other._index;
		this->_allowed_methods = other._allowed_methods;
	}
	return (*this);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ValidConfig::~ValidConfig()
{
	if (this->_address_info)
		freeaddrinfo(this->_address_info);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/* Port number range: 0 to 65353 */
void	ValidConfig::parseListenPort(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	this->_port = strToInt(tokens[0]);

	if (this->_port < 0 || this->_port > 65353)
		throw InvalidConfigError("Listening port must be a number from 0 to 65353");
}

void	ValidConfig::parseBodyMaxLength(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	this->_body_max_length = strToSizet(tokens[0]);
}

void	ValidConfig::parseAutoindex(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);
	if (tokens[0] == "on")
		this->_autoindex = true;
	else if (tokens[0] == "off")
		this->_autoindex = false;
	else
		throw InvalidConfigError("Invalid param for autoindex");
}

void	ValidConfig::parseHost(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	if (tokens[0] == "localhost")
		this->_host = "127.0.0.1";
	else
		this->_host = tokens[0];

	parseAddressInfo(this->_host, intToStr(this->_port));
}

void	ValidConfig::parseAddressInfo(std::string& host, std::string port)
{
	struct addrinfo hints;
	struct addrinfo	*result;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int	res = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
	if (res != 0)
		throw InvalidConfigError(gai_strerror(res));

	this->_address_info = result;
}

void	ValidConfig::parseRoot(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	std::string	root(tokens[0]);
	if (root[root.size() - 1] != '/')
		root += "/";

	this->_root = root;
}

void	ValidConfig::parseRedirect(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	std::string	redirect(tokens[0]);
	if (redirect.size() > 0 && redirect[0] != '/')
		redirect = "/" + redirect;

	this->_redirect = redirect;
}

void	ValidConfig::parseServerName(const t_strvec& tokens)
{
	this->_server_name = tokens;
}

void	ValidConfig::parseIndex(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	this->_index = tokens[0];
}

void	ValidConfig::parseAllowedMethods(const t_strvec& tokens)
{
	if (tokens.empty())
		return ;

	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] != "GET" && tokens[i] != "POST" && tokens[i] != "DELETE")
			throw InvalidConfigError("Invalid method");
	}
	this->_allowed_methods = tokens;
}

void	ValidConfig::parseErrorPages(const t_strvec& tokens)
{
	for (size_t i = 0; i < tokens.size(); i++)
	{
		int	delimiter = tokens[i].find("=");
		std::string	status = tokens[i].substr(0, delimiter);
		int	status_code = strToInt(status);

		std::string	error_page = tokens[i].substr(delimiter + 1, std::string::npos);

		this->_error_page[status_code] = error_page;
	}
}

/*
** ---------------------------------- UTILS -----------------------------------
*/

int	ValidConfig::strToInt(const std::string& str)
{
	std::stringstream	stream(str);
	int	nb;
	stream >> nb;

	if (!stream.eof() || stream.fail())
		throw InvalidConfigError("Non-numeric parameter");
	return (nb);
}

int	ValidConfig::strToSizet(const std::string& str)
{
	std::stringstream	stream(str);
	size_t	nb;
	stream >> nb;

	if (!stream.eof() || stream.fail())
		throw InvalidConfigError("Non-numeric parameter");
	return (nb);
}

std::string	ValidConfig::intToStr(const int nb)
{
	std::stringstream	stream;
	stream << nb;

	return (stream.str());
}

bool	ValidConfig::isStatusCode(const std::string& str)
{
	std::stringstream	stream(str);
	int	nb;
	stream >> nb;

	if (!stream.eof() || stream.fail())
		return (false);
	if (nb < 100 || nb > 599)
		return (false);
	return (true);
}

/*
** -------------------------------- ACCESSORS ---------------------------------
*/

t_strmap&	ValidConfig::getDirectives(void)
{
	return (this->_directives);
}

int	ValidConfig::getPort(void)
{
	return (this->_port);
}

int	ValidConfig::getBodyMaxLength(void)
{
	return (this->_body_max_length);
}

bool	ValidConfig::getAutoindex(void)
{
	return (this->_autoindex);
}

struct addrinfo	*ValidConfig::getAddressInfo(void)
{
	return (this->_address_info);
}

std::string	ValidConfig::getHost(void)
{
	return (this->_host);
}

std::string	ValidConfig::getRoot(void)
{
	return (this->_root);
}

std::string	ValidConfig::getRedirect(void)
{
	return (this->_redirect);
}

std::string	ValidConfig::getIndex(void)
{
	return (this->_index);
}

t_strvec	ValidConfig::getServerName(void)
{
	return (this->_server_name);
}

t_strvec	ValidConfig::getAllowedMethods(void)
{
	return (this->_allowed_methods);
}

std::string	ValidConfig::getErrorPage(int status_code)
{
	if (this->_error_page.find(status_code) == this->_error_page.end())
		return ("");
	return (this->_error_page[status_code]);
}

/*
** -------------------------------- EXCEPTIONS --------------------------------
*/

ValidConfig::InvalidConfigError::InvalidConfigError(const std::string& message)
	: _message("Invalid config: " + message) {};

ValidConfig::InvalidConfigError::~InvalidConfigError() throw() {}

const char	*ValidConfig::InvalidConfigError::what() const throw()
{
	return (_message.c_str());
}
