NAME		= PmergeMe

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++17

SRCDIR		= .
OBJDIR		= obj

SRCS		= main.cpp \
			  PmergeMe.cpp

#DEBUG_SRCS	= main_debug.cpp \
			  PmergeMeDebug.cpp

OBJS		= $(SRCS:%.cpp=$(OBJDIR)/%.o)
#DEBUG_OBJS	= $(DEBUG_SRCS:%.cpp=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

#debug: fclean $(DEBUG_OBJS)
#	$(CXX) $(CXXFLAGS) -o $(NAME) $(DEBUG_OBJS)

.PHONY: all clean fclean re #debug
