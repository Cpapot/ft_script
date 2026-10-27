#				______ _ _
#				|  ____(_) |
#				| |__   _| | ___  ___
#				|  __| | | |/ _ \/ __|
#				| |    | | |  __/\__ \.
#				|_|    |_|_|\___||___/

HEADERS		=	ft_script.h				\
				parsing.h				\
				files.h					\
				print.h

SRCS		=	main.c						\
				parsing/parsing.c			\
				parsing/parsing_utils.c		\
				files.c						\
				print.c

TEST_NAME	= tests/test_parsing

TEST_OUTPUT	= tests/parsing_output.log

TEST_SRCS	= tests/test_parsing.c				\
				srcs/parsing/parsing.c			\
				srcs/parsing/parsing_utils.c		\
				srcs/print.c

LIBFTSRC	=	libftprintf.a libft.a printffd.a

#					Directories

UTILSDIR		=	./srcs/utils/

HEADERSDIR		=	./inc/

SRCSDIR			=	./srcs/

LIBFTDIR		=	./libft/

OBJSDIR			=	./.objs/

#					Full Path

HEAD		=	$(addprefix $(HEADERSDIR),$(HEADERS))

SRCSPATH	=	$(addprefix $(SRCSDIR),$(SRCS))

LIBFT		=	$(addprefix $(LIBFTDIR),$(LIBFTSRC))

OBJS		=	$(addprefix $(OBJSDIR), $(SRCSPATH:.c=.o))

#		 __      __        _       _     _
#		 \ \    / /       (_)     | |   | |
#		  \ \  / /_ _ _ __ _  __ _| |__ | | ___  ___
#		   \ \/ / _` | '__| |/ _` | '_ \| |/ _ \/ __|
#		    \  / (_| | |  | | (_| | |_) | |  __/\__ \.
#		     \/ \__,_|_|  |_|\__,_|_.__/|_|\___||___/

define HEADER
"\e[2J\e[H
\033[1;36m
█████ █████       ████  ███  ████  ███ ████  █████   
█░░░░░ ░█░░░     █ ░░░░█ ░░░ █░░░█  █░░█░░░█  ░█░░░  
████░░░ █░░░░     ███░░█░ ░░░████░░ █░░████░░  █░░░░ 
█░░░░   █░░        ░░█ █░░   █░░█░ ░█░░█░░░░ ░ █░░   
█░░░░░  █░░ ████ ████░░ ███  █░░░█░███░█░░░░░  █░░   
 ░░      ░░  ░░░░ ░░░░ ░ ░░░  ░░  ░ ░░░ ░░      ░░   
  ░       ░   ░░░░ ░░░░   ░░░  ░   ░ ░░░ ░       ░ 
\033[0m
endef
export HEADER

RED			=	\033[1;31m
GREEN		=	\033[1;32m
YELLOW		=	\033[1;33m
BLUE		=	\033[1;34m
CYAN		=	\033[1;36m
DEFAULT		=	\033[0m
SUPPR		=	\r\033[2K

NAME		=	ft_script

AR			=	ar rc

CFLAGS		=	-Wall -Wextra -Werror -pthread
TEST_CFLAGS	=	${CFLAGS} -DFT_SCRIPT_TEST

CC			=	cc

RM			=	rm -rf

MKDIR		=	mkdir -p

MUTE		:=	1

#				  _____       _
#				 |  __ \     | |
#				 | |__) |   _| | ___  ___
#				 |  _  / | | | |/ _ \/ __|
#				 | | \ \ |_| | |  __/\__ \.
#				 |_|  \_\__,_|_|\___||___/

all : header lib ${NAME}

${NAME}: $(OBJS) $(LIBFT)
ifeq ($(MUTE),1)
	@${CC} ${OBJS} ${LIBFT} -o ${NAME} $(FLAGS) -lm
	@echo -n "${SUPPR} ${GREEN}	${NAME} : 🆗${DEFAULT}\n"
else
	${CC} ${OBJS} ${LIBFT} -o ${NAME} $(FLAGS)
endif

$(OBJSDIR)%.o: %.c ${HEAD}
	@$(MKDIR) .objs
	@$(MKDIR) $(dir $@)
ifeq ($(MUTE),1)
	@echo -n "${YELLOW}${SUPPR}	⌛ Creating ft_ping objects : $@"
	@$(CC) ${CFLAGS} -c $< -o $@ -I$(HEADERSDIR) -Ilibft/includes
else
	$(CC) ${CFLAGS} -c $< -o $@ -I$(HEADERSDIR) -Ilibft/includes
endif

clean:
	@${MAKE} clean -C ${LIBFTDIR}
	@${RM} ${OBJS}
	@${RM} ${TEST_NAME} ${TEST_OUTPUT}

fclean:
	@echo "${RED}🗑  ${NAME} cleanded"
	@${RM} ${OBJS}
	@${RM} ${OBJSDIR}
	@${RM} ${NAME}
	@${RM} ${TEST_NAME} ${TEST_OUTPUT}
	@${MAKE} fclean --no-print-directory -C ${LIBFTDIR}

reobj:
	rm -rf $(OBJSDIR)
	@${MAKE} --no-print-directory all

header:
	@echo "$$HEADER"

re:
	@${MAKE} --no-print-directory fclean
	@${MAKE} --no-print-directory all

lib:
	@${MAKE} --no-print-directory -C ${LIBFTDIR}

${TEST_NAME}: ${TEST_SRCS} ${LIBFT}
	@${CC} ${TEST_CFLAGS} ${TEST_SRCS} ${LIBFT} -o ${TEST_NAME} -I${HEADERSDIR} -Ilibft/includes

test: lib ${TEST_NAME}
	@./${TEST_NAME}

.PHONY : re all clean fclean printf lib header test
