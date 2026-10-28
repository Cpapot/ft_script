/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 15:00:10 by coren             #+#    #+#             */
/*   Updated: 2026/10/28 09:24:56 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "print.h"

int		print_error(char *errorType, char *errorInfo, bool printHelp, int retValue)
{
	write(SCRIPT_WRITE_FD, ERROR_PRINT, ft_strlen(ERROR_PRINT));
	write(SCRIPT_WRITE_FD, errorType, ft_strlen(errorType));
	if (errorInfo != NULL)
		write(SCRIPT_WRITE_FD, errorInfo, ft_strlen(errorInfo));
	write(SCRIPT_WRITE_FD, "\n", 1);
	if (printHelp)
		write(SCRIPT_WRITE_FD, HELP_INFO, ft_strlen(HELP_INFO));

	return retValue;
}

void	print_start_info(t_script_data data)
{
	if (!data.settings.isQuiet)
	{
		write(SCRIPT_WRITE_FD, START_MESSAGE, ft_strlen(START_MESSAGE));
		write(SCRIPT_WRITE_FD, data.settings.target, ft_strlen(data.settings.target));
		write(SCRIPT_WRITE_FD, "`\n", 2);
	}
}

//DEBUG FUCTION TO REMOVE LATER
void print_settings(t_script_settings data)
{
	ft_printf("target: %s\n", data.target);
	ft_printf("isAppend: %s\n", data.isAppend ? "true" : "false");
	ft_printf("command: %s\n", data.command);
	ft_printf("echoMode: %d\n", data.echoMode);
	ft_printf("isReturn: %s\n", data.isReturn ? "true" : "false");
	ft_printf("isFlush: %s\n", data.isFlush ? "true" : "false");
	ft_printf("isForce: %s\n", data.isForce ? "true" : "false");
	ft_printf("logIo: %s\n", data.logIo);
	ft_printf("logIn: %s\n", data.logIn);
	ft_printf("logTiming: %s\n", data.logTiming);
	ft_printf("loggingFormat: %d\n", data.loggingFormat);
	ft_printf("outputLimit: %u\n", (unsigned int)data.outputLimit);
	ft_printf("isQuiet: %s\n", data.isQuiet ? "true" : "false");
	ft_printf("timing: %s\n", data.timing);
	ft_printf("isClosing: %s\n", data.isClosing ? "true" : "false");
}