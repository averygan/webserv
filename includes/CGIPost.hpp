/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIPost.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/05 21:15:42 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/12 15:37:17 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "CGIHandler.hpp"

class CGIHandler;

class CGIPost : public CGIHandler
{
	private:
		std::string		_cgi_exec;

		CGIPost();

	public:
		CGIPost(Request const & request, LocationConfig* location, std::string ext);
		CGIPost(CGIPost const & src);
		~CGIPost();
		CGIPost &		operator=( CGIPost const & rhs );

		void			execute_cgi(int pipe_fd[], int pipe_data[], Request const & request);
		void			process_result_cgi(int pid, int pipe_fd[], int pipe_data[], Request const & request);
};