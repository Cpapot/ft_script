/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_script.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:07:22 by coco              #+#    #+#             */
/*   Updated: 2026/10/26 14:09:56 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCRIPT_H
# define SCRIPT_H

# include "../libft/includes/libft.h"
# include <stdbool.h>

# define PATH_MAX	4096

# define ERROR_PRINT "\e[1;31m[ERROR]: \e[0;37m"
# define WARN_PRINT "\e[1;33m[WARN]: \e[0;37m"
# define MALLOC_ERROR "Error during allocation, closing program\n"

enum e_echo_mode {
	ECHO_ALWAYS	= 1,
	ECHO_NEVER	= 2,
	ECHO_AUTO	= 3
};

enum e_logging_format {
	LFORMAT_CLASSIC		= 1,
	LFORMAT_ADVANCED	= 2	
};

typedef struct s_script_settings
{
	char	*target;		//if none target will be typescript
	
	bool	isAppend;
	char	*command;				//if a command is given it will point to the corresponding argv, NULL otherwise
	int		echoMode;				//default mode is auto (3)
	bool	isReturn;
	bool	isFlush;
	bool	isForce;
	char	*logIo;
	char	*logIn;
	char	*logOut;
	char	*logTiming;
	int		loggingFormat;			//default Classic (1)
	off_t	outputLimit;			//default no limit (0)
	bool	isQuiet;
	char	*timing;
}	t_script_settings;

#endif
