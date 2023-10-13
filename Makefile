NAME = ircserv

FILES = main.cpp IrcServer.cpp UserConn.cpp parser.cpp Channel.cpp 

#SRCDIR = ./src/
OBJDIR = ./obj/
BINDIR = ./bin/
#INCDIR = ./inc/

OBJ = $(addprefix $(OBJDIR), $(FILES:.cpp=.o))

CC = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98

RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
PURPLE = \033[0;35m
CYAN = \033[0;36m
WHITE = \033[0;37m
NC = \033[0m
.PHONY: all clean fclean re
.DEFAULT : re

all: head $(BINDIR)$(NAME)
	@echo "$(BLUE)█ $(YELLOW)Creating$(RESET) $(NAME):\r\t\t\t\t\t\t\t$(GREEN){DONE}$(BLUE) █$(RESET)"
	@echo "$(BLUE)██████████████████████ Compiling is DONE ███████████████████████$(RESET)"

head:
	@echo "$(BLUE)█████████████████████████ Making Files █████████████████████████$(RESET)"

$(BINDIR)$(NAME): $(OBJ)
	@if [ ! -d $(BINDIR) ];\
	then\
		echo "$(BLUE)█ $(YELLOW)Creating $(RESET) $(BINDIR):\r\t\t\t\t\t\t\t$(GREEN){DONE}$(BLUE) █$(RESET)";\
		mkdir $(BINDIR);\
	fi
	@$(CC) $(CFLAGS) $(OBJ) -o $(BINDIR)$(NAME) -I.
	@echo "$(BLUE)█ $(YELLOW)Compiling$(RESET) $(NAME):\r\t\t\t\t\t\t\t$(GREEN){DONE}$(BLUE) █$(RESET)"

$(OBJDIR)%.o: %.cpp
	@if [ ! -d $(OBJDIR) ];\
	then\
		echo "$(BLUE)█ $(YELLOW)Creating $(RESET) $(OBJDIR):\r\t\t\t\t\t\t\t$(GREEN){DONE}$(BLUE) █$(RESET)";\
		mkdir $(OBJDIR);\
	fi
	@echo "$(BLUE)█ $(YELLOW)Compiling$(RESET) $<:\r\t\t\t\t\t\t\t$(GREEN){DONE}$(BLUE) █$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@ -I.

clean:
	@if [ -d $(OBJDIR) ];\
	then\
		echo "$(RED)Deleting $(OBJDIR)$(NC)";\
		$(RM) -rf $(OBJDIR);\
	fi

fclean: clean
	@if [ -d $(BINDIR) ];\
	then\
		if [ -f $(BINDIR)$(NAME) ]; then\
			echo "$(RED)Deleting $(BINDIR)$(NAME)$(NC)";\
			$(RM) $(BINDIR)$(NAME);\
		fi;\
		echo "$(RED)Deleting $(BINDIR)$(NC)";\
		$(RM) -rf $(BINDIR);\
	fi

re: fclean all
