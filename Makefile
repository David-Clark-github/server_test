SRCS	=	Server.cpp \
			configuration_file_parser.cpp

INC		=	./

NAME	=	cfg_file_parser

OBJS	=	$(SRCS:.cpp=.o)

CPPFLAGS	+= -std=c++98 -Wall -Wextra -Werror -g

CC		=	c++

all:	$(NAME)

clean:
	rm -f $(OBJS)

fclean:	clean
	rm -f $(NAME)

re:		fclean
	@make

%.o : %.cpp
	@$(CC) $(CPPFLAGS) -I $(INC) -o $@ -c $<

$(NAME): $(OBJS)
	@$(CC) $(CPPFLAGS) -I $(INC) $(OBJS) -o $(NAME)

.PHONY: all clean fclean re remove