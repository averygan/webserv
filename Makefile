# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/09 14:32:15 by agan              #+#    #+#              #
#    Updated: 2024/08/13 10:43:44 by jbarbay          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# program name
NAME	= webserv

# compiler
CC		= c++
CFLAGS	= -Wall -Wextra -Werror -std=c++98 -g
DFLAGS	= -MMD -MP # handle header dependencies
RM		= rm -fr

# directories
INC_DIR = ./includes \
		./includes/config
INC 	= $(addprefix -I, $(INC_DIR))
SRC_DIR = ./sources
BUILD_DIR = ./sources/temp
CONFIG_DIR = ./sources/config

# build files
CONFIG_SRCS = $(addprefix $(CONFIG_DIR)/, \
		ConfigFile.cpp \
		ValidConfig.cpp \
		ServerConfig.cpp \
		LocationConfig.cpp)
SRCS	= $(addprefix $(SRC_DIR)/, \
		main.cpp \
		Webserver.cpp \
		Cluster.cpp \
		Client.cpp \
		Request.cpp \
		Response.cpp \
		CGIHandler.cpp \
		CGIGet.cpp \
		CGIPost.cpp) $(CONFIG_SRCS)
OBJS    = $(addprefix $(BUILD_DIR)/, $(notdir $(SRCS:.cpp=.o)))
DEPS    = $(OBJS:.o=.d)

#------------------------------------------------------------------------

# colours
GREEN = \033[0;32m
B_GREEN = \033[1;32m
BROWN = \033[0;33m
B_BROWN = \033[1;33m
END = \033[0m

#------------------------------------------------------------------------

all: $(NAME)

# build program
$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(B_GREEN)$(NAME) compiled.$(END)"

# build objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(INC)
	@echo "$(B_GREEN)$< compiled.$(END)"

$(BUILD_DIR)/%.o: $(CONFIG_DIR)/%.cpp | $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(INC)
	@echo "$(B_GREEN)$< compiled.$(END)"

# build directory to store objects and dependency files
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# clean rules
clean:
	@rm -fr $(BUILD_DIR)
	@echo "$(B_GREEN)clean completed.$(END)"

fclean: clean
	@rm -fr $(NAME)
	@echo "$(B_GREEN)fclean completed.$(END)"

re: fclean all

-include $(DEPS)

#------------------------------------------------------------------------

.PHONY: all clean fclean re
