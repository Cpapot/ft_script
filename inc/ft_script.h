/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_script.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:07:22 by coco              #+#    #+#             */
/*   Updated: 2026/10/27 12:15:48 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCRIPT_H
# define SCRIPT_H

# include "../libft/includes/libft.h"
# include <stdbool.h>

# ifdef FT_SCRIPT_TEST
#  define SCRIPT_WRITE_FD parsing_test_fd
extern int parsing_test_fd;
# else
#  define SCRIPT_WRITE_FD 1
# endif

# define PATH_MAX	4096

# define ERROR_PRINT "ft_script: "
# define WARN_PRINT "\e[1;33m[WARN]: \e[0;37m"

enum e_echo_mode {
	ECHO_AUTO	= 0,
	ECHO_ALWAYS	= 1,
	ECHO_NEVER	= 2
};

enum e_logging_format {
	LFORMAT_CLASSIC		= 0,
	LFORMAT_ADVANCED	= 1
};

typedef struct s_script_settings
{
	char	*target;		//if none target will be typescript
	
	bool	isAppend;
	char	*command;				//if a command is given it will point to the corresponding argv, NULL otherwise
	int		echoMode;				//default mode is auto (0)
	bool	isReturn;
	bool	isFlush;
	bool	isForce;
	char	*logIo;
	char	*logIn;
	char	*logOut;
	char	*logTiming;
	int		loggingFormat;			//default Classic (0)
	off_t	outputLimit;			//default no limit (0)
	bool	isQuiet;
	char	*timing;
	
	bool	isClosing;
}	t_script_settings;

#endif
