/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIGet.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/02 20:33:16 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/05 22:00:34 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

# include "CGIHandler.hpp"
# include "Request.hpp"

class CGIGet : public CGIHandler
{
	public:
		CGIGet();
		CGIGet(Request const & request);
		CGIGet( CGIGet const & src );
		~CGIGet();
		CGIGet &		operator=( CGIGet const & rhs );

		void			execute_cgi(std::string path, int pipe_fd[], Request const & request);
		void			process_result_cgi(int pid, int pipe_fd[]);
};