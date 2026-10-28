/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_script.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:07:22 by coco              #+#    #+#             */
/*   Updated: 2026/10/28 09:51:03 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCRIPT_H
# define SCRIPT_H

# include "../libft/includes/libft.h"
# include <stdbool.h>
# include <errno.h>

# ifdef FT_SCRIPT_TEST
#  define SCRIPT_WRITE_FD parsing_test_fd
extern int parsing_test_fd;
# else
#  define SCRIPT_WRITE_FD 1
# endif

# define PATH_MAX	4096

# define ERROR_PRINT	"ft_script: "
# define HELP_INFO		"Try 'ft_script --help' for more information.\n"

enum e_echo_mode {
	ECHO_AUTO	= 0,
	ECHO_ALWAYS	= 1,
	ECHO_NEVER	= 2
};

enum e_logging_format {
	LFORMAT_CLASSIC		= 0,
	LFORMAT_ADVANCED	= 1
};

enum	e_return {
	SUCCES = 0,
	FAILED = 1
};

typedef struct s_script_settings
{
	char	*logOut;		//if none logOut will be typescript
	bool	isAppend;
	char	*command;				//if a command is given it will point to the corresponding argv, NULL otherwise
	int		echoMode;				//default mode is auto (0)
	bool	isReturn;
	bool	isFlush;
	bool	isForce;
	char	*logIn;
	char	*logTiming;
	int		loggingFormat;			//default Classic (0)
	off_t	outputLimit;			//default no limit (0)
	bool	isQuiet;
	char	*timing;
	
	bool	isClosing;
}	t_script_settings;

typedef struct s_script_data
{
	t_script_settings	settings;
	
	int					logOutFd;
	int					logIoFd;
	int					logInFd;
	int					logTimingFd;
	
}	t_script_data;

int		print_error(char *errorType, char *errorInfo, bool printHelp, int retValue);
//DEBUG FUCTION TO REMOVE LATER
void	print_settings(t_script_settings data);

#endif
